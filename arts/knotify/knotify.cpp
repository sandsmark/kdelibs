/*
   Copyright (c) 1997 Christian Esken (esken@kde.org)
                 2000 Charles Samuels (charles@kde.org)
                 2000 Stefan Schimanski (1Stein@gmx.de)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <unistd.h>
#include <signal.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kstddirs.h>
#include <kuniqueapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <soundserver.h>
#include <dispatcher.h>

#include "knotify.h"
#include "knotify.moc"

#include <qfile.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qiomanager.h>
#include <iostream.h>
#include <qtextstream.h>

/*
 * This is global because it needs to be initialized *before* the KNotify
 * DCOP object will accept requests (otherwise there may be reentrancy issues
 * and race conditions
 */

Arts::SimpleSoundServer g_soundServer;


bool connectSoundServer()
{
    /*
     * obtain an object reference to the soundserver - retry sometimes, so
     * it will work during the startup sequence, even if artsd is started
     * some time after the first process requests knotify to do some
     * notifications
     */
    for( int tries=0; tries<8; tries++ )
    {
        g_soundServer = Arts::Reference("global:Arts_SimpleSoundServer");
        if( !g_soundServer.isNull() ) return true;
        sleep( 1 );
    }

    return false;
}


int main(int argc, char **argv)
{
    KAboutData aboutdata("knotify", I18N_NOOP("KNotify"),
                         "2.0pre", I18N_NOOP("KDE Notification Server"),
                         KAboutData::License_GPL, "(C) 1997-2000, KDE Developers");
    aboutdata.addAuthor("Christian Esken",0,"esken@kde.org");
    aboutdata.addAuthor("Stefan Westerfeld",I18N_NOOP("Sound support"),"stefan@space.twc.de");
    aboutdata.addAuthor("Charles Samuels",I18N_NOOP("Current Maintainer"),"charles@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutdata );
    KUniqueApplication::addCmdLineOptions();

    // initialize application
    if ( !KUniqueApplication::start() ) {
        kdDebug() << "Running knotify found" << endl;
        return 0;
    }

    KUniqueApplication app;
    app.disableSessionManagement();

    // setup mcop communication
    Arts::QIOManager qiomanager;
    Arts::Dispatcher dispatcher(&qiomanager);

    if( !connectSoundServer() )
        kdDebug() << "artsd is not running, there will be no sound notifications." << endl;

    // start notify service
    KNotify notify;
    return app.exec();
}


KNotify::KNotify()
    : QObject(), DCOPObject("Notify")
{
}


void KNotify::notify(const QString &event, const QString &fromApp,
                     const QString &text, QString sound, QString file,
                     int present, int level)
{
//    kdDebug() << "event=" << event << " fromApp=" << fromApp << " text=" << text << " sound=" << sound <<
//        " file=" << file << " present=" << present << " level=" << level << endl;

    // check for valid events
    if ( event.length()>0 )     {

        // get config file
        KConfig *eventsFile;
        if ( isGlobal(event) )
            eventsFile = new KConfig( locate("config", "eventsrc"), true, false );
        else
            eventsFile = new KConfig( locate("data", fromApp+"/eventsrc"), true, false );

        eventsFile->setGroup( event );

        // get event presentation
        if ( present==-1 )
            present = eventsFile->readNumEntry( "presentation", -1 );
        if ( present==-1 )
            present = eventsFile->readNumEntry( "default_presentation", 0 );

        // get sound file name
        sound = eventsFile->readEntry( "soundfile" );
        if ( sound.length()==0 )
            sound = eventsFile->readEntry( "default_sound" );

        // get log file name
        file = eventsFile->readEntry( "logfile" );
        if ( file.length()==0 )
            file = eventsFile->readEntry( "default_logfile" );

        // get default event level
        level = eventsFile->readNumEntry( "level", 0 );

        delete eventsFile;
    }

    // emit event
    if ( present & KNotifyClient::Sound ) // && QFile(sound).isReadable()
        notifyBySound( sound );

    if ( present & KNotifyClient::Messagebox )
        notifyByMessagebox( text, level );

    if ( present & KNotifyClient::Logfile ) // && QFile(file).isWritable()
        notifyByLogfile( text, file );

    if ( present & KNotifyClient::Stderr )
        notifyByStderr( text );
}


bool KNotify::notifyBySound( const QString &sound )
{
    // Oh dear! we seem to have lost our connection to artsd!
    if( g_soundServer.isNull() || g_soundServer.error() )
        connectSoundServer();

    if ( !g_soundServer.isNull() && !g_soundServer.error() ) {

        // get file name
        QString soundFile(sound);
        if ( QFileInfo(sound).isRelative() )
            soundFile = locate( "sound", sound );

        // play sound finally
        kdDebug() << "KNotify::notifyBySound - trying to play file " << soundFile << endl;
        g_soundServer.play( QFile::encodeName(soundFile).data() );

        return true;
    } else {
        kdDebug() << "KNotify::notifyBySound - can't connect to aRts daemon" << endl;
        return false;
    }
}


bool KNotify::notifyByMessagebox(const QString &text, int level)
{
    // ignore empty messages
    if ( text.isEmpty() )
        return false;

    // display message box for specified event level
    switch( level ) {
    default:
    case KNotifyClient::Notification:
        KMessageBox::information( 0, text, i18n("Notification"), 0, false );
        break;
    case KNotifyClient::Warning:
        KMessageBox::sorry( 0, text, i18n("Warning"), false );
        break;
    case KNotifyClient::Error:
        KMessageBox::error( 0, text, i18n("Error"), false );
        break;
    case KNotifyClient::Catastrophe:
        KMessageBox::error( 0, text, i18n("Catastrophe!"), false );
        break;
    }

    return true;
}


bool KNotify::notifyByLogfile(const QString &text, const QString &file)
{
    // open file in append mode
    QFile logFile(file);
    if ( !logFile.open(IO_WriteOnly | IO_Append) ) return false;

    // append msg
    QTextStream strm( &logFile );
    strm << "- KNotify " << QDateTime::currentDateTime().toString() << ": ";
    strm << text << endl;

    // close file
    logFile.close();
    return true;
}

bool KNotify::notifyByStderr(const QString &text)
{
    // open stderr for output
    QTextStream strm( stderr, IO_WriteOnly );

    // output msg
    strm << "KNotify " << QDateTime::currentDateTime().toString() << ": ";
    strm << text << endl;

    return true;
}


bool KNotify::isGlobal(const QString &eventname)
{
    // it's global if there is a config group with the event's name
    KConfig config( locate("config", "eventsrc"), true, false );
    return config.hasGroup( eventname );
}
