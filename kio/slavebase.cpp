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

    // by kahl for netmgr (need a way to identify slaves)
    d = new SlaveBasePrivate;
    d->slaveid = protocol;
    d->slaveid += QString::number(getpid());
}

SlaveBase::~SlaveBase()
{
    // by kahl for netmgr
    if (d)
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

void SlaveBase::canResume( bool _resume )
{
    KIO_DATA << (int)_resume;
    m_pConnection->send( MSG_RESUME, data );
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
       if ( (cmd == CMD_META_DATA) ||
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

bool SlaveBase::openPassDlg( const QString& head, QString& user, QString& pass, const QString& key )
{
    kdDebug(7019) << "openPassDlg " << head << endl;
    KIO_DATA << key << head << user << pass;
    m_pConnection->send( INF_NEED_PASSWD, data );
    int cmd;
    if ( waitForAnswer( CMD_USERPASS, CMD_NONE, data, &cmd ) != -1 && cmd == CMD_USERPASS ) {
        QDataStream stream( data, IO_ReadOnly );
        stream >> user >> pass;
        kdDebug(7019) << "got " << user << "  [password hidden]" << endl;
        return true;
    } else
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
    default:
	assert( 0 );
    }
}

bool SlaveBase::checkCachedAuthentication(QString& user, QString& passwd, int& auth_type, QString& valid_path, const QString& realm)
{
    // no matter what, make sure our return types are either defined
    // or are null... nothing undefined
    user       = QString::null;
    passwd     = QString::null;
    auth_type  = 0;
    valid_path = QString::null;

    if( realm.isNull() )
        return false;

    KDEsuClient client;
    if( client.ping() == - 1 )
        return false;

    user = QString::fromUtf8( client.getVar( (realm + "-user").utf8() ) );
    passwd = QString::fromUtf8( client.getVar( (realm + "-pass").utf8() ) );
    valid_path = QString::fromUtf8( client.getVar( (realm + "-path").utf8() ) );

    QCString auth = client.getVar( (realm + "-auth").utf8() );
    auth_type = auth.isNull() ? 0 : auth.toInt();

    kdDebug(7019) << "Got cached info: key=" << realm << ", user=" << user << ", password=[hidden], path=" << valid_path << endl;

    return true;
}

void SlaveBase::cacheAuthentication(const KURL& url, const QString& user, const QString& password, int auth_type)
{
    KDEsuClient client;

    int success = client.ping();
    if (success == -1)
        success = client.startServer();

    if (success == -1)
        return;

    QString key(url.host());
    QString path;
    if (url.path().isNull())
      path = "/";
    else
      path = url.directory();

    kdDebug(7019) << "Caching: key=" << key << ", user=" << user << ", password=[hidden], path=" << path << endl;
    QCString auth;
    auth.setNum(auth_type);
    client.setVar( (key + "-user").utf8(), user.utf8() );
    client.setVar( (key + "-pass").utf8(), password.utf8() );
    client.setVar( (key + "-auth").utf8(), auth );
    client.setVar( (key + "-path").utf8(), path.utf8() );
}
