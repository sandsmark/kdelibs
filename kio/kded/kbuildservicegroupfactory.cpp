/*  This file is part of the KDE libraries
 *  Copyright (C) 2000 Waldo Bastian <bastian@kde.org>
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

#include "kbuildservicegroupfactory.h"
#include "ksycoca.h"
#include "ksycocadict.h"
#include "kresourcelist.h"

#include <kglobal.h>
#include <kstddirs.h>
#include <kmessageboxwrapper.h>
#include <kdebug.h>
#include <klocale.h>
#include <assert.h>

KBuildServiceGroupFactory::KBuildServiceGroupFactory() :
  KServiceGroupFactory()
{
   m_resourceList = new KSycocaResourceList();
   m_resourceList->add( "apps", "*.directory" );
}

KBuildServiceGroupFactory::~KBuildServiceGroupFactory() 
{
   delete m_resourceList;
}

KServiceGroup *
KBuildServiceGroupFactory::createEntry( const QString& file, const char *resource )
{
  return addNewEntry(file, resource, 0);
}

KServiceGroup *
KBuildServiceGroupFactory::addNewEntry( const QString& file, const char *resource, KSycocaEntry *newEntry)
{
  if (strcmp(resource, "apps") != 0) return 0;

  QString name = file;
  int pos = name.findRev('/');
  if (pos != -1) {
     name = name.left(pos+1);
  } else {
     name = "/";
  }

  KServiceGroup *entry = m_entryDict.find(name);

  if (!entry)
  {
     // Create new group entry
     QString fullPath = locate( resource, name + ".directory");

     entry = new KServiceGroup(fullPath, name);
     m_entryDict.insert(name, entry);
     addEntry( entry );

     if (name != "/")
     {
        // Make sure parent dir exists.
        KServiceGroup *parentEntry = 0;
        QString parent = name.left(name.length()-1);
        int i = parent.findRev('/');
        if (i > 0) {
           parent = parent.left(i+1);
        } else {
           parent = "/";
        }
        parentEntry = m_entryDict.find(parent);
        if (!parentEntry)
        {
           parentEntry = addNewEntry( parent, resource, 0 );
        }
        if (!entry->isDeleted())
           parentEntry->addEntry( entry );
     }
  }
  if (newEntry)
     entry->addEntry( newEntry );
  return 0;
}

void
KBuildServiceGroupFactory::addEntry( KSycocaEntry *newEntry )
{
   KSycocaFactory::addEntry(newEntry);
}

