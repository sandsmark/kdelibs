/* This file is part of the KDE project
 *
 * Copyright (C) 2001 George Staikos <staikos@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include "kcertpart.h"
#include <kparts/genericfactory.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <qframe.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbutton.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <kmessagebox.h>
#include <kpassdlg.h>
#include <ksslall.h>
#include <kfiledialog.h>
#include <kprocess.h>
#include <qtabwidget.h>
#include <kseparator.h>
#include <klistview.h>
#include <kio/kmimemagic.h>
#include <qmultilineedit.h>
#include <kparts/browserextension.h>
#include <kparts/browserinterface.h>

K_EXPORT_COMPONENT_FACTORY( libkcertpart, KParts::GenericFactory<KCertPart> )


KX509Item::KX509Item(KListViewItem *parent, KSSLCertificate *x) :
			KListViewItem(parent, 0L)
{
	cert = x;
	if (x) {
		setText(0, x->getSubject());
	} else {
		setText(0, i18n("Invalid!"));
	}
}


KX509Item::~KX509Item() 
{
	if (cert)
		delete cert;
}


KPKCS12Item::KPKCS12Item(KListViewItem *parent, KSSLPKCS12 *x) :
			KListViewItem(parent, 0L)
{
	cert = x;
	if (x) {
		setText(0, x->getCertificate()->getSubject());
	} else {
		setText(0, i18n("Invalid!"));
	}
}


KPKCS12Item::~KPKCS12Item() 
{
	if (cert)
		delete cert;
}


class KCertPartPrivate {
	public:
		KParts::BrowserExtension *browserExtension;
};


KCertPart::KCertPart(QWidget *parentWidget, const char *widgetName,
                     QObject *parent, const char *name,
		     const QStringList & /*args*/ ) 
          : KParts::ReadWritePart(parent, name) {
 KInstance *instance = new KInstance("KCertPart");
 QGridLayout *grid;
 setInstance(instance);

 // This is a bit confusing now.  Here's now it works:
 //    We create an _frame and split it left/right
 //    Then we add the ListView to the left and create
 //    a new frame on the right.  We set the main widget
 //    to on the right.

 _p12 = NULL;
 _ca = NULL;
 d = new KCertPartPrivate;
 d->browserExtension = new KParts::BrowserExtension(this);

 _frame = new QFrame(parentWidget, widgetName);
 setWidget(_frame);

 _baseGrid = new QGridLayout(_frame, 15, 9, KDialog::marginHint(),
		 			   KDialog::spacingHint());

 _sideList = new KListView(_frame);
 _sideList->setRootIsDecorated(true);
 _sideList->addColumn(i18n("Certificates"));
 _parentCA = new KListViewItem(_sideList, i18n("Signers"));
 _parentCA->setExpandable(true);
 _sideList->setOpen(_parentCA, true);
 _parentP12 = new KListViewItem(_sideList, i18n("Client"));
 _parentP12->setExpandable(true);
 _sideList->setOpen(_parentP12, true);

 _baseGrid->addMultiCellWidget(_sideList, 0, 14, 0, 1);



//------------------------------------------------------------------------
//   The PKCS widget
//------------------------------------------------------------------------
 _pkcsFrame = new QFrame(_frame);

 grid = new QGridLayout(_pkcsFrame, 13, 6, KDialog::marginHint(),
                                       KDialog::spacingHint() );
 grid->addMultiCellWidget(new QLabel(i18n("KDE Secure Certificate Import"), _pkcsFrame), 0, 0, 0, 5);
 grid->addWidget(new QLabel(i18n("Chain:"), _pkcsFrame), 1, 0);
 _p12_chain = new QComboBox(_pkcsFrame);
 grid->addMultiCellWidget(_p12_chain, 1, 1, 1, 4);
 connect(_p12_chain, SIGNAL(activated(int)), SLOT(slotChain(int)));

 grid->addWidget(new QLabel(i18n("Subject:"), _pkcsFrame), 2, 0);
 grid->addWidget(new QLabel(i18n("Issued by:"), _pkcsFrame), 2, 3);
 _p12_subject = KSSLInfoDlg::certInfoWidget(_pkcsFrame, QString(""));
 _p12_issuer = KSSLInfoDlg::certInfoWidget(_pkcsFrame, QString(""));
 grid->addMultiCellWidget(_p12_subject, 3, 6, 0, 2);
 grid->addMultiCellWidget(_p12_issuer, 3, 6, 3, 5);

 grid->addWidget(new QLabel(i18n("File:"), _pkcsFrame), 7, 0);
 _p12_filenameLabel = new QLabel("", _pkcsFrame);
 grid->addWidget(_p12_filenameLabel, 7, 1);
 grid->addWidget(new QLabel(i18n("File Format:"), _pkcsFrame), 7, 3);
 grid->addWidget(new QLabel("PKCS#12", _pkcsFrame), 7, 4);


 //
 //  Make the first tab
 //
 _tabs = new QTabWidget(_pkcsFrame);
 grid->addMultiCellWidget(_tabs, 8, 12, 0, 5);

 QFrame *tab = new QFrame(_pkcsFrame);
 QGridLayout *tabGrid = new QGridLayout(tab, 4, 5, KDialog::marginHint(),
                                                   KDialog::spacingHint() );
 tabGrid->addWidget(new QLabel(i18n("State:"), tab), 0, 0);
 _p12_certState = new QLabel("", tab);
 tabGrid->addMultiCellWidget(_p12_certState, 0, 0, 1, 4);

 tabGrid->addWidget(new QLabel(i18n("Valid From:"), tab), 1, 0);
 _p12_validFrom = new QLabel("", tab);
 tabGrid->addMultiCellWidget(_p12_validFrom, 1, 1, 1, 4);

 tabGrid->addWidget(new QLabel(i18n("Valid Until:"), tab), 2, 0);
 _p12_validUntil = new QLabel("", tab);
 tabGrid->addMultiCellWidget(_p12_validUntil, 2, 2, 1, 4);

 tabGrid->addWidget(new QLabel(i18n("Serial Number:"), tab), 3, 0);
 _p12_serialNum = new QLabel("", tab);
 tabGrid->addWidget(_p12_serialNum, 3, 1);
 _tabs->addTab(tab, i18n("State"));


 //
 // Make the second tab
 //
 tab = new QFrame(_pkcsFrame);
 tabGrid = new QGridLayout(tab, 4, 5, KDialog::marginHint(),
                                      KDialog::spacingHint() );
 tabGrid->addWidget(new QLabel(i18n("MD5 Digest:"), tab), 0, 0);
 _p12_digest = new QLabel(tab);
 tabGrid->addMultiCellWidget(_p12_digest, 0, 0, 1, 4);
 tabGrid->addWidget(new QLabel(i18n("Signature:"), tab), 1, 0);
 _p12_sig = new QMultiLineEdit(tab);
 tabGrid->addMultiCellWidget(_p12_sig, 1, 3, 1, 4);
 _p12_sig->setReadOnly(true);

 _tabs->addTab(tab, i18n("Signature"));


 //
 // Make the third tab
 //
 tab = new QFrame(_pkcsFrame);
 tabGrid = new QGridLayout(tab, 4, 5, KDialog::marginHint(),
                                      KDialog::spacingHint() );
 tabGrid->addWidget(new QLabel(i18n("Public Key:"), tab), 0, 0);
 _p12_pubkey = new QMultiLineEdit(tab);
 tabGrid->addMultiCellWidget(_p12_pubkey, 0, 3, 1, 4);
 _p12_pubkey->setReadOnly(true);
 

 _tabs->addTab(tab, i18n("Public Key"));

 _pkcsFrame->hide();

//------------------------------------------------------------------------
//   The X509 widget
//------------------------------------------------------------------------



//------------------------------------------------------------------------
//   The blank widget
//------------------------------------------------------------------------
 _blankFrame = new QFrame(_frame);
 grid = new QGridLayout(_blankFrame, 1, 1, KDialog::marginHint(),
                                           KDialog::spacingHint() );
 grid->addMultiCellWidget(new QLabel(i18n("KDE Secure Certificate Import"), _blankFrame), 0, 0, 0, 0);
 _blankFrame->show();



 //
 // Finish it off
 //
 _baseGrid->addMultiCellWidget(new KSeparator(KSeparator::HLine, _frame), 13, 13, 2, 8);
 _launch = new QPushButton(i18n("&Crypto Manager..."), _frame);
 _import = new QPushButton(i18n("&Import"), _frame);
 _save = new QPushButton(i18n("&Save..."), _frame);
 _done = new QPushButton(i18n("&Done"), _frame);
 _baseGrid->addMultiCellWidget(_launch, 14, 14, 4, 5);
 _baseGrid->addWidget(_import, 14, 6);
 _baseGrid->addWidget(_save, 14, 7);
 _baseGrid->addWidget(_done, 14, 8);
 connect(_launch, SIGNAL(clicked()), SLOT(slotLaunch()));
 connect(_import, SIGNAL(clicked()), SLOT(slotImport()));
 connect(_save, SIGNAL(clicked()), SLOT(slotSave()));
 connect(_done, SIGNAL(clicked()), SLOT(slotDone()));
 _import->setEnabled(false);
 _save->setEnabled(false);



 _baseGrid->addMultiCellWidget(_pkcsFrame, 0, 12, 2, 8);
 _baseGrid->addMultiCellWidget(_blankFrame, 0, 12, 2, 8);

 connect(_sideList, SIGNAL(selectionChanged(QListViewItem*)),
	 this, SLOT(slotSelectionChanged(QListViewItem*)));
 setReadWrite(true);
}


KCertPart::~KCertPart() {
 delete d->browserExtension;
 delete d;
}


void KCertPart::setReadWrite(bool rw) {
  if (rw == false) {
  	_import->setEnabled(false);
  	_save->setEnabled(false);
  }
  ReadWritePart::setReadWrite(rw);
}


bool KCertPart::saveFile() {
if (_p12) {
  QString certFile = KFileDialog::getSaveFileName(QString::null, "application/x-pkcs12");
  if (certFile.isEmpty())
    return false;
 
  if (!_p12->toFile(certFile)) {
    KMessageBox::sorry(_frame, i18n("Save failed."), i18n("Certificate Import"));
    return false;
  }

	return true;
} else if (_ca) {
	return true;
} else {
	return false;
}
}


bool KCertPart::openFile() {
#ifndef HAVE_SSL
  KMessageBox::sorry(_frame, i18n("You don't seem to have compiled KDE with SSL support."), i18n("Certificate Import"));
 return false;
#else
QCString pass;

  emit completed();

  QString whatType = KMimeType::findByURL(m_url)->name();

if (whatType == "application/x-pkcs12") {
  _p12 = KSSLPKCS12::loadCertFile(m_file);

  while (!_p12) {
     // try prompting for a password.
    int rc = KPasswordDialog::getPassword(pass, i18n("Certificate password"));
    if (rc != KPasswordDialog::Accepted) break;

    _p12 = KSSLPKCS12::loadCertFile(m_file, QString(pass));

    if (!_p12) {
      rc = KMessageBox::warningYesNo(_frame, i18n("The certificate file could not be loaded.  Try a different password?"), i18n("Certificate Import"));
      if (rc == KMessageBox::Yes) continue;
      break;
    }
  }

  if (!_p12) return false;

  new KPKCS12Item(_parentP12, _p12);
  _p12 = NULL;
 return true;
} else if (whatType == "application/x-x509-ca-cert") {
 QFile qf(m_file);
 FILE *fp; 
 if (!qf.open(IO_ReadOnly)) {
    KMessageBox::sorry(_frame, i18n("This file cannot be opened."), i18n("Certificate Import"));
    return false;
 }

 fp = fdopen(qf.handle(), "r");
 if (!fp) {
    KMessageBox::sorry(_frame, i18n("This file cannot be opened."), i18n("Certificate Import"));
    return false;
 }

// STACK_OF(X509_INFO) *sx5i = KOSSL::self()->PEM_X509_INFO_read(fp, NULL, , NULL);
 return true;
} else {
  KMessageBox::sorry(_frame, i18n("I don't know how to handle this type of file."), i18n("Certificate Import"));
 return false;
}
#endif
}


void KCertPart::displayPKCS12() {
  KSSLCertificate *xc = _p12->getCertificate();
  _p12_filenameLabel->setText(m_file);
  displayCert(xc);
  _p12_certState->setText(KSSLCertificate::verifyText(_p12->validate()));

  // Set the chain if it's there
  if (xc->chain().depth() > 1) {
    QPtrList<KSSLCertificate> cl = xc->chain().getChain();
    int cnt = 0;
    _p12_chain->setEnabled(true);
    _p12_chain->clear();
    _p12_chain->insertItem(i18n("0 - Site Certificate"));
    for (KSSLCertificate *c = cl.first(); c != 0; c = cl.next()) {
       KSSLX509Map map(c->getSubject());
       _p12_chain->insertItem(QString::number(++cnt)+" - "+map.getValue("CN"));
    }
    _p12_chain->setCurrentItem(0);
  } else {
    _p12_chain->clear();
    _p12_chain->setEnabled(false);
  }
}


void KCertPart::displayCert(KSSLCertificate *c) {
  // We have the file, lets work with it.
  _p12_subject->setValues(c->getSubject());
  _p12_issuer->setValues(c->getIssuer());

  // Set the valid period
         QPalette cspl = _p12_validFrom->palette();
         if (QDateTime::currentDateTime() < c->getQDTNotBefore()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         _p12_validFrom->setPalette(cspl);
         _p12_validFrom->setText(c->getNotBefore());

         cspl = _p12_validUntil->palette();
         if (QDateTime::currentDateTime() > c->getQDTNotAfter()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         _p12_validUntil->setPalette(cspl);
         _p12_validUntil->setText(c->getNotAfter());

  _p12_serialNum->setText(c->getSerialNumber());
         cspl = _p12_certState->palette();
         if (!c->isValid()) {
            cspl.setColor(QColorGroup::Foreground, QColor(196,33,21));
         } else {
            cspl.setColor(QColorGroup::Foreground, QColor(42,153,59));
         }
         _p12_certState->setPalette(cspl);
  _p12_certState->setText(KSSLCertificate::verifyText(c->validate()));

  _p12_pubkey->setText(c->getPublicKeyText());
  _p12_digest->setText(c->getMD5DigestText());
  _p12_sig->setText(c->getSignatureText());
}



void KCertPart::slotChain(int c) {

  if (c == 0) {
     displayCert(_p12->getCertificate());
     _p12_certState->setText(KSSLCertificate::verifyText(_p12->validate()));
  } else {
     displayCert(_p12->getCertificate()->chain().getChain().at(c-1));
  }
}


void KCertPart::slotImport() {
if (_p12) {
  KSimpleConfig cfg("ksslcertificates", false);

  if (cfg.hasGroup(_p12->getCertificate()->getSubject())) {
     int rc = KMessageBox::warningYesNo(_frame, i18n("A certificate with that name already exists.  Are you sure that you wish to replace it?"), i18n("Certificate Import"));
     if (rc == KMessageBox::No) {
        return;
     }
  }

  cfg.setGroup(_p12->getCertificate()->getSubject());
  cfg.writeEntry("PKCS12Base64", _p12->toString());
  cfg.writeEntry("Password", "");
  cfg.sync();
  KMessageBox::information(_frame, i18n("Certificate has been successfully imported into KDE.\nYou can manage your certificate settings from the KDE Control Center."), i18n("Certificate Import"));
} else if (_ca) {
}
}


void KCertPart::slotSave() {
  saveFile();
}


void KCertPart::slotDone() {
  KParts::BrowserInterface *iface = d->browserExtension->browserInterface();
  iface->callMethod("goHistory(int)", -1);
}


void KCertPart::slotLaunch() {
KShellProcess p;
  p << "kcmshell" << "crypto";
  p.start(KProcess::DontCare);
}


void KCertPart::slotSelectionChanged(QListViewItem *x) {
  _p12 = NULL;
  _ca = NULL;
  _blankFrame->hide();
  _pkcsFrame->hide();
  if (x && x->parent() == _parentCA) {
	_x509Frame->show();
	_ca = dynamic_cast<KX509Item*>(x)->cert;
	_import->setEnabled(isReadWrite());
	_save->setEnabled(isReadWrite());
  } else if (x && x->parent() == _parentP12) {
	_pkcsFrame->show();
	_p12 = dynamic_cast<KPKCS12Item*>(x)->cert;
	_import->setEnabled(isReadWrite());
	_save->setEnabled(isReadWrite());
	displayPKCS12();
  } else {
	_blankFrame->show();
        _import->setEnabled(false);
        _save->setEnabled(false);
  }
}


KAboutData *KCertPart::createAboutData()
{
    return new KAboutData("KCertPart", I18N_NOOP("KDE Certificate Part"), "1.0");
}

#include "kcertpart.moc"

