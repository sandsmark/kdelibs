#include "kmwfile.h"
#include "kmwizard.h"
#include "kmprinter.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qlayout.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>

KMWFile::KMWFile(QWidget *parent, const char *name)
: KMWizardPage(parent,name)
{
	m_ID = KMWizard::File;
	m_title = i18n("File selection");
	m_nextpage = KMWizard::Driver;

	m_edit = new QLineEdit(this);
	QToolButton	*browse = new QToolButton(this);
	browse->setIconSet(SmallIcon("folder"));
	QLabel	*l1 = new QLabel(this);
	l1->setText(i18n("<p>The printing will be redirected to a file. Enter here the path "
			 "of the file you want to use for redirection. Use an absolute path or "
			 "the browse button for graphical selection.</p>"));
	QLabel	*l2 = new QLabel(i18n("File:"), this);

	connect(browse,SIGNAL(clicked()),SLOT(slotBrowse()));

	QVBoxLayout	*lay1 = new QVBoxLayout(this, 0, 30);
	QHBoxLayout	*lay2 = new QHBoxLayout(0, 0, 10);
	lay1->addWidget(l1);
	lay1->addLayout(lay2);
	lay1->addStretch(1);
	lay2->addWidget(l2,0);
	lay2->addWidget(m_edit,1);
	lay2->addWidget(browse,0);
}

bool KMWFile::isValid(QString& msg)
{
	QFileInfo	fi(m_edit->text());
	if (fi.fileName().isEmpty())
	{
		msg = i18n("Empty file name !");
		return false;
	}

	if (!fi.dir().exists())
	{
		msg = i18n("Drirectory does not exist !");
		return false;
	}

	return true;
}

void KMWFile::updatePrinter(KMPrinter *p)
{
	QString	dev = QString::fromLatin1("file:%1").arg(m_edit->text());
	p->setDevice(KURL(dev));
}

void KMWFile::slotBrowse()
{
	QString	filename = KFileDialog::getSaveFileName(QString::null,QString::null,this);
	if (!filename.isEmpty())
		m_edit->setText(filename);
}
#include "kmwfile.moc"
