// $Id$
// Revision 1.1.1.3  1997/12/11 07:19:11  jacek
// $Log$
// Kalle: Copyright headers
// kdoctoolbar removed
//
// Revision 1.3  1997/10/09 11:46:20  kalle
// Assorted patches by Fritz Elfert, Rainer Bawidamann, Bernhard Kuhn and Lars Kneschke
//
// Revision 1.2  1997/10/04 19:42:46  kalle
// new KConfig
//
// Revision 1.1.1.1  1997/04/13 14:42:41  cvsuser
// Source imported
//
// Revision 1.1.1.1  1997/04/09 00:28:07  cvsuser
// Sources imported
//
// Revision 1.1  1996/11/24 12:54:42  kalle
// Initial revision
//
//
#include <Kconfig.h>
//
// (C) 1996 Matthias Kalle Dalheimer <mda@stardivision.de>

#ifndef _KCOLORGROUP_H
#define _KCOLORGROUP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _KCONFIG_H
#include <kconfig.h>
#endif

#include <qpalette.h>
#include <qstring.h>

/// KColorGroup: A collection of color entries for application widgets
/** A KColorGroup provides seven colors for widgets: foreground,
  background, light, mid, dark, text and base - exactly as the
  QColorGroup. KColorGroup adds the possibility to store the
  KColorGroup via the KConfig mechanism. It would be nicer if
  KColorGroup was derived from QColorGroup, but QColorGroup provides
  no write access to its data members except via the constructor.
  */
class KColorGroup
{
private:
  QColorGroup* pQColorGroup;
public:
  /// Construct a KColorGroup with all colors black
  /** Construct a KColorGroup with all colors black */
  KColorGroup();

  /// Construct a KColorGroup with given colors.
  /** Construct a KColorGroup with given colors. */
  KColorGroup( const QColor& rForeground, const QColor& rBackground,
	       const QColor& rLight, const QColor& rMid, const QColor&
	       rDark, const QColor& rText, const QColor& rBase );

  /// KColorGroup destructor
  /** KColorGroup destructor */
  ~KColorGroup();

  /// Retrieve the foreground value.
  /** Retrieve the foreground value. */
  const QColor& foreground() const 
  { return pQColorGroup->foreground(); }

  /// Retrieve the background value.
  /** Retrieve the background value. */
  const QColor& background() const 
  { return pQColorGroup->background(); }

  /// Retrieve the light value.
  /** Retrieve the light value. */
  const QColor& light() const 
  { return pQColorGroup->light(); }

  /// Retrieve the mid value.
  /** Retrieve the mid value. */
  const QColor& mid() const 
  { return pQColorGroup->mid(); }

  /// Retrieve the dark value.
  /** Retrieve the dark value. */
  const QColor& dark() const 
  { return pQColorGroup->dark(); }

  /// Retrieve the text value.
  /** Retrieve the text value. */
  const QColor& text() const 
  { return pQColorGroup->text(); }

  /// Retrieve the base value.
  /** Retrieve the base value. */
  const QColor& base() const 
  { return pQColorGroup->base(); }

    is used. If pString is not NULL, the value of pString is prepended
  /** Compare two KColorGroups. */
  bool operator==( const KColorGroup& rColorGroup ) const
  void load( KConfig& rConfig, QString* pString = NULL );

  /// Compare two KColorGroups.
  /** Compare two KColorGroups. */
    used. If pString is not NULL, the value of pString is prepended to
  { return (*pQColorGroup) != (*rColorGroup.pQColorGroup); }

  void save( KConfig& rConfig, QString* pString = NULL );

  /// Save color values to a KConfig object.
  /** Save color values to a KConfig object. The group "ColorGroup" is
    used. If pString is not 0, the value of pString is prepended to
    the group name.
    */
  void save( KConfig& rConfig, QString* pString = 0L );
  
  /// Return the QColorGroup object
  /** Return the QColorGroup object (for drawing shades e.g.).
   */
  const QColorGroup *colorGroup() const;
};

#endif
