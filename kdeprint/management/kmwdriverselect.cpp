#include "kmwdriverselect.h"
#include "kmwizard.h"
#include "kmprinter.h"
#include "kmdbentry.h"
#include "kmdriverdb.h"

#include <qlabel.h>
#include <qlayout.h>
#include <klistbox.h>
#include <klocale.h>

KMWDriverSelect::KMWDriverSelect(QWidget *parent, const char *name)
: KMWizardPage(parent,name)
{
	m_ID = KMWizard::DriverSelect;
	m_title = i18n("Driver selection");
	m_nextpage = KMWizard::DriverTest;
	m_entries = NULL;

	m_list = new KListBox(this);
	QLabel	*l1 = new QLabel(this);
	l1->setText(i18n("<p>Several drivers have been detected for this model. Select the driver "
			 "you want to use. You will have the opportunity to test it as well as to "
			 "change it if necessary.</p>"));

	QVBoxLayout	*main_ = new QVBoxLayout(this, 0, 10);
	main_->addWidget(l1,0);
	main_->addWidget(m_list,1);
}

bool KMWDriverSelect::isValid(QString& msg)
{
	if (m_list->currentItem() == -1)
	{
		msg = i18n("You must select a driver !");
		return false;
	}
	return true;
}

void KMWDriverSelect::initPrinter(KMPrinter *p)
{
	m_entries = KMDriverDB::self()->findEntry(p->manufacturer(),p->model());
	m_list->clear();
	if (m_entries)
	{
		KMDBEntryListIterator	it(*m_entries);
		for (;it.current();++it)
			m_list->insertItem(it.current()->description);
		if (m_entries->count() > 0)
			m_list->setSelected(0,true);
	}
}

void KMWDriverSelect::updatePrinter(KMPrinter *p)
{
	int	index = m_list->currentItem();
	if (m_entries && index >= 0 && index < (int)(m_entries->count()))
	{
		KMDBEntry	*entry = m_entries->at(index);
		p->setDbEntry(entry);
		p->setDriverInfo(entry->description);
	}
	else
	{
		p->setDbEntry(0);
		p->setDriverInfo(QString::null);
	}
}
