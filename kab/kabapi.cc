/* -*- C++ -*-
 * This file implements the application programming interface
 * for using kab's addressbook files within other programs.
 * Parse it with kdoc to get the API documentation.
 * 
 * the KDE addressbook
 * copyright:  (C) Mirko Sucker, 1998, 1999
 * license:    GNU Public License, Version 2
 * mail to:    Mirko Sucker <mirko@kde.org>
 * requires:   recent C++-compiler, at least Qt 2.0
 * $Id$
 */

#include "kabapi.h"
#include <qlistbox.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <knana.h>


#include "kabapi.moc"

#ifdef KAB_KDEBUG_AREA
#undef KAB_KDEBUG_AREA
#endif 

#define KAB_KDEBUG_AREA 800

using namespace std;

KabAPI::KabAPI(QWidget* parent, const char* name)
  : KDialogBase(parent, name),
    book(0),
    listbox(new QListBox(this)),
    selection(-1)
{
  CHECK_PTR(listbox);
  setMainWidget(listbox);
  showButtonApply(false);
  enableButtonSeparator(true);
  connect(listbox, SIGNAL(highlighted(int)), SLOT(entrySelected(int)));
}

int KabAPI::exec()
{
  QStringList names;
  // -----
  if(book==0)
    {
      debug("KabAPI::exec: you have to call init before using the API.");
      return -1;
    } else {
      if(book->getListOfNames(&names, true, false)==AddressBook::NoError)
	{
	  listbox->clear();
	  listbox->insertStringList(names);
	  if(names.count()>0)
	    {
	      listbox->setCurrentItem(0);
	    }
	  listbox->setMinimumSize(listbox->sizeHint());
	  adjustSize();
	  resize(minimumSize());
	  return KDialogBase::exec();
	} else {
	  debug("KabAPI::exec: error creating interface.");
	  return -1;
	}
    }
}  
    
AddressBook::ErrorCode KabAPI::init()
{
  // ############################################################################  
  book=new AddressBook(this, "KABAPI::book", true);
  if(book->getState()==AddressBook::NoError)
    {
      connect(book, SIGNAL(setStatus(const QString&)),
	      SLOT(setStatusSlot(const QString&)));
      return AddressBook::NoError;
    } else {
      return AddressBook::InternError;
    }
  // ############################################################################  
}

AddressBook::ErrorCode KabAPI::getEntry(AddressBook::Entry& entry, KabKey& key)
{
  REQUIRE(book!=0);
  REQUIRE(selection!=-1); // this is true if the dialog has been accepted
  // ############################################################################
  if(book->noOfEntries()==0)
    {
      return AddressBook::NoEntry;
    }
  if(selection>=0)
    {
      if(book->getKey(selection, key)==AddressBook::NoError)
	{
	  if(book->getEntry(key, entry)==AddressBook::NoError)
	    {
	      return AddressBook::NoError;
	    } else {
	      return AddressBook::InternError; // this may not happen
	    }
	} else {
	  CHECK(book->noOfEntries()==0); // mirror map inconsistency ?
	  return AddressBook::NoEntry;
	}
    } else {
      return AddressBook::InternError;
    }
  // ############################################################################  
}

AddressBook::ErrorCode KabAPI::add(const AddressBook::Entry& entry, KabKey& key,
				   bool update)
{
  REQUIRE(book!=0);
  register bool GUARD; GUARD=true;
  // ############################################################################  
  if(book->add(entry, key, update)!=AddressBook::NoError)
    {
      KMessageBox::sorry(this, i18n("Your new entry could not be added."));
      return AddressBook::InternError;
    } else {
      return AddressBook::NoError;
    }
  // ############################################################################  
}

/*
  AddressBook::ErrorCode KabAPI::edit(AddressBook::Entry& entry)
  {
  REQUIRE(widget!=0);
  register bool GUARD; GUARD=true;
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit[foreign entry]: called.\n");
  // ############################################################################  
  EditEntryDialog dialog(this);
  // -----
  dialog.setEntry(entry);
  if(dialog.exec())
  {
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::add: dialog finished with accept().\n");
  entry=dialog.getEntry();
  } else {
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::add: dialog finished with reject().\n");
  return Rejected;
  }  
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit[foreign entry]: finished.\n");
  return NoError;
  // ############################################################################  
  }

  AddressBook::ErrorCode KabAPI::edit(const QCString& key)
  {
  REQUIRE(widget!=0);
  register bool GUARD; GUARD=true;
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit: called.\n");
  // ############################################################################  
  AddressBook::Entry entry;
  // -----
  if(widget->isRO())
  {
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit: database is readonly.\n");
  return PermDenied;
  }
  if(widget->getEntry(key, entry))
  {
  if(edit(entry)==NoError)
  {
  if(!widget->change(key, entry))
  {
  kDebugInfo("KabAPI::edit: Changing the entry failed!");
  CHECK(false);
  }
  } else {
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit: rejected.\n");
  return Rejected;
  }
  } else {
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit: no such entry.\n");
  return NoEntry;
  }
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::edit: finished.\n");
  return NoError;
  // ############################################################################  
  }

*/

AddressBook::ErrorCode KabAPI::remove(const KabKey& key)
{
  REQUIRE(book!=0);
  register bool GUARD; GUARD=true;
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::remove: called.\n");
  // ############################################################################  
  if(book->AddressBook::remove(key)==AddressBook::NoError)
    {
      return AddressBook::NoError;
    } else {
      return AddressBook::NoEntry;
    }
  // ############################################################################  
}

AddressBook::ErrorCode KabAPI::getEntryByName(const QString&, 
					 list<AddressBook::Entry>&, const int)
{
  // ############################################################################  
  return AddressBook::NotImplemented;
  // ############################################################################  
}

AddressBook::ErrorCode KabAPI::getEntryByName(const AddressBook::Entry&,
					 list<AddressBook::Entry>&, const int)
{
  // ############################################################################  
  return AddressBook::NotImplemented;
  // ############################################################################  
}

AddressBook::ErrorCode KabAPI::getEntries(list<AddressBook::Entry>& entries)
{
  register bool GUARD; GUARD=true;
  REQUIRE(entries.empty());
  REQUIRE(book!=0);
  kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::getEntries: called.\n");
  // ############################################################################  
  if(book->noOfEntries()==0)
    { // ----- database is valid, but empty:
      kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::getEntries: no entries.\n");
      return AddressBook::NoEntry;
    }
  if(!book->getEntries(entries))
    {
      kDebugInfo("KabAPI::getEntries: intern error.\n");
      return AddressBook::InternError;
    } else {
      kDebugInfo(GUARD, KAB_KDEBUG_AREA, "KabAPI::getEntries: done.\n");
      return AddressBook::NoError;
    }
  // ############################################################################  
}

AddressBook::ErrorCode KabAPI::sendEmail(const QString& /* address */,
				    const QString& /* subject */)
{
  REQUIRE(book!=0);
  // ############################################################################
  // WORK_TO_DO: kabapi is  not supposed to send emails.
  /*
    if(widget->sendEmail(address, subject))
    {
    return NoError;
    } else {
    return InternError;
    }
  */
  return AddressBook::NotImplemented;
  // ############################################################################
}

AddressBook* KabAPI::addressbook()
{
  // ############################################################################
  return book;
  // ############################################################################
}

AddressBook::ErrorCode KabAPI::save(bool force)
{
  REQUIRE(book!=0);
  // ############################################################################
  if(book->save("", force)!=AddressBook::NoError)
    {
      return AddressBook::PermDenied;
    } else {
      return AddressBook::NoError;
    }
  // ############################################################################
}

void KabAPI::entrySelected(int index)
{
  CHECK(index>=0 && (unsigned)index<book->noOfEntries());
  debug("KabAPI::entrySelected: entry %i selected.", index);
  selection=index;
}

void KabAPI::setStatusSlot(const QString& text)
{
  emit(setStatus(text));
}
