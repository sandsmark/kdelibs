// -*- c++ -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
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

#ifndef KIO_SLAVE_CONFIG_H
#define KIO_SLAVE_CONFIG_H

#include <qobject.h>
#include <kio/global.h>

namespace KIO {

    class SlaveConfigPrivate;
    /**
     * SlaveConfig
     *
     * This class does stuff.
     */
    class SlaveConfig : public QObject
    {
	Q_OBJECT
    public:
        static SlaveConfig *self();
        ~SlaveConfig();
        /**
         * Configure slaves of type @protocol by setting @p key to @p value.
         * If @p host is specified the configuration only applies when dealing
         * with @p host. @p host can be a domain as well.
         *
         * Examples of valid values for host are "www.kde.org" and "kde.org".
         *
         * Changes made to the slave configuration only apply to slaves
         * used by the current process.
         */
        void setConfigData(const QString &protocol, const QString &host, const QString &key, const QString &value );
        
        /**
         * Configure slaves of type @protocol with @p config.
         * If @p host is specified the configuration only applies when dealing
         * with @p host. @p host can be a domain as well.
         *
         * Examples of valid values for host are "www.kde.org" and "kde.org".
         *
         * Changes made to the slave configuration only apply to slaves
         * used by the current process.
         */
        void setConfigData(const QString &protocol, const QString &host, const MetaData &config );
                
        /**
         * Query slave configuration for slaves of type @p protocol when
         * dealing with @p host.
         *
         * Examples of valid values for host are "www.kde.org".
         */
        MetaData configData(const QString &protocol, const QString &host);

        /**
         * Query a specific configuration key for slaves of type @p protocol when
         * dealing with @p host.
         *
         * Examples of valid values for host are "www.kde.org".
         */
        QString configData(const QString &protocol, const QString &host, const QString &key);

        /**
         * Undo any changes made by calls to @ref setConfigData.
         */
        void reset();
    signals:
        /**
         * This signal is raised when a slave of type @p protocol deals
         * with @p host for the first time.
         *
         * Your application can use this signal to make some last minute
         * configuration changes with @ref setConfiguration based on the
         * host.
         */
        void configNeeded(const QString &protocol, const QString &host);
    protected:
        SlaveConfig();
        static SlaveConfig *_self;
        SlaveConfigPrivate *d;
    };
};

#endif
