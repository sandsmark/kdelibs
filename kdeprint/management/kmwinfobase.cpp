#include "kmwinfobase.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>

KMWInfoBase::KMWInfoBase(int n, QWidget *parent, const char *name)
: KMWizardPage(parent,name)
{
	m_labels.setAutoDelete(false);
	m_edits.setAutoDelete(false);

	m_nlines = n;
	QGridLayout	*lay1 = new QGridLayout(this, m_nlines+3, 2, 0, 10);
	lay1->addRowSpacing(1,10);
	lay1->setRowStretch(m_nlines+2,1);
	lay1->setColStretch(1,1);

	m_info = new QLabel(this);
	m_info->setTextFormat(Qt::RichText);
	lay1->addMultiCellWidget(m_info,0,0,0,1);

	for (int i=0;i<m_nlines;i++)
	{
		m_labels.append(new QLabel(this));
		m_edits.append(new QLineEdit(this));
		lay1->addWidget(m_labels.last(),i+2,0);
		lay1->addWidget(m_edits.last(),i+2,1);
	}
}

void KMWInfoBase::setInfo(const QString& s)
{
	m_info->setText(s);
}

void KMWInfoBase::setLabel(int i, const QString& s)
{
	if (i >= 0 && i < m_nlines)
		m_labels.at(i)->setText(s);
}

void KMWInfoBase::setText(int i, const QString& s)
{
	if (i >= 0 && i < m_nlines)
		m_edits.at(i)->setText(s);
}

QString KMWInfoBase::text(int i)
{
	if (i >= 0 && i < m_nlines)
		return m_edits.at(i)->text();
	return QString::null;
}

void KMWInfoBase::setCurrent(int i)
{
	if (i >= 0 && i < m_nlines)
	{
		m_edits.at(i)->selectAll();
		m_edits.at(i)->setFocus();
	}
}