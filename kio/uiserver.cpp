/* This file is part of the KDE libraries
   Copyright (C) 2000 Matej Koss <koss@miesto.sk>
                      David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
// -*- mode: c++; c-basic-offset: 4 -*-

#include <kstddirs.h>
#include <kuniqueapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "kio/uiserver.h"

int main(int argc, char **argv)
{
    KAboutData aboutdata("kio_uiserver", I18N_NOOP("UIServer"),
                         "0.8", I18N_NOOP("KDE Progress Information UI Server"),
                         KAboutData::License_GPL, "(C) 2000, David Faure & Matt Koss");
    // Who's the maintainer ? :)
    aboutdata.addAuthor("David Faure",I18N_NOOP("Developer"),"faure@kde.org");
    aboutdata.addAuthor("Matej Koss",I18N_NOOP("Developer"),"koss@miesto.sk");

    KCmdLineArgs::init( argc, argv, &aboutdata );
    // KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();
    KUniqueApplication app;

    // setup dcop communication
    if (!app.dcopClient()->isAttached())
        app.dcopClient()->registerAs("kio_uiserver",false);
    if (!app.dcopClient()->isAttached())
        return 1;

    (void) new UIServer;

    return app.exec();
}

UIServer::UIServer() : DCOPObject("UIServer")
{
}

//static
int UIServer::s_jobId = 0;

int UIServer::newJob()
{
    return ++s_jobId;
}

void UIServer::totalEntries( int id, unsigned long total )
{
    kdDebug() << "UIServer::totalEntries " << id << " " << total << endl;
}

void UIServer::entries( int id, unsigned long count )
{
    kdDebug() << "UIServer::entries " << id << " " << count << endl;
}
