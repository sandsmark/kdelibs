/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
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

#include "kservicefactory.h"
#include "ksycoca.h"
#include "ksycocatype.h"
#include "ksycocadict.h"
#include "kservice.h"

#include <qstring.h>

#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstddirs.h>

KServiceFactory::KServiceFactory()
 : KSycocaFactory( KST_KServiceFactory )
{
   m_offerListOffset = 0;
   if (m_str)
   {
      // Read Header
      Q_INT32 i;
      (*m_str) >> i;
      m_offerListOffset = i; 
   }
   _self = this;
}

KServiceFactory::~KServiceFactory()
{
   _self = 0L;
}

KSycocaEntry * KServiceFactory::createEntry( const QString& file )
{
  // Just a backup file ?
  if ( file.right(1) == "~" || file.right(4) == ".bak" || ( file[0] == '%' && file.right(1) == "%" ) )
      return 0;

  KService * serv = new KService( file );

  if ( serv->isValid() )
     return serv;
  else {
     kDebugWarning( 7012, "Invalid Service : %s", file.ascii() );
     delete serv;
     return 0L;
  }
}

KServiceFactory * KServiceFactory::self()
{
  if (!_self)
    _self = new KServiceFactory();
  return _self;
}

KService * KServiceFactory::findServiceByName(const QString &_name)
{
   if (!m_sycocaDict) return 0; // Error!

   // Warning : this assumes we're NOT building a database
   // But since findServiceByName isn't called in that case...
   // [ see KServiceTypeFactory for how to do it if needed ]

   int offset = m_sycocaDict->find_string( _name );
   if (!offset) return 0; // Not found

   KService * newService = createService(offset);

   // Check whether the dictionary was right.
   if (newService && (newService->name() != _name))
   {
      // No it wasn't...
      delete newService;
      newService = 0; // Not found
   }
   return newService;
}

KService* KServiceFactory::createService(int offset)
{
   KService * newEntry = 0L;
   KSycocaType type; 
   QDataStream *str = KSycoca::self()->findEntry(offset, type);
   switch(type)
   {
     case KST_KService:
        newEntry = new KService(*str, offset);
        break;
        
     default:
        kDebugError( 7011, QString("KServiceFactory: unexpected object entry in KSycoca database (type = %1)").arg((int)type) );
        break;
   }
   if (!newEntry->isValid())
   {
      kDebugError( 7011, "KServiceFactory: corrupt object in KSycoca database!\n");
      delete newEntry;
      newEntry = 0;
   }   
   return newEntry;
}

KService::List KServiceFactory::allServices()
{
   kDebugInfo( 7011, "KServiceFactory::allServices()");
   KService::List list;
   if (!m_str) return list;

   // Assume we're NOT building a database

   int offset = m_beginEntryOffset;
   KService *newService;
   while ( offset < m_endEntryOffset )
   {
      newService = createService(offset);
      if (newService)
         list.append( KService::Ptr( newService ) );
      offset = m_str->device()->at();
   }
   return list;
}

KService::List KServiceFactory::offers( int serviceTypeOffset )
{
   //kDebugInfo( 7011, QString("KServiceFactory::offers ( %1 )")
   //                          .arg(serviceTypeOffset,8,16));
   KService::List list;

   QDataStream *str = m_str;
   // Jump to the offer list
   str->device()->at( m_offerListOffset );
   
   Q_INT32 aServiceTypeOffset;
   Q_INT32 aServiceOffset;
   // We might want to do a binary search instead of a linear search
   // since servicetype offsets are sorted. Bah.
   while (true)
   {
      (*str) >> aServiceTypeOffset;
      if ( aServiceTypeOffset )
      {
         (*str) >> aServiceOffset;
         if ( aServiceTypeOffset == serviceTypeOffset )
         {
            //kDebugInfo( 7011, QString("KServiceFactory::offers : Found !"));
            // Save stream position !
            int savedPos = str->device()->at();
            // Create Service
            KService * serv = createService( aServiceOffset );
            list.append( KService::Ptr( serv ) );
            // Restore position
            str->device()->at( savedPos );
         } else if ( aServiceTypeOffset > (Q_INT32)serviceTypeOffset )
            break; // too far
      }
      else
         break; // 0 => end of list
   }
   return list;
}

KServiceFactory *KServiceFactory::_self = 0;
