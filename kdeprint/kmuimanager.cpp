#include "kmuimanager.h"
#include "kprintdialog.h"
#include "kprintdialogpage.h"
#include "kpcopiespage.h"
#include "kprinter.h"
#include "kprinterpropertydialog.h"
#include "kmfactory.h"
#include "kmmanager.h"
#include "kmprinter.h"
#include "kpdriverpage.h"
#include "kpqtpage.h"

#include <klocale.h>

KMUiManager::KMUiManager(QObject *parent, const char *name)
: QObject(parent,name)
{
	m_printdialogflags = (KMUiManager::PrintDialogAll & ~KMUiManager::Options);
	m_printdialogpages.setAutoDelete(false);
	m_printdialogstd = KPrinter::CopiesPage;
	m_applicationtype = KPrinter::Dialog;
	m_pageselection = KPrinter::SystemSide;
	m_copyflags = 0;
}

KMUiManager::~KMUiManager()
{
}

void KMUiManager::addPropertyPages(KMPropertyPage*)
{
}

void KMUiManager::setupWizard(KMWizard*)
{
}

void KMUiManager::setApplicationType(KPrinter::ApplicationType t)
{
	m_applicationtype = t;
}

void KMUiManager::setupPrintDialog(KPrintDialog *dlg)
{
	// dialog flags
	int	f = m_printdialogflags;
	if (applicationType() == KPrinter::StandAlone)
		f &= ~(KMUiManager::Preview|KMUiManager::OutputToFile);
	dlg->setFlags(f);

	// dialog pages
	if (m_printdialogstd & KPrinter::CopiesPage)
	{
		KPCopiesPage	*cp = new KPCopiesPage(0,"CopiesPage");
		cp->setFlags(copyFlags());
		m_printdialogpages.insert(0,cp);
	}
	dlg->setDialogPages(&m_printdialogpages);
}

void KMUiManager::setupPropertyDialog(KPrinterPropertyDialog *dlg)
{
	if (dlg->printer())
	{
		if (dlg->printer()->name() == "__kdeprint_file")
		{ // special case of "print to file"
			dlg->addPage(new KPQtPage(dlg,"QtPage"));
			dlg->setCaption(i18n("Configuration of file printing"));
			// hide "save" button
			dlg->setDefaultButton(QString::null);
		}
		else
		{
			// check for a driver
			DrMain	*driver = KMFactory::self()->manager()->loadPrinterDriver(dlg->printer(), false);
			dlg->setDriver(driver);

			// add pages specific to print system
			setupPrinterPropertyDialog(dlg);

			// add driver page
			if (driver)
				dlg->addPage(new KPDriverPage(dlg->printer(),driver,dlg,"DriverPage"));

			dlg->setCaption(i18n("Configuration of %1").arg(dlg->printer()->name()));
		}
	}
}

void KMUiManager::setupPrinterPropertyDialog(KPrinterPropertyDialog *dlg)
{
	if (applicationType() == KPrinter::Dialog)
		dlg->addPage(new KPQtPage(dlg,"QtPage"));
}