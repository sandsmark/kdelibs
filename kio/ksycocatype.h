/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                            Waldo Bastian <bastian@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __ksycocatype_h__
#define __ksycocatype_h__

/**
 * A KSycocaType is a code (out of the KSycocaType enum) assigned to each
 * class type derived from KSycocaEntry .
 * To use it, call the macro K_SYCOCATYPE( your_typecode, parent_class ) 
 * at the top of your class definition.
 */
enum KSycocaType { KST_KSycocaEntry = 0, KST_KService = 1, KST_KServiceType = 2, KST_KMimeType = 3,
		 KST_KFolderType = 4, KST_KDEDesktopMimeType = 5, KST_KExecMimeType = 6,
		 KST_KServiceGroup = 7,
		 KST_KCustom = 1000 };

#define K_SYCOCATYPE( type, baseclass ) \
public: \
 virtual bool isType(KSycocaType t) const { if (t == type) return true; return baseclass::isType(t);} \
 virtual KSycocaType sycocaType() const { return type; } \
private:

/**
 * A KSycocaFactoryId is a code (out of the KSycocaFactoryId enum) 
 * assigned to each class type derived from KSycocaFactory.
 * To use it, call the macro K_SYCOCAFACTORY( your_factory_id ) 
 * at the top of your class definition.
 */
enum KSycocaFactoryId { KST_KServiceFactory = 1, 
                        KST_KServiceTypeFactory = 2,
                        KST_KServiceGroupFactory = 3 };

#define K_SYCOCAFACTORY( factory_id ) \
public: \
 virtual KSycocaFactoryId factoryId() const { return factory_id; } \
private:



#endif
