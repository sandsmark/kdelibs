/*
   This file is part of the KDE libraries

   Copyright (c) 2001 George Staikos <staikos@kde.org>

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtimer.h>

#include <kardsvc.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kdebug.h>

#include <kpcsc.h>
#include <kcardreader.h>

#include <qapplication.h>       // for beep();


extern "C" {
   KDEDModule *create_kardsvc(const QCString &name) {
	   return new KardSvc(name);
   }

   const bool __kde_do_unload = false;
}


KardSvc::KardSvc(const QCString &name) : KDEDModule(name)
{
	_pcsc = new KPCSC;
	_timer = NULL;
	_readers = _pcsc->listReaders(NULL);
	reconfigure();
}
  

KardSvc::~KardSvc()
{
	delete _pcsc;
}


QStringList KardSvc::getSlotList() {
return _pcsc->listReaders(NULL);
}


bool KardSvc::isCardPresent(QString slot) {
KCardReader *_card = _pcsc->getReader(slot);

	if (!_card) {
		return false;
	}

	bool rc = _card->isCardPresent();
	delete _card;

return rc;
}


void KardSvc::poll() {
int err;
QStringList newReaders = _pcsc->listReaders(&err);

	// Update the list of readers
	if (_readers != newReaders) {
		if (err == 0) {
			kdDebug() << "kardsvc: reader list changed." << endl;
			for (QStringList::Iterator s = _readers.begin();
			     s != _readers.end();
			     ++s) {
				
				if (!newReaders.contains(*s) && 
						_states[*s] != 0) {
					_states[*s] = 0;
					kdDebug() << "kardsvc: card removed from slot " 
						  << *s << endl;
				}

				if (!newReaders.contains(*s))
					_states.remove(*s);
			}
			_readers = newReaders;
		} else return;
	}

	// Check each slot for a card insertion/removal
	for (QStringList::Iterator s = _readers.begin();
	     s != _readers.end();
	     ++s) {
		KCardReader *_card = _pcsc->getReader(*s);
		if (_card && _card->isCardPresent()) {
			if (!_states.contains(*s) || _states[*s] == 0) {
				_states[*s] = 1;
				kdDebug() << "kardsvc: card inserted in slot " 
					  << *s << endl;
				if (_beepOnEvent)
					QApplication::beep();
			}
		} else {
			if (_states.contains(*s) && _states[*s] == 1) {
				_states[*s] = 0;
				kdDebug() << "kardsvc: card removed from slot " 
					  << *s << endl;
				if (_beepOnEvent) {
					QApplication::beep();
					QApplication::beep();
				}
			}
		}
		
		if (_card)
			delete _card;
	}
}



QString KardSvc::getCardATR(QString slot) {
QString res;
KCardReader *_card = _pcsc->getReader(slot);

	if (!_card) {
		return QString::null;
	}

	if (!_card->isCardPresent()) {
		delete _card;
		return QString::null;
	}

	KCardATR kres = _card->getATR();
	if (kres.size() <= 0) {
		kdDebug() << "kardsvc: error getting ATR for " << slot << endl;
		delete _card;
		return QString::null;
	}

	for (unsigned int i = 0; i < kres.size(); i++) {
		if (i == 0) {
			res = "0x" + QString::number(kres[0], 16);
		} else {
			res += " 0x" + QString::number(kres[i], 16);
		}
	}
	
	delete _card;

return res;
}


void KardSvc::reconfigure() {
KConfig cfg("ksmartcardrc", false, false);

	_beepOnEvent = cfg.readBoolEntry("Beep on Insert", true);
	_enablePolling = cfg.readBoolEntry("Enable Polling", true);
	_launchManager = cfg.readBoolEntry("Launch Manager", true);

	if (_enablePolling && !_timer) {
		_timer = new QTimer(this);
		connect(_timer, SIGNAL(timeout()), this, SLOT(poll()));
		_timer->start(1500, false);
	} else if (!_enablePolling && _timer) {
		delete _timer;
		_timer = NULL;
	}
}


#include "kardsvc.moc"

