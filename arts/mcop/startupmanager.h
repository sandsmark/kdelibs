    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#ifndef STARTUPMANAGER_H
#define STARTUPMANAGER_H

#include <list>

class StartupClass {
public:
	StartupClass();
	virtual void startup() = 0;
	virtual void shutdown();
};

class StartupManager {
	static std::list<StartupClass *> *startupClasses;
	static class ExtensionLoader *activeExtensionLoader;
public:
	static void add(StartupClass *sc);
	static void startup();
	static void shutdown();

	/**
	 * since extensions will register startup classes in the same global way
	 * all other classes do (with StartupManager::add(<some startup class>)),
	 * the StartupManager supports forwarding all add requests to the
	 * active extension, which will allow synchronizing the lt_dlopen
	 * lt_dlclose with the calls of startup()/shutdown() of all StartupClasses
	 * present in the extension
	 */
	static void setExtensionLoader(class ExtensionLoader *extension);
};
#endif /* STARTUPMANAGER_H */
