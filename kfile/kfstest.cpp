/* This file is part of the KDE libraries
    Copyright (C) 1997, 1998 Richard Moore <rich@kde.org>
                  1998 Stephan Kulow <coolo@kde.org>
		
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

#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qwidget.h>

#include <kfiledialog.h>
#include <kfileiconview.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapp.h>
#include <kurl.h>
#include <kdiroperator.h>
#include <kfilewidget.h>
#include <kfile.h>
#include <kdebug.h>

int main(int argc, char **argv)
{
    KApplication a(argc, argv, "kfstest");
    QString name1;

    QString argv1;
    if (argc > 1)
	argv1 = QString::fromLatin1(argv[1]);

    if (argv1 == QString::fromLatin1("diroperator")) {
	KDirOperator *op = new KDirOperator(QString::null, 0, "operator");
	op->setView(KFile::Simple);
	op->show();
	a.setMainWidget(op);
	a.exec();
    } else if (argv1 == QString::fromLatin1("preselect")) {
        name1 = KFileDialog::getOpenURL(QString::fromLatin1("/etc/passwd")).url();
    } else if (argv1 == QString::fromLatin1("widget")) {
	KFileWidget *widget = new KFileWidget(KFileWidget::Simple);
	// widget->setURL(QString::fromLatin1("ftp://localhost"));
	// widget->setURL(QString::fromLatin1("smb://without.penguinpowered.com/)");
	// widget->setURL(QString::fromLatin1("smb://redwood203.marin.k12.ca.us"),false);
	widget->show();
	a.setMainWidget(widget);
	a.exec();
    } else if (argv1 == QString::fromLatin1("dirs"))
	name1 = KFileDialog::getExistingDirectory();
    else if (argv1 == QString::fromLatin1("heap")) {
	KFileDialog *dlg = new KFileDialog( QString::null, QString::null, 0L,
					    "file dialog", true );
	if ( dlg->exec() == KDialog::Accepted )
	    name1 = dlg->selectedURL().url();
    }

    else{
	KFileDialog dlg(QString::null,
			QString::fromLatin1("*|All files\n"
					    "*.lo *.o *.la|All libtool files"),
			0, 0, true);
	dlg.setMode( KFile::File );
	if ( dlg.exec() == QDialog::Accepted ) {
	    KURL::List list = dlg.selectedURLs();
	    KURL::List::ConstIterator it = list.begin();
	    while ( it != list.end() ) {
		name1 = (*it).url();
		kdDebug() << "Selected URL: " << name1 << endl;
		++it;
	    }
	}
    }

    if (!(name1.isNull()))
	KMessageBox::information(0, QString::fromLatin1("You selected the file " ) + name1,
				 QString::fromLatin1("Your Choice"));
    return 0;
}
