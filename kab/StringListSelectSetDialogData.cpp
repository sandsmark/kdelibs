/**********************************************************************

	--- Qt Architect generated file ---

	File: StringListSelectSetDialogData.cpp
	Last generated: Thu Aug 6 13:19:37 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include "StringListSelectSetDialogData.h"

#define Inherited QDialog


StringListSelectSetDialogData::StringListSelectSetDialogData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 0 )
{
	frameBase = new QFrame( this, "Frame_1" );
	frameBase->setGeometry( 10, 10, 380, 280 );
	frameBase->setMinimumSize( 10, 10 );
	frameBase->setMaximumSize( 32767, 32767 );
	frameBase->setFrameStyle( 34 );

	lbStrings = new QListBox( this, "ListBox_1" );
	lbStrings->setGeometry( 20, 20, 360, 220 );
	lbStrings->setMinimumSize( 10, 10 );
	lbStrings->setMaximumSize( 32767, 32767 );
	lbStrings->setFrameStyle( 51 );
	lbStrings->setLineWidth( 2 );
	lbStrings->setMultiSelection( TRUE );

	buttonOK = new QPushButton( this, "PushButton_1" );
	buttonOK->setGeometry( 20, 250, 100, 30 );
	buttonOK->setMinimumSize( 10, 10 );
	buttonOK->setMaximumSize( 32767, 32767 );
	connect( buttonOK, SIGNAL(pressed()), SLOT(accept()) );
	buttonOK->setText( "" );
	buttonOK->setAutoRepeat( FALSE );
	buttonOK->setAutoResize( FALSE );

	buttonCancel = new QPushButton( this, "PushButton_2" );
	buttonCancel->setGeometry( 280, 250, 100, 30 );
	buttonCancel->setMinimumSize( 10, 10 );
	buttonCancel->setMaximumSize( 32767, 32767 );
	connect( buttonCancel, SIGNAL(pressed()), SLOT(reject()) );
	buttonCancel->setText( "" );
	buttonCancel->setAutoRepeat( FALSE );
	buttonCancel->setAutoResize( FALSE );

	resize( 400,300 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


StringListSelectSetDialogData::~StringListSelectSetDialogData()
{
}
