// -*- c++ -*-
/* This file is part of the KDE libraries
    Copyright (C) 1997 Stephan Kulow <coolo@kde.org>

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

#ifndef KFILEVIEW_H
#define KFILEVIEW_H

class KFileViewItem;
class KFileViewItemList;
class QSignal;
class QPoint;

#include "kfilereader.h"
#include <qwidget.h>

/**
 * internal class to make easier to use signals possible
 * @internal
 **/
class KFileViewSignaler : public QObject {
    Q_OBJECT

public:
    void activateDir(const KFileViewItem *i) { emit dirActivated(i); }
    void highlightFile(const KFileViewItem *i) { emit fileHighlighted(i); }
    void activateFile(const KFileViewItem *i) { emit fileSelected(i); }
    void activateMenu( const KFileViewItem *i )
    { emit activatedMenu( i ); }

signals:
    void dirActivated(const KFileViewItem*);
    void fileHighlighted(const KFileViewItem*);
    void fileSelected(const KFileViewItem*);
    void activatedMenu( const KFileViewItem *i );
};

/**
  * A base class for views of the KDE file selector
  *
  * This class defines an interface to all file views. Its intent is
  * to allow to switch the view of the files in the selector very easily.
  * It defines some pure virtual functions, that must be implemented to
  * make a file view working.
  *
  * Since this class is not a widget, but it's meant to be added to other
  * widgets, its most important function is @code widget. This should return
  * a pointer to the implemented widget.
  *
  * @short A base class for views of the KDE file selector
  * @author Stephan Kulow <coolo@kde.org>
  * @version $Id$
  **/
class KFileView {

public:
    enum ViewMode {
	Files = 1,
	Directories = 2,
	All = Files | Directories
    };
    enum SelectionMode {
	Single,
	Multi
    };

    KFileView( );

    /** Destructor */
    virtual ~KFileView();

    /**
     * inserts a list of items. This is a speed optimized version of
     * function above.
     **/
    virtual void addItemList(const KFileViewItemList *list);

    /**
      * a pure virtual function to get a QWidget, that can be added to
      * other widgets. This function is needed to make it possible for
      * derived classes to derive from other widgets.
      **/
    virtual QWidget *widget() = 0;

    /**
      * set the current item in the view.
      *
      * the normal use of this function is to use the parameter filename to
      * set the highlight on this file.
      *
      * the extend use of this function is to set filename to 0. In this
      * case the @see KFileViewItem is used to find out the position of the
      * file. Since this is faster, this way is used in case a view has
      * other child views.
      **/
    virtual void setCurrentItem(const QString &filename,
				const KFileViewItem * entry = 0);

    /**
     * clears the view and and all item lists
      */
    virtual void clear();

    /**
      * does a repaint of the view.
      *
      * The default implementation calls
      * <code>widget()->repaint(f)</code>
      **/
    virtual void updateView(bool f = true);

    virtual void updateView(const KFileViewItem*);

    /**
      * specifies the sorting of the internal list. Newly added files
      * are added through this sorting.
      *
      **/
    QDir::SortSpec sorting() const { return mySorting; }

    /**
      * set the sorting of the view
      *
      * Default is QDir::Name | QDir::IgnoreCase | QDir::DirsFirst
      * Don't use QDir::Reversed, use @ref sortReversed() if you want to
      * reverse the sort order.
      * Calling this method keeps the reversed-setting
      * @see #setSortMode
      **/
    void setSorting(QDir::SortSpec sort); // a little bit complexer

    /**
      * Increasing means greater indicies means bigger values
      *
      * Decrease means greater indicies means smaller values
      * Switching is deprecated, don't use that anymore!
      **/
    enum SortMode { Increasing, Decreasing, Switching };

    /**
      * set the sorting mode. Default mode is Increasing. Affects only
      * newly added items.
      * @see #setSorting
      **/
    void setSortMode(SortMode mode) { mySortMode = mode; }

    /**
     * @returns the current sort mode
     * @see #setSortMode
     * @see #setSorting
     */
    SortMode sortMode() const { return mySortMode; }

    /**
     * Toggles the current sort order, i.e. the order is reversed.
     * @see #isReversed
     */
    void sortReversed();

    /**
     * Tells whether the current items are in reversed order (= contrary to
     * @ref sortMode).
     */
    bool isReversed() const { return reversed; }

    /**
      * @returns returns the number of dirs and files
      **/
    uint count() const { return filesNumber + dirsNumber; }

    /**
      * @returns the number of files.
      **/
    uint numFiles() const { return filesNumber; }

    /**
      * @returns the number of directories
      **/
    uint numDirs() const { return dirsNumber; }

    virtual void setSelectMode( SelectionMode sm );
    virtual SelectionMode selectMode() const;

    virtual void setViewMode( ViewMode vm );
    virtual ViewMode viewMode() const;

    /**
     * @returns the localized name of the view, which could be displayed
     * somewhere, e.g. in a menu, where the user can choose between views.
     * @see #setViewName
     */
    QString viewName() { return viewname; }

    /**
     * Sets the name of the view, which could be displayed somewhere.
     * E.g. "Image Preview".
     */
    void setViewName( const QString& name ) { viewname = name; }

    virtual void setOperator(QObject *ops);

    /**
     * The derived view must implement this function to add
     * the file in the widget.
     *
     **/
    virtual void insertItem( KFileViewItem *i) = 0;

    /**
     * pure virtual function, that should be implemented to clear
     * the view. At this moment the list is already empty
     **/
    virtual void clearView() = 0;

    /**
     * Clear any selection, unhighlight everything. Must be implemented by the
     * view.
     */
    virtual void clearSelection() = 0;
    
    /**
     * Selects all items. You may want to override this, if you can implement
     * it more efficiently than calling highlightItem() with every item.
     * This works only in Multiselection mode of course.
     */
    virtual void selectAll();

    /**
     * @returns whether the given item is currently selected.
     * Must be implemented by the view.
     */
    virtual bool isSelected( const KFileViewItem * ) const = 0;
    
    /**
     * @returns all currently highlighted items.
     */
    const KFileViewItemList * selectedItems() const;

    /**
     * @returns all items currently available
     */
    const KFileViewItemList * items() const;
    
    virtual void insertSorted(KFileViewItem *first, uint counter);

protected:

    /**
      * Tells the view that it should highlight the item. 
      * This function must be implemented by the view
      **/
    virtual void highlightItem(const KFileViewItem *) = 0;

    /**
      * Call this method when an item is selected (depends on single click /
      * double click configuration). Emits the appropriate signal.
      **/
    void select( const KFileViewItem *entry);

    /**
     * emits the highlighted signal for item. Call this in your subclass, 
     * whenever the selection changes.
     */
    void highlight( const KFileViewItem *item);

    /**
     * compares two items in the current context (sortMode and others)
     * returns -1, if i1 is before i2 and 1, if the other case is true
     * in case, both are equal (in current context), the behaviour is
     * undefined!
     **/
    int compareItems(const KFileViewItem *fi1, const KFileViewItem *fi2) const;

    /**
     * this is a help function for sorting, since I can't use the libc
     * version (because I have a variable sort function)
     *
     */
    void QuickSort(KFileViewItem* a[], int lo0, int hi0) const;

    bool updateNumbers(const KFileViewItem *i);

    KFileViewItem *mergeLists(KFileViewItem *list1, KFileViewItem *list2);

    void activateMenu( const KFileViewItem *i );

    /**
     * @internal
     * class to distribute the signals
     **/
    KFileViewSignaler *sig;

    KFileViewItem *firstItem() const { return first; }

private:

    bool reversed;
    QDir::SortSpec mySorting;
    static QDir::SortSpec defaultSortSpec;
    enum SortMode mySortMode;
    QString viewname;

    /**
     * counters
     **/
    uint filesNumber;
    uint dirsNumber;

    ViewMode view_mode;
    SelectionMode selection_mode;

    KFileViewItem *first;
    mutable KFileViewItemList *itemList, *selectedList;
    mutable  bool itemListDirty;
};

#endif // KFILEINFOLISTWIDGET_H
