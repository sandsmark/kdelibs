/*
 *
 *  This file is part of the KDE libraries
 *  Copyright (c) 2000 Waldo Bastian <bastian@kde.org>
 *  Copyright (c) 2000 David Faure <faure@kde.org>
 *  Copyright (c) 2000 Stephan Kulow <coolo@kde.org>
 *
 * $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "slavebase.h"

#include <config.h>

#include <sys/time.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>		// Needed on some systems.
#endif

#include <assert.h>
#include <kdebug.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <qfile.h>

#include <ksock.h>
#include <kcrash.h>
#include <kdesu/client.h>
#include <kprotocolmanager.h>

#include "kio/slavebase.h"
#include "kio/slaveinterface.h"
#include "kio/connection.h"

using namespace KIO;

template class QList<QValueList<UDSAtom> >;

#define KIO_DATA QByteArray data; QDataStream stream( data, IO_WriteOnly ); stream

namespace KIO {
class SlaveBasePrivate {
public:
    QString slaveid;
    bool resume:1;
    bool needSendCanResume:1;
};
};


//////////////

SlaveBase::SlaveBase( const QCString &protocol,
                      const QCString &pool_socket,
                      const QCString &app_socket )
    : mProtocol(protocol), m_pConnection(0),
      mPoolSocket( QFile::decodeName(pool_socket)),
      mAppSocket( QFile::decodeName(app_socket))
{
    KCrash::setCrashHandler( sigsegv_handler );
    signal( SIGPIPE, sigpipe_handler );

    appconn = new Connection();
    listEntryCurrentSize = 0;
    struct timeval tp;
    gettimeofday(&tp, 0);
    listEntry_sec = tp.tv_sec;
    listEntry_usec = tp.tv_usec;
    mConnectedToApp = true;
    connectSlave(mAppSocket);

    d = new SlaveBasePrivate;
    // by kahl for netmgr (need a way to identify slaves)
    d->slaveid = protocol;
    d->slaveid += QString::number(getpid());
    d->resume = false;
    d->needSendCanResume = false;
}

SlaveBase::~SlaveBase()
{
    delete d;
}

void SlaveBase::dispatchLoop()
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    while (true) {
	FD_ZERO(&rfds);

	if (appconn->inited())
	  FD_SET(appconn->fd_from(), &rfds);

	/* Wait up to 30 seconds. */
	tv.tv_sec = 30;
	tv.tv_usec = 0;

	retval = select(appconn->fd_from()+ 1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval > 0)
        {
	    if (FD_ISSET(appconn->fd_from(), &rfds))
            { // dispatch application messages
		int cmd;
		QByteArray data;
                if ( appconn->read(&cmd, data) != -1 )
                {
                  if (cmd == CMD_SLAVE_CONNECT)
                  {
                     QString app_socket;
                     QDataStream stream( data, IO_ReadOnly);
                     stream >> app_socket;
                     kdDebug(7019) << "slavewrapper: Connecting to new app (" << app_socket << ")." << endl;
                     appconn->send( MSG_SLAVE_ACK );
                     disconnectSlave();
                     mConnectedToApp = true;
                     connectSlave(app_socket);
                  }
                  else
                  {
                     dispatch(cmd, data);
                  }
                }
                else // some error occured, perhaps no more application
                {
// When the app exits, should the slave be put back in the pool ?
                  if (mConnectedToApp)
                  {
                     kdDebug(7019) << "slavewrapper: Communication with app lost. Returning to slave pool." << endl;
                     disconnectSlave();
                     mConnectedToApp = false;
                     connectSlave(mPoolSocket);
                  }
                  else
                  {
                     kdDebug(7019) << "slavewrapper: Communication with pool lost. Exiting." << endl;
                     exit(0);
                  }
                }
	    }
	}
        else if (retval == -1) // error
        {
          kdDebug(7019) << "slavewrapper: select returned error " << (errno==EBADF?"EBADF":errno==EINTR?"EINTR":errno==EINVAL?"EINVAL":errno==ENOMEM?"ENOMEM":"unknown") << " (" << errno << ")" << endl;
          exit(0);
        }
    }
}

void SlaveBase::connectSlave(const QString& path)
{
    appconn->init(new KSocket(QFile::encodeName(path)));
    setConnection(appconn);
}

void SlaveBase::disconnectSlave()
{
    appconn->close();
}

void SlaveBase::setMetaData(const QString &key, const QString &value)
{
   mOutgoingMetaData.replace(key, value);
}

QString SlaveBase::metaData(const QString &key)
{
   if (!mIncomingMetaData.contains(key))
      return QString::null;
   return mIncomingMetaData[key];
}

void SlaveBase::sendMetaData()
{
   KIO_DATA << mOutgoingMetaData;

   m_pConnection->send( INF_META_DATA, data );
   mOutgoingMetaData.clear(); // Clear
}


void SlaveBase::data( const QByteArray &data )
{
    if (!mOutgoingMetaData.isEmpty())
       sendMetaData();
    m_pConnection->send( MSG_DATA, data );
}

void SlaveBase::dataReq( )
{
    if (!mOutgoingMetaData.isEmpty())
       sendMetaData();
    if (d->needSendCanResume)
        canResume(0);
    m_pConnection->send( MSG_DATA_REQ );
}

void SlaveBase::error( int _errid, const QString &_text )
{
    mIncomingMetaData.clear(); // Clear meta data
    mOutgoingMetaData.clear();
    KIO_DATA << _errid << _text;

    m_pConnection->send( MSG_ERROR, data );
}

void SlaveBase::connected()
{
    m_pConnection->send( MSG_CONNECTED );
}

void SlaveBase::finished()
{
    mIncomingMetaData.clear(); // Clear meta data
    if (!mOutgoingMetaData.isEmpty())
       sendMetaData();
    m_pConnection->send( MSG_FINISHED );
}

void SlaveBase::needSubURLData()
{
    m_pConnection->send( MSG_NEED_SUBURL_DATA );
}

void SlaveBase::slaveStatus( const QString &host, bool connected )
{
    pid_t pid = getpid();
    Q_INT8 b = connected ? 1 : 0;
    KIO_DATA << pid << mProtocol << host << b;
    m_pConnection->send( MSG_SLAVE_STATUS, data );
}

void SlaveBase::canResume()
{
    m_pConnection->send( MSG_CANRESUME );
}

void SlaveBase::totalSize( unsigned long _bytes )
{
    KIO_DATA << _bytes;
    m_pConnection->send( INF_TOTAL_SIZE, data );
}

void SlaveBase::processedSize( unsigned long _bytes )
{
    KIO_DATA << _bytes;
    m_pConnection->send( INF_PROCESSED_SIZE, data );
}

void SlaveBase::speed( unsigned long _bytes_per_second )
{
    KIO_DATA << _bytes_per_second;
    m_pConnection->send( INF_SPEED, data );
}

void SlaveBase::redirection( const KURL& _url )
{
    KIO_DATA << _url;
    m_pConnection->send( INF_REDIRECTION, data );
}

void SlaveBase::errorPage()
{
    m_pConnection->send( INF_ERROR_PAGE );
}

void SlaveBase::mimeType( const QString &_type)
{
    KIO_DATA << _type;
    m_pConnection->send( INF_MIME_TYPE, data );
    int cmd = 0;
    while(true)
    {
       if ( m_pConnection->read( &cmd, data ) == -1 ) {
           kdDebug(7019) << "SlaveBase: mimetype: read error" << endl;
           ::exit(255);
       }
       if ( (cmd == CMD_REPARSECONFIGURATION) ||
            (cmd == CMD_META_DATA) ||
            (cmd == CMD_SUBURL) )
       {
          dispatch( cmd, data );
          continue; // Disguised goto
       };
       break;
    }
// WABA: cmd can be "CMD_NONE" or "CMD_GET" (in which case the slave
// had been put on hold.) [or special, for http posts].
// Something else is basically an error
    kdDebug(7019) << "mimetype: reading " << cmd << endl;
    ASSERT( (cmd == CMD_NONE) || (cmd == CMD_GET) || (cmd == CMD_SPECIAL) );
}

// ?
void SlaveBase::gettingFile( const QString &file)
{
    KIO_DATA << file;
    m_pConnection->send( INF_GETTING_FILE, data );
}

void SlaveBase::warning( const QString &_msg)
{
    KIO_DATA << _msg;
    m_pConnection->send( INF_WARNING, data );
}

void SlaveBase::infoMessage( const QString &_msg)
{
    KIO_DATA << _msg;
    m_pConnection->send( INF_INFOMESSAGE, data );
}

bool SlaveBase::requestNetwork(const QString& host)
{
    KIO_DATA << host << d->slaveid;
    m_pConnection->send( MSG_NET_REQUEST, data );

    if ( waitForAnswer( INF_NETWORK_STATUS, 0, data ) != -1 )
    {
        bool status;
        QDataStream stream( data, IO_ReadOnly );
        stream >> status;
        kdDebug(7019) << "got " << status << endl;
        return status;
    } else
        return false;
}

void SlaveBase::dropNetwork(const QString& host)
{
    KIO_DATA << host << d->slaveid;
    m_pConnection->send( MSG_NET_DROP, data );
}

void SlaveBase::statEntry( const UDSEntry& entry )
{
    KIO_DATA << entry;
    m_pConnection->send( MSG_STAT_ENTRY, data );
}

void SlaveBase::listEntry( const UDSEntry& entry, bool _ready )
{
    static struct timeval tp;
    static const int maximum_updatetime = 800;
    static const int minimum_updatetime = (maximum_updatetime * 3) / 4;

    if (!_ready) {
	pendingListEntries.append(entry);

	if (pendingListEntries.count() > listEntryCurrentSize) {

            gettimeofday(&tp, 0);

            long diff = ((tp.tv_sec - listEntry_sec) * 1000000 +
                         tp.tv_usec - listEntry_usec) / 1000;

            if (diff > maximum_updatetime) {
                listEntryCurrentSize = listEntryCurrentSize * 3 / 4;
		_ready = true;
            } else if (diff < minimum_updatetime) {
                listEntryCurrentSize = listEntryCurrentSize * 5 / 4;
            } else {
		_ready = true;
	    }
        }
    }

    if (_ready) { // may happen when we started with !ready
	gettimeofday(&tp, 0);
	listEntry_sec = tp.tv_sec;
	listEntry_usec = tp.tv_usec;

	listEntries( pendingListEntries );
	pendingListEntries.clear();
    }
}

void SlaveBase::listEntries( const UDSEntryList& list )
{
    kdDebug(7019) << "listEntries " << list.count() << endl;

    KIO_DATA << list.count();
    UDSEntryListConstIterator it = list.begin();
    UDSEntryListConstIterator end = list.end();
    for (; it != end; ++it)
	stream << *it;
    m_pConnection->send( MSG_LIST_ENTRIES, data);
}

void SlaveBase::sigsegv_handler (int)
{
    // Debug and printf should be avoided because they might
    // call malloc.. and get in a nice recursive malloc loop
    write(2, "kioslave : ###############SEG FAULT#############\n", 49);
    exit(1);
}

void SlaveBase::sigpipe_handler (int)
{
    // TODO: maybe access the only instance of SlaveBase and call abort() on it
    // Default implementation of abort would exit(1), but specific slaves can
    // abort in a nicer way and be ready for more invocations
    kdDebug(7019) << "SIGPIPE" << endl;
    exit(1);
}

void SlaveBase::setHost(QString const &host, int, QString const &, QString const &)
{
    kdDebug( 7019 ) << "setHost( host = " << host << ")" << endl;
}

void SlaveBase::openConnection(void)
{ error(  ERR_UNSUPPORTED_ACTION, "open" ); }
void SlaveBase::closeConnection(void)
{ error(  ERR_UNSUPPORTED_ACTION, "close" ); }
void SlaveBase::stat(KURL const &)
{ error(  ERR_UNSUPPORTED_ACTION, "stat" ); }
void SlaveBase::put(KURL const &, int, bool, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "put" ); }
void SlaveBase::special(QArray<char> const &)
{ error(  ERR_UNSUPPORTED_ACTION, "special" ); }
void SlaveBase::listDir(KURL const &)
{ error(  ERR_UNSUPPORTED_ACTION, "listDir" ); }
void SlaveBase::get(KURL const & )
{ error(  ERR_UNSUPPORTED_ACTION, "get" ); }
void SlaveBase::mimetype(KURL const &url)
{ get(url); }
void SlaveBase::rename(KURL const &, KURL const &, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "rename" ); }
void SlaveBase::symlink(QString const &, KURL const &, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "symlink" ); }
void SlaveBase::copy(KURL const &, KURL const &, int, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "copy" ); }
void SlaveBase::del(KURL const &, bool)
{ error(  ERR_UNSUPPORTED_ACTION, "del" ); }
void SlaveBase::mkdir(KURL const &, int)
{ error(  ERR_UNSUPPORTED_ACTION, "mkdir" ); }
void SlaveBase::chmod(KURL const &, int)
{ error(  ERR_UNSUPPORTED_ACTION, "chmod" ); }
void SlaveBase::setSubURL(KURL const &)
{ error(  ERR_UNSUPPORTED_ACTION, "suburl" ); }

void SlaveBase::slave_status()
{ slaveStatus( QString::null, false ); }

void SlaveBase::reparseConfiguration()
{
}

bool SlaveBase::dispatch()
{
    assert( m_pConnection );

    int cmd;
    QByteArray data;
    if ( m_pConnection->read( &cmd, data ) == -1 )
	return false;

    dispatch( cmd, data );
    return true;
}

bool SlaveBase::openPassDlg( const QString& msg, QString& user, QString& passwd, bool lockUserName )
{
    kdDebug(7019) << "SlaveBase: OpenPassDlg: User= " << user << ", Message= " << msg << ", LockUserName= " << lockUserName << endl;
    KIO_DATA << msg << user << lockUserName;
    m_pConnection->send( INF_NEED_PASSWD, data );
    int cmd;
    if ( waitForAnswer( CMD_USERPASS, CMD_NONE, data, &cmd ) != -1 && cmd == CMD_USERPASS )
    {
        QDataStream stream( data, IO_ReadOnly );
        stream >> user >> passwd;
        kdDebug(7019) << "Got " << user << ":" << passwd << endl;
        return true;
    }
    return false;
}

int SlaveBase::messageBox( int type, const QString &text, const QString &caption, const QString &buttonYes, const QString &buttonNo )
{
    kdDebug(7019) << "messageBox " << type << " " << text << " - " << caption << buttonYes << buttonNo << endl;
    KIO_DATA << type << text << caption << buttonYes << buttonNo;
    m_pConnection->send( INF_MESSAGEBOX, data );
    if ( waitForAnswer( CMD_MESSAGEBOXANSWER, 0, data ) != -1 )
    {
        QDataStream stream( data, IO_ReadOnly );
        int answer;
        stream >> answer;
        kdDebug(7019) << "got messagebox answer" << answer << endl;
        return answer;
    } else
        return 0; // communication failure
}

bool SlaveBase::canResume( unsigned long offset )
{
    kdDebug() << "SlaveBase::canResume offset=" << offset << endl;
    d->needSendCanResume = false;
    KIO_DATA << offset;
    m_pConnection->send( MSG_RESUME, data );
    int cmd;
    if ( waitForAnswer( CMD_RESUMEANSWER, CMD_NONE, data, &cmd ) != -1 )
    {
        kdDebug() << "SlaveBase::canResume returning " << (cmd == CMD_RESUMEANSWER) << endl;
        return cmd == CMD_RESUMEANSWER;
    } else
        return false;
}



int SlaveBase::waitForAnswer( int expected1, int expected2, QByteArray & data, int *pCmd )
{
    int cmd, result;
    for (;;)
    {
        result = m_pConnection->read( &cmd, data );
        if ( result == -1 )
            return -1;
        if ( cmd == expected1 || cmd == expected2 )
        {
            if ( pCmd ) *pCmd = cmd;
            return result;
        }
        if ( cmd == CMD_SLAVE_STATUS || cmd == CMD_REPARSECONFIGURATION || cmd == CMD_META_DATA )
            dispatch( cmd, data );
        else
            kdWarning() << "Got cmd " << cmd << " while waiting for an answer!" << endl;
    }
}


int SlaveBase::readData( QByteArray &buffer)
{
   int result = waitForAnswer( MSG_DATA, 0, buffer );
   //kdDebug(7019) << "readData: length = " << result << " " << endl;
   return result;
}


void SlaveBase::dispatch( int command, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    KURL url;
    int i;

    switch( command ) {
    case CMD_HOST: {
        QString passwd;
        QString host, user;
	stream >> host >> i >> user >> passwd;
	setHost( host, i, user, passwd );
    }
    break;
    case CMD_CONNECT:
	openConnection( );
	break;
    case CMD_DISCONNECT:
	closeConnection( );
	break;
    case CMD_SLAVE_STATUS:
        slave_status();
        break;
    case CMD_REPARSECONFIGURATION:
        KProtocolManager::reparseConfiguration();
        reparseConfiguration();
	break;
    case CMD_GET: {
        stream >> url;
	get( url );
    }
    break;
    case CMD_PUT: {
	int permissions;
	Q_INT8 iOverwrite, iResume;

	stream >> url >> iOverwrite >> iResume >> permissions;

	bool overwrite = ( iOverwrite != 0 );
	bool resume = ( iResume != 0 );

        // Remember that we need to send canResume(), TransferJob is expecting
        // it. Well, in theory this shouldn't be done if resume is true.
        //   (the resume bool is currently unused)
        d->needSendCanResume = true   /* !resume */;

	put( url, permissions, overwrite, resume);
    }
    break;
    case CMD_STAT:
	stream >> url;
	stat( url );
	break;
    case CMD_MIMETYPE:
	stream >> url;
	mimetype( url );
	break;
    case CMD_LISTDIR:
	stream >> url;
	listDir( url );
	break;
    case CMD_MKDIR:
	stream >> url >> i;
	mkdir( url, i );
	break;
    case CMD_RENAME: {
	Q_INT8 iOverwrite;
        KURL url2;
        stream >> url >> url2 >> iOverwrite;
        bool overwrite = (iOverwrite != 0);
        rename( url, url2, overwrite );
    }
    break;
    case CMD_SYMLINK: {
	Q_INT8 iOverwrite;
        QString target;
        stream >> target >> url >> iOverwrite;
        bool overwrite = (iOverwrite != 0);
        symlink( target, url, overwrite );
    }
    break;
    case CMD_COPY: {
        int permissions;
        Q_INT8 iOverwrite;
        KURL url2;
        stream >> url >> url2 >> permissions >> iOverwrite;
        bool overwrite = (iOverwrite != 0);
        copy( url, url2, permissions, overwrite );
    }
    break;
    case CMD_DEL: {
        Q_INT8 isFile;
        stream >> url >> isFile;
	del( url, isFile != 0);
    }
    break;
    case CMD_CHMOD:
        stream >> url >> i;
	chmod( url, i);
	break;
    case CMD_SPECIAL:
	special( data );
	break;
    case CMD_META_DATA:
        stream >> mIncomingMetaData;
	break;
    case CMD_SUBURL:
        stream >> url;
        setSubURL(url);
        break;
    case CMD_NONE:
	fprintf(stderr, "Got unexpected CMD_NONE!\n");
	break;
	case CMD_DEL_AUTHORIZATION: // TODO in slaveinterface
	{
	    QString key;
	    stream >> key;
	    delCachedAuthentication( key );
	    break;
	}
    default:
	assert( 0 );
    }
}

QString SlaveBase::createAuthCacheKey( const KURL& url )
{
    if( url.isMalformed() )
        return QString::null;

    // Generate the basic key sequence.
    QString key = url.protocol() + "-" + url.host();
    int port = url.port();
    if( port )
      key += ':' + QString::number( port );

    return key;
}

bool SlaveBase::pingCacheDaemon() const
{
    KDEsuClient client;
    int sucess = client.ping();
    if( sucess == -1 )
    {
        kdDebug(7019) << "No running \"kdesu\" daemon found. Attempting to start one..." << endl;
        sucess = client.startServer();
        if( sucess == -1 )
        {
            kdDebug(7019) << "Cannot start a new \"kdesu\" deamon!!" << endl;
            return false;
        }
        kdDebug(7019) << "New \"kdesu\" daemon was sucessfully started..." << endl;
    }
    return true;
}

bool SlaveBase::checkCachedAuthentication(const KURL& url, QString& user, QString& passwd )
{
    QString key, extra;
    return checkCachedAuthentication(url, user, passwd, key, extra, false);
}

bool SlaveBase::checkCachedAuthentication( const KURL& url,
                                          QString& user,
                                          QString& passwd,
                                          QString& realm,
                                          QString& extra,
                                          bool verify_path )
{
    QString auth_key = createAuthCacheKey( url );
    if( auth_key.isNull() )
        return false;

    if( !pingCacheDaemon() )
    	return false;

    KDEsuClient client;

    // Always check to see if we have a username entry that is not empty
    // for a given key.  If it is, we then already know that the supplied
    // URL has no cached password and simply return false. This should speed
    // things up for URLs that do not have cached passwords since there will
    // effectively be only one trip to the daemon.
    QString u = QString::fromUtf8(client.getVar((auth_key + "-user").utf8()));
    if( !u.isEmpty() )
    {
        // Deal with protection space based authentications, namely HTTP.
	    // It has by far the most complex scheme in terms of password
    	// caching requirements. (DA)
	    if( verify_path )
    	{
	        bool found = false;
    	    QString new_path = url.path(); // The new path...
        	if( new_path.isEmpty() )
        		new_path = '/';

			// Get all stored_paths for the specified key...
    	    QStringList stored_path;
        	stored_path << QString::fromUtf8( client.getVar((auth_key + "-path").utf8()));
	        int count = QString::fromUtf8( client.getVar((auth_key + "-dupcount").utf8() )).toInt();
    	    if( count > 0 )
        	{
                bool realmNotEmpty = !realm.isEmpty();
				for( int i=1; i <= count; i++ )
				{
				 	stored_path << QString::fromUtf8( client.getVar((auth_key + QString(":%1").arg( i ) + "-path").utf8()));
				 	if( realmNotEmpty )
				 		stored_path << QString::fromUtf8( client.getVar((auth_key + QString(":%1").arg( i ) + "-realm").utf8()));
				}
	    	}

			// Tests for exact path (filename included) match first.
			// If it fails, then simply try the less stricter directory
			// prefix match. (DA)
			count = 0;
    		for( QStringList::Iterator it = stored_path.begin();
    			 it != stored_path.end(); ++it )
	    	{
    			if( new_path == (*it) )
    			{
    				if( !realm.isEmpty() )
    				{
		            	if( (*(++it)) == realm )
        		    	{
    						if( count > 0 )
	    						auth_key += QString("%1").arg(count);	// Update the Authentication key...
				  			kdDebug(7019) << "STRICT MATCH: Relam check passed: " << (*it) << endl;
		                	found = true;          // Realm value mis-match!!
		                	break;
        		      	}
    				}
    				else
    				{
    					if( count > 0 )
    						auth_key += QString("%1").arg(count);	// Update the Authentication key...
	    				found = true;
	    				break;
    				}
    			}
    			count++;
    		}

	    	if( !found )
    		{
    			// Now we can attempt the directory prefix match, i.e.
    			// testing whether the directiory of the new URL contains
    			// the stored_one.  If it does, then we have a match...
                // Also, we make a best effort attempt not to fall for a
                // match b/n "/me" with "/me_and_you".
	            int last_slash = new_path.findRev( '/' );
				if( last_slash > 0 )
					new_path.truncate( last_slash+1 );

  	    	    count = 0;
	    		for( QStringList::Iterator it = stored_path.begin();
    				 it != stored_path.end(); ++it )
	    		{
					QString str = (*it);
					last_slash = str.findRev( '/' );
					if( last_slash > 0 )
						str.truncate( last_slash+1 );
   					if(new_path.startsWith(str))
		      		{
	    				if( !realm.isEmpty() )
    					{
		    	        	if( (*(++it)) == realm )
        			    	{
    							if( count > 0 )
	    							auth_key += QString("%1").arg(count);	// Update the Authentication key...
			                	found = true;          // Realm value mis-match!!
			                	break;
        			      	}
        				}
						else
						{
							if( count > 0 )
								auth_key += QString(":%1").arg( count );  // Update the auth_key.
		      				found = true;
							break;
						}
					}
					 count++;
				}
			}

	   	    if( !found )
        	{
            	kdDebug(7019) << "Cached Authentication NOT found!" << endl;
	            return false;
    	    }
    	}

    	// Now we obtain the cached Authentication if the user
    	// name is empty or there is a match b/n the stored
    	// and the supplied one.
    	if( ( user.isEmpty() || (!user.isEmpty() && u == user)) )
	    {
    	    if( user.isEmpty() ) { user = u; }
        	passwd = QString::fromUtf8(client.getVar((auth_key + "-pass").utf8()));
	        if( realm.isEmpty() ) { realm = QString::fromUtf8( client.getVar((auth_key + "-realm").utf8())); }
    	    extra = QString::fromUtf8( client.getVar((auth_key + "-extra").utf8()));
        	kdDebug(7019) << "Found cached authentication entry: " << endl
	                      << "  User= " << user << endl
    	                  << "  Password= [hidden]" << endl
        	              << "  Key = " << auth_key << endl
	                      << "  Extra= " << extra << endl;
    	    return true;
	    }
	}
    return false;
}

bool SlaveBase::cacheAuthentication( const KURL& url,
                                    const QString& user,
                                    const QString& passwd,
                                    const QString& realm,
                                    const QString& extra )
{
    // Do not allow caching if: URL is malformed or user name is
    // empty or the password is null!!  Empty password is acceptable
    // but not a NULL one. This is mainly intended as a partial defense
    // against incorrect or incorrect use of this function...
    QString auth_key = createAuthCacheKey( url );
    if( auth_key.isNull() || user.isEmpty() || passwd.isNull() )
        return false;

    if( !pingCacheDaemon() )
    	return false;

    kdDebug(7019) << "Attempting to cache Authentication for: " << auth_key << endl;
    KDEsuClient client;
    bool isCached = false;
    if( !realm.isEmpty() )
    {
        QString stored_realm = QString::fromUtf8(client.getVar((auth_key + "-realm").utf8()));
        if( stored_realm == realm )
            isCached = true;

        if( !isCached )
        {
            bool hasDuplicateEntry = false;
            int count = QString::fromUtf8( client.getVar((auth_key + "-dupcount").utf8() )).toInt( &hasDuplicateEntry );
            if( hasDuplicateEntry )
            {
                for( int i=1; i <= count; i++ )
                {
                    stored_realm = QString::fromUtf8( client.getVar((auth_key + QString(":%1").arg( i ) + "-relam").utf8()));
                    if( stored_realm == realm )
                    {
                        auth_key += QString(":%1").arg( i );  // Update the auth_key.
                        isCached = true;                      // Indicate that the auth_key was found
                        i = count+1;                          // Break out of loop gracefully.
                    }
                }
                // Hey, this must be a new protection space in a
                // multiple protection space scheme.  Updating accordingly...
                if( !isCached )
                {
                    client.setVar( (auth_key+"-dupcount").utf8(), QString("%1").arg( count+1 ).utf8(), 0, auth_key.utf8() );
                    auth_key += QString(":%1").arg( count+1 );  // Update
                }
            }
            else
            {
                // The first new protection space on the same server!!
                // Updating auth_key accordingly...
                if( !stored_realm.isEmpty() )
                {
                    client.setVar( (auth_key+"-dupcount").utf8(), QString("%1").arg( 1 ).utf8(), 0, auth_key.utf8() );
                    auth_key += QString(":%1").arg( 1 );
                    kdDebug(7019) << "New protection space for a known host!!  Creating new entry labled: " << auth_key << endl;
                }
            }
        }
    }
    else
    {
        QString u = QString::fromUtf8( client.getVar((auth_key + "-user").utf8()) );
        if( u == user )
            isCached = true;
    }

    // If Authentication has already been cached, simply increment the ref-counter
    if( isCached )
    {
        QString strRefCount = QString("%1").arg( client.getVar((auth_key + "-refcount").utf8()).toInt() + 1 );
        kdDebug(7019) << "Found an exisitng Authentication entry for: " << auth_key << endl
                      << ". Incrementing reference count to : " << strRefCount << endl;
        client.setVar( (auth_key + "-refcount").utf8(), strRefCount.utf8(), 0, auth_key.utf8() );
        // BTW, we need to update the extra field just in case it is different.
        // Important in "Digest authentication" for HTTP where the server can
        // change the "nonce" values from connection to connection :))
        if( !extra.isEmpty() )
        {
            QString e = QString::fromUtf8(client.getVar( (auth_key + "-extra").utf8() ));
            if( e != extra )
            {
                kdDebug(7019) << "Updating the \"extra\" auth_key: " << extra << endl;
                client.setVar( (auth_key + "-extra").utf8(), extra.utf8(), 0, auth_key.utf8() );
            }
        }
    }
    // Add new Authentication entry...
    else
    {
        client.setVar( (auth_key+"-user").utf8(), user.utf8(), 0, auth_key.utf8() );
        client.setVar( (auth_key+"-pass").utf8(), passwd.utf8(), 0, auth_key.utf8() );
        client.setVar( (auth_key+"-refcount").utf8(), QString("%1").arg(1).utf8(), 0, auth_key.utf8() );
        if( !realm.isEmpty() )
        {
            client.setVar( (auth_key + "-realm").utf8(), realm.utf8(), 0, auth_key.utf8() );
            QString new_path = url.path();
            if( new_path.isEmpty() )
            	new_path = '/';
            client.setVar( (auth_key+"-path").utf8(), new_path.utf8(), 0, auth_key.utf8() );
        }
        if( !extra.isEmpty() )
            client.setVar( (auth_key + "-extra").utf8(), extra.utf8(), 0, auth_key.utf8() );

        kdDebug(7019) << "Added NEW Authentication entry for: " << endl
                      << "  URL= " << url.url() << endl
                      << "  User= " << user << endl
                      << "  Password= [hidden]" << endl
                      << "  Realm= " << realm << endl
                      << "  Extra= " << extra << endl;

        // Nifty way to group passwords for automatic deletion later on ??
        // We will use the supplied keys which are unique.  We also keep
        // ref-count so that the group does not die a premature death!!
        // That is it is only deleted when all applications using it are
        // gone.  Or at least that is the plan.
        setMetaData( "authGroupKey", auth_key );
        return true;
    }
    return false;
}

void SlaveBase::delCachedAuthentication( const QString& grpname )
{
	if( !pingCacheDaemon() )
		return;

    bool ok;
    KDEsuClient client;
    int count = client.getVar( (grpname + "-refcount").utf8() ).toInt( &ok );
    // Test point though I know that this value is going to be an integer
	kdDebug(7019) << "Attempting to delete cached Authentication for: " << grpname << endl;
    if( ok )
    {
        kdDebug(7019) << "Deleting cached Authentication for: " << grpname << endl;
        if( count > 1 )
        {
            QString ref_count = QString("%1").arg( count - 1 );
            client.setVar( (grpname + "-refcount").utf8(), ref_count.utf8(), 0, grpname.utf8() );
            kdDebug(7019) << "Multiple applications use key. Decrementing reference count: " << ref_count << endl;
        }
        else
        {
            kdDebug(7019) << "No reference left or none found.  Deleting the whole group..." << endl;
            client.delGroup(grpname.utf8());
        }
    }
}
