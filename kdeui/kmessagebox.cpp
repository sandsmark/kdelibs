/*  This file is part of the KDE libraries
    Copyright (C) 1999 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 2 
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
/*
 * $Id$
 *
 * $Log$
 *
 */

#include "kmessagebox.h"

#include <qmessagebox.h>

#include "kapp.h"
#include "klocale.h"

 /** 
  * Easy MessageBox Dialog. 
  *
  * Provides convenience functions for some i18n'ed standard dialogs.
  *
  * @author Waldo Bastian (bastian@kde.org)
  * @version $Id$
  */

int   
KMessageBox::questionYesNo(QWidget *parent, const QString &text,
                           const QString &caption,
                           const QString &buttonYes, 
                           const QString &buttonNo)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("Question");
    _caption += " - "+kapp->getCaption();
    QString _buttonYes = buttonYes;
    if (_buttonYes.isEmpty())
        _buttonYes = i18n("&Yes");
    QString _buttonNo = buttonNo;
    if (_buttonNo.isEmpty())
        _buttonNo = i18n("&No");

    return QMessageBox::information(parent, _caption, text, 
	_buttonYes, _buttonNo, QString::null, 0, 1);
}

int 
KMessageBox::warningYesNo(QWidget *parent, const QString &text,
                          const QString &caption,
                          const QString &buttonYes,  
                          const QString &buttonNo)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("Warning");
    _caption += " - "+kapp->getCaption();
    QString _buttonYes = buttonYes;
    if (_buttonYes.isEmpty())
        _buttonYes = i18n("&Yes");
    QString _buttonNo = buttonNo;
    if (_buttonNo.isEmpty())
        _buttonNo = i18n("&No");

    return QMessageBox::warning(parent, _caption, text,
	_buttonYes, _buttonNo, QString::null, 1, 1);
}

int 
KMessageBox::warningYesNoCancel(QWidget *parent, const QString &text,
                                const QString &caption,
                                const QString &buttonYes,  
                                const QString &buttonNo)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("Warning");
    _caption += " - "+kapp->getCaption();
    QString _buttonYes = buttonYes;
    if (_buttonYes.isEmpty())
        _buttonYes = i18n("&Yes");
    QString _buttonNo = buttonNo;
    if (_buttonNo.isEmpty())
        _buttonNo = i18n("&No");

    return QMessageBox::warning(parent, _caption, text, 
	       _buttonYes, _buttonNo, i18n("&Cancel"), 0, 2);
}

void
KMessageBox::error(QWidget *parent,  const QString &text,
                   const QString &caption)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("Error");
    _caption += " - "+kapp->getCaption();

    (void) QMessageBox::critical(parent, _caption, text,
	       i18n("&Ok"), QString::null, QString::null, 0, 0);
    return;
}

void
KMessageBox::sorry(QWidget *parent, const QString &text,
                   const QString &caption)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("Sorry");
    _caption += " - "+kapp->getCaption();

    (void) QMessageBox::warning(parent, _caption, text, 
	       i18n("&Ok"), QString::null, QString::null, 0, 0);
    return;
}

void
KMessageBox::information(QWidget *parent,const QString &text,
                   const QString &caption)
{
    QString _caption = caption;
    if (_caption.isEmpty())
        _caption = i18n("Information");
    _caption += " - "+kapp->getCaption();

    (void) QMessageBox::information(parent, _caption, text,  
	       i18n("&Ok"), QString::null, QString::null, 0, 0);
    return;
}

