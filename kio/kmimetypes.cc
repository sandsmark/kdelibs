#include "kmimetypes.h"
#include "kmimemagic.h"
#include "kservices.h"
#include "krun.h"
#include "kautomount.h"
#include "kio_job.h"
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#include <qstring.h>
#include <qmessagebox.h>

#include <ksimpleconfig.h>
#include <kapp.h>
#include <kurl.h>
#include <kdebug.h>

QDict<KMimeType>* KMimeType::s_mapTypes = 0L;
KMimeType* KMimeType::s_pDefaultType = 0L;
bool KMimeType::s_bChecked = false;

void KMimeType::initStatic()
{
  if ( s_mapTypes != 0L )
    return;
  
  s_mapTypes = new QDict<KMimeType>;

  /*
  // Read the application bindings in the local directories
  QString path = kapp->localkdedir().data();
  path += "/share/mimelnk";
  scanMimeTypes( path.data() );
    
  // Read the application bindings in the global directories
  path = kapp->kde_mimedir().copy();
  scanMimeTypes( path.data() );  */
}

void KMimeType::check()
{
  if ( s_bChecked )
    return;
  initStatic();

  kdebug( KDEBUG_INFO, 7009, "================== %d MTs ==========", s_mapTypes->count() );

  s_bChecked = true; // must be done before building mimetypes
  
  // Try to find the default type
  if ( ( s_pDefaultType = KMimeType::find( "application/octet-stream" ) ) == 0L )
    errorMissingMimeType( "application/octet-stream" );

  // No default type ?
  if ( s_pDefaultType == 0L )
  {
    QStrList tmp;
    s_pDefaultType = new KMimeType( "application/octet-stream", "unknown.xpm", "", tmp );
  }
  
  // No Mime-Types installed ?
  // Lets do some rescue here.
  if ( s_mapTypes->count() <= 1 )
    QMessageBox::critical( 0, i18n( "KFM Error" ), i18n( "No mime types installed!" ), i18n( "OK" ) );
	
  if ( KMimeType::find( "inode/directory" ) == s_pDefaultType )
    errorMissingMimeType( "inode/directory" );
  if ( KMimeType::find( "inode/directory-locked" ) == s_pDefaultType )
    errorMissingMimeType( "inode/directory-locked" );
  if ( KMimeType::find( "inode/blockdevice" ) == s_pDefaultType )
    errorMissingMimeType( "inode/blockdevice" );
  if ( KMimeType::find( "inode/chardevice" ) == s_pDefaultType )
    errorMissingMimeType( "inode/chardevice" );
  if ( KMimeType::find( "inode/socket" ) == s_pDefaultType )
    errorMissingMimeType( "inode/socket" );
  if ( KMimeType::find( "inode/fifo" ) == s_pDefaultType )
    errorMissingMimeType( "inode/fifo" );
  if ( KMimeType::find( "application/x-shellscript" ) == s_pDefaultType )
    errorMissingMimeType( "application/x-shellscript" );
  if ( KMimeType::find( "application/x-executable" ) == s_pDefaultType )
    errorMissingMimeType( "application/x-executable" );
  if ( KMimeType::find( "application/x-kdelnk" ) == s_pDefaultType )
    errorMissingMimeType( "application/x-kdelnk" );
}

void KMimeType::errorMissingMimeType( const char *_type )
{
  QString tmp = i18n( "Could not find mime type\n" );
  tmp += _type;
    
  QMessageBox::critical( 0, i18n( "KFM Error" ), tmp, i18n("OK" ) );

  QString mime = _type;
  QStrList dummy;
  
  KMimeType *e;
  if ( mime == "inode/directory" )
    e = new KFolderType( mime, "unknown.xpm", "", dummy );
  else if ( mime == "application/x-kdelnk" )
    e = new KDELnkMimeType( mime, "unknown.xpm", "", dummy );
  else if ( mime == "application/x-executable" || mime == "application/x-shellscript" )
    e = new KExecMimeType( mime, "unknown.xpm", "", dummy );
  else
    e = new KMimeType( mime, "unknown.xpm", "", dummy );

  s_mapTypes->insert( _type, e );
}

///////// Old method, never called ////////////
void KMimeType::scanMimeTypes( const char* _path )
{   
  initStatic();

  DIR *dp;
  struct dirent *ep;
  dp = opendir( _path );
  if ( dp == 0L )
    return;
    
  // Loop thru all directory entries
  while ( (ep = readdir( dp ) ) != 0L )
  {
    if ( strcmp( ep->d_name, "." ) != 0 && strcmp( ep->d_name, ".." ) != 0 &&
	 strcmp( ep->d_name, "magic" ) != 0 && ep->d_name[0] != '.' )
    {
      string file = _path;
      file += "/";
      file += ep->d_name;

      struct stat buff;
      stat( file.c_str(), &buff );
      if ( S_ISDIR( buff.st_mode ) )
	scanMimeTypes( file.data() );
      else if ( S_ISREG( buff.st_mode ) )
      {
	if ( access( file.c_str(), R_OK ) == -1 )
	  continue;

	KSimpleConfig config( file.c_str(), true );
	config.setGroup( "KDE Desktop Entry" );
		
	// Get a ';' separated list of all pattern
	QString pats = config.readEntry( "Patterns" );
	QString icon = config.readEntry( "Icon" );
	QString comment = config.readEntry( "Comment" );
	QString mime = config.readEntry( "MimeType" );

	if ( mime.isEmpty() )
	{
          QString tmp = i18n( "The mime type config file\n"
			      "%1\n"
			      "does not contain a MimeType=... entry")
	      .arg(file.c_str() );

	  QMessageBox::critical( 0L, i18n( "KFM Error" ), tmp, i18n( "OK" ) );
	  continue;
	}
      
	// Is this file type already registered ?
	if ( KMimeType::find( mime ) )
	  continue;
	// If not then create a new type
	if ( icon.isEmpty() )
	  icon = "unknown.xpm";
      
	QStrList patterns;
	int pos2 = 0;
	int old_pos2 = 0;
	while ( ( pos2 = pats.find( ";", pos2 ) ) != - 1 )
        {
	  // Read a pattern from the list
	  QString name = pats.mid( old_pos2, pos2 - old_pos2 );
	  patterns.append( name );
	  pos2++;
	  old_pos2 = pos2;
	}

	if ( mime == "inode/directory" )
	  new KFolderType( mime.data(), icon.data(), comment.data(), patterns );
	else if ( mime == "inode/directory-locked" )
	  new KFolderType( mime.data(), icon.data(), comment.data(), patterns );
	else if ( mime == "application/x-kdelnk" )
	  new KDELnkMimeType( mime.data(), icon.data(), comment.data(), patterns );
	else if ( mime == "application/x-executable" || mime == "application/x-shellscript" )
	  new KExecMimeType( mime.data(), icon.data(), comment.data(), patterns );
	else
	  new KMimeType( mime.data(), icon.data(), comment.data(), patterns );
      }
    }
  }

  closedir(dp);
}

KMimeType* KMimeType::find( const char *_name )
{
  check();

  assert( s_mapTypes );

  KMimeType* mime = (*s_mapTypes)[ _name ];
  if ( !mime )
    return s_pDefaultType;

  return mime;
}

KMimeType* KMimeType::findByURL( const KURL& _url, mode_t _mode,
				 bool _is_local_file, bool _fast_mode )
{
  check();

  if ( !_fast_mode && !_is_local_file && _url.isLocalFile() )
    _is_local_file = true;
  
  if ( !_fast_mode && _is_local_file && _mode == 0 )
  {
    struct stat buff;
    if ( stat( _url.path(), &buff ) != -1 )
      _mode = buff.st_mode;
  }

  // Look at mode_t first
  if ( S_ISDIR( _mode ) )
  {
    // Special hack for local files. We want to see whether we
    // are allowed to enter the directory
    if ( _is_local_file )
    {
      QString path ( _url.path( 0 ) );
      if ( access( path.data(), R_OK ) == -1 )
	return find( "inode/directory-locked" );
    }
    return find( "inode/directory" );
  }
  if ( S_ISCHR( _mode ) )
    return find( "inode/chardevice" );
  if ( S_ISBLK( _mode ) )
    return find( "inode/blockdevice" );
  if ( S_ISFIFO( _mode ) )
    return find( "inode/fifo" );
  if ( S_ISSOCK( _mode ) )
    return find( "inode/socket" );
  // KMimeMagic can do that better for local files
  if ( !_is_local_file && S_ISREG( _mode ) && ( _mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) ) )
    return find( "application/x-executable" );
      
  QString path ( _url.path( 0 ) );
  
  // Try to find it out by looking at the filename
  assert( s_mapTypes );
  QDictIterator<KMimeType> it( *s_mapTypes );
  for( ; it.current() != 0L; ++it )
    if ( it.current()->matchFilename( path.data() ) )
      return it.current();
  
  // Another filename binding, hardcoded, is .kdelnk :
  if ( !strcmp(path.right(7), ".kdelnk") )
    return find( "application/x-kdelnk" );

  if ( !_is_local_file || _fast_mode )
  {
    QString path = _url.path();
    if ( path.right(1) == "/" || path.isEmpty() )
      return find( "inode/directory" );
  }
  
  // No more chances for non local URLs
  if ( !_is_local_file || _fast_mode )
    return find( "application/octet-stream" );
  
  // Do some magic for local files
  kdebug( KDEBUG_INFO, 7009, "Mime Type finding for '%s'", path.data() );
  KMimeMagicResult* result = KMimeMagic::self()->findFileType( path );
  /** DEBUG code **/
  /* assert( result );
  assert( result->mimeType() );
  assert( result->mimeType()[0] != 0 ); */
  /** End Debug code **/

  // If we still did not find it, we must assume the default mime type
  if ( !result || !result->isValid() )  /* !result->mimeType() || result->mimeType()[0] == 0 ) */
    return find( "application/octet-stream" );

  // The mimemagic stuff was successful
  return find( result->mimeType() );
}

KMimeType::KMimeType( const char *_type, const char *_icon, const char *_comment, QStrList& _patterns )
{
  initStatic();
  
  assert( s_mapTypes );
  
  s_mapTypes->insert( _type, this );
  m_strIcon = _icon;
  m_strComment = _comment;
  m_strMimeType = _type;
  m_lstPatterns = _patterns;
}

KMimeType::~KMimeType()
{
  s_mapTypes->remove( m_strMimeType );
}

bool KMimeType::matchFilename( const char *_filename )
{
  int len = strlen( _filename );
  
  const char *s;
  for( s = m_lstPatterns.first(); s != 0L; s = m_lstPatterns.next() )
  {
    int pattern_len = strlen( s );
    if (!pattern_len)
      continue;

    if ( s[ pattern_len - 1 ] == '*' && len + 1 >= pattern_len )
      if ( strncasecmp( _filename, s, pattern_len - 1 ) == 0 )
	return true;
    
    if ( s[ 0 ] == '*' && len + 1 >= pattern_len )
      if ( strncasecmp( _filename + len - pattern_len + 1, s + 1, pattern_len - 1 ) == 0 )
	return true;
    if ( strcasecmp( _filename, s ) == 0 )
      return true;
  }

  return false;
}

/*******************************************************
 *
 * KFolderType
 *
 ******************************************************/

KFolderType::KFolderType( const char *_type, const char *_icon, const char *_comment, QStrList& _patterns )
  : KMimeType( _type, _icon, _comment, _patterns )
{
}

QString KFolderType::icon( const char *_url, bool _is_local )
{
  if ( !_is_local || !_url )
    return KMimeType::icon( _url, _is_local );

  KURL u( _url );
  u.addPath( ".directory" );
  
  KSimpleConfig cfg( u.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString icon = cfg.readEntry( "Icon" );
  QString empty_icon = cfg.readEntry( "EmptyIcon" );

  if ( !empty_icon.isEmpty() )
  {
    bool isempty = true;
    DIR *dp = 0L;
    struct dirent *ep;
    dp = opendir( u.path() );
    if ( dp )
    {
      ep=readdir( dp );
      ep=readdir( dp );      // ignore '.' and '..' dirent
      if ( readdir( dp ) == 0L ) // third file is NULL entry -> empty directory
	isempty = false;
      closedir( dp );
    }
    
    if ( isempty )
      return empty_icon;
  }

  if ( icon.isEmpty() )
    return KMimeType::icon( _url, _is_local );

  return icon;
}

QString KFolderType::icon( KURL& _url, bool _is_local )
{
  if ( !_is_local )
    return KMimeType::icon( _url, _is_local );

  KURL u( _url );
  u.addPath( ".directory" );
  
  KSimpleConfig cfg( u.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString icon = cfg.readEntry( "Icon" );
  if ( icon.isEmpty() )
    return KMimeType::icon( _url, _is_local );

  return icon;
}

QString KFolderType::comment( const char *_url, bool _is_local )
{
  if ( !_is_local || !_url )
    return KMimeType::comment( _url, _is_local );

  KURL u( _url );
  u.addPath( ".directory" );

  KSimpleConfig cfg( u.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString comment = cfg.readEntry( "Comment" );
  if ( comment.isEmpty() )
    return KMimeType::comment( _url, _is_local );

  return comment;
}

QString KFolderType::comment( KURL& _url, bool _is_local )
{
  if ( !_is_local )
    return KMimeType::comment( _url, _is_local );

  KURL u( _url );
  u.addPath( ".directory" );

  KSimpleConfig cfg( u.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString comment = cfg.readEntry( "Comment" );
  if ( comment.isEmpty() )
    return KMimeType::comment( _url, _is_local );

  return comment;
}

/*******************************************************
 *
 * KDELnkMimeType
 *
 ******************************************************/

KDELnkMimeType::KDELnkMimeType( const char *_type, const char *_icon, const char *_comment, QStrList& _patterns )
  : KMimeType( _type, _icon, _comment, _patterns )
{
}

QString KDELnkMimeType::icon( const char *_url, bool _is_local )
{
  if ( !_is_local || !_url )
    return KMimeType::icon( _url, _is_local );

  KURL u( _url );
  return icon( u, _is_local );
}

QString KDELnkMimeType::icon( KURL& _url, bool _is_local )
{
  if ( !_is_local )
    return KMimeType::icon( _url, _is_local );

  KSimpleConfig cfg( _url.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString icon = cfg.readEntry( "Icon" );
  QString type = cfg.readEntry( "Type" );

  if ( type == "FSDevice" )
  {
    QString unmount_icon = cfg.readEntry( "UnmountIcon" );
    QString dev = cfg.readEntry( "Dev" );
    if ( !icon.isEmpty() && !unmount_icon.isEmpty() && !dev.isEmpty() )
    {
      QString mp = KIOJob::findDeviceMountPoint( dev );
      // Is the device not mounted ?
      if ( mp.isNull() )
	return unmount_icon;
    }
  }
  
  if ( icon.isEmpty() )
    return KMimeType::icon( _url, _is_local );

  return icon;
}

QString KDELnkMimeType::comment( const char *_url, bool _is_local )
{
  if ( !_is_local || !_url )
    return KMimeType::comment( _url, _is_local );

  KURL u( _url );
  return comment( u, _is_local );
}

QString KDELnkMimeType::comment( KURL& _url, bool _is_local )
{
  if ( !_is_local )
    return KMimeType::comment( _url, _is_local );

  KSimpleConfig cfg( _url.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString comment = cfg.readEntry( "Comment" );
  if ( comment.isEmpty() )
    return KMimeType::comment( _url, _is_local );

  return comment;
}

bool KDELnkMimeType::run( const char *_url, bool _is_local )
{
  // It might be a security problem to run external untrusted kdelnk files
  if ( !_is_local )
    return false;

  KURL u( _url );
  
  KSimpleConfig cfg( u.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString type = cfg.readEntry( "Type" );
  if ( type.isEmpty() )
  {
    QString tmp = i18n("The kdelnk file\n%1\n"
		       "has no Type=... entry").arg(_url );
    QMessageBox::critical( 0L, i18n("Error"), tmp, i18n( "OK" ) );
    return false;
  }

  kdebug( KDEBUG_INFO, 7009, "TYPE = %s", type.data() );
  
  if ( type == "FSDevice" )
    return runFSDevice( _url, cfg );
  else if ( type == "Application" )
    return runApplication( _url, cfg );
  else if ( type == "Link" )
    return runLink( _url, cfg );
  else if ( type == "MimeType" )
    return runMimeType( _url, cfg );
  
  
  QString tmp = i18n("The kdelnk type\n%1\nis unknown").arg( type );
  QMessageBox::critical( 0L, i18n("Error"), tmp, i18n( "OK" ) );
  
  return false;    
}

bool KDELnkMimeType::runFSDevice( const char *_url, KSimpleConfig &cfg )
{
  QString point = cfg.readEntry( "MountPoint" );
  QString dev = cfg.readEntry( "Dev" );

  if ( dev.isEmpty() )
  {
    QString tmp = i18n("The kdelnk file\n%1\nis of type FSDevice but has no Dev=... entry").arg( _url );
    QMessageBox::critical( 0L, i18n("Error"), tmp, i18n( "OK" ) );
    return false;
  }

  QString mp = KIOJob::findDeviceMountPoint( dev );
  // Is the device already mounted ?
  if ( !mp.isNull() )
  {
    QString mp2 = "file:";
    mp2 += mp;
    // Open a new window
    KFileManager::getFileManager()->openFileManagerWindow( mp2 );
  }
  else
  {    
    QString readonly = cfg.readEntry( "ReadOnly" );
    bool ro = FALSE;
    if ( !readonly.isNull() )
      if ( readonly == "1" )
	ro = true;
		    
    (void) new KAutoMount( ro, 0L, dev, 0L );
  }

  return true;
}

bool KDELnkMimeType::runApplication( const char *_url, KSimpleConfig &cfg )
{
  KService* s = KService::parseService( _url, cfg, false );
  if ( !s )
    // The error message was already displayed, so we can just quit here
    return false;
  
  QStringList empty;
  bool res = KRun::run( *s, empty );
  delete s;
  return res;
}

bool KDELnkMimeType::runLink( const char *_url, KSimpleConfig &cfg )
{
  QString url = cfg.readEntry( "URL" );
  if ( url.isEmpty() )
  {
    QString tmp = i18n("The kdelnk file\n%1\nis of type Link but has no URL=... entry").arg( _url );
    QMessageBox::critical( 0L, i18n("Error"), tmp, i18n( "OK" ) );
    return false;
  }

  (void)new KRun( url );

  return true;
}

bool KDELnkMimeType::runMimeType( const char *_url, KSimpleConfig &cfg )
{
  // HACK: TODO
  return false;
}

void KDELnkMimeType::builtinServices( KURL& _url, list<Service>& _lst )
{
  if ( !_url.isLocalFile() )
    return;
  
  KSimpleConfig cfg( _url.path(), true );
  cfg.setGroup( "KDE Desktop Entry" );
  QString type = cfg.readEntry( "Type" );
  if ( type.isEmpty() )
    return;
  
  if ( type == "FSDevice" )
  {
    QString dev = cfg.readEntry( "Dev" );
    if ( dev.isEmpty() )
    {
      QString tmp = i18n("The kdelnk file\n%1\nis of type FSDevice but has no Dev=... entry").arg( _url.path() );
      QMessageBox::critical( 0L, i18n("Error"), tmp, i18n( "OK" ) );
    }
    else
    {
      QString mp = KIOJob::findDeviceMountPoint( dev );
      // not mounted ?
      if ( mp.isEmpty() )
      {
	Service mount;
	mount.m_strName = i18n("Mount");
	mount.m_type = ST_MOUNT;
	_lst.push_back( mount );  
      }
      else
      {
	Service unmount;
	unmount.m_strName = i18n("Unmount");
	unmount.m_type = ST_UNMOUNT;
	_lst.push_back( unmount );  
      }
    }
  }

  /* Service props;
  props.m_strName = i18n("Properties");
  props.m_type = ST_PROPERTIES;
  _lst.push_back( props );   */
}

void KDELnkMimeType::userDefinedServices( KURL& _url, list<KDELnkMimeType::Service>& _lst )
{
  if ( !_url.isLocalFile() )
    return;

  KSimpleConfig cfg( _url.path(), true );
  cfg.setGroup( "Menu" );
  
  QStrList keys;
  if ( cfg.readListEntry( "Menus", keys ) <= 0 )
    return;
  
  const char *k;
  for( k = keys.first(); k != 0L; k = keys.next() )
  {
    kdebug( KDEBUG_INFO, 7009, "CURRENT KEY = %s", k );
   
    QStrList lst;
    if ( cfg.readListEntry( k, lst ) == 3 )
    {
      Service s;
      s.m_strName = lst.at(0);
      s.m_strIcon = lst.at(1);
      s.m_strExec = lst.at(2);
      s.m_type = ST_USER_DEFINED;
      _lst.push_back( s ); 
    }
    else
    {
      QString tmp = i18n("The kdelnk file\n%1\n has an invalid menu entry\n%2").arg( _url.path()).arg( k );
      QMessageBox::critical( 0L, i18n("Error"), tmp, i18n("OK" ) );
    }
  }
}

void KDELnkMimeType::executeService( const char *_url, KDELnkMimeType::Service& _service )
{
  kdebug( KDEBUG_INFO, 7009, "EXECUTING Service %s", _service.m_strName.data() );
  
  KURL u( _url );
  
  if ( _service.m_type == ST_USER_DEFINED )
  {
    QStringList lst;
    KRun::run( _service.m_strExec, lst, _service.m_strName, _service.m_strIcon,
	       _service.m_strIcon, u.path() );
    return;
  }
  /* else if ( _service.m_type == ST_PROPERTIES )
  {
  } */
  else if ( _service.m_type == ST_MOUNT || _service.m_type == ST_UNMOUNT )
  {
    kdebug( KDEBUG_INFO, 7009, "MOUNT&UNMOUNT" );

    KSimpleConfig cfg( u.path(), true );
    cfg.setGroup( "KDE Desktop Entry" );
    QString dev = cfg.readEntry( "Dev" );
    if ( dev.isEmpty() )
    {
      QString tmp = i18n("The kdelnk file\n%1\nis of type FSDevice but has no Dev=... entry").arg( u.path() );
      QMessageBox::critical( 0L, i18n("Error"), tmp, i18n( "OK" ) );
      return;
    }
    QString mp = KIOJob::findDeviceMountPoint( dev );

    if ( _service.m_type == ST_MOUNT )
    {
      // Already mounted? Strange, but who knows ...
      if ( !mp.isEmpty() )
      {
	kdebug( KDEBUG_INFO, 7009, "ALREADY Mounted" );
	return;
      }
      
      QString readonly = cfg.readEntry( "ReadOnly" );
      bool ro = false;
      if ( !readonly.isNull() )
	if ( readonly == "1" )
	  ro = true;

      (void)new KAutoMount( ro, 0L, dev, 0L, false );
    }
    else if ( _service.m_type == ST_UNMOUNT )
    {
      // Not mounted? Strange, but who knows ...
      if ( mp.isEmpty() )
	return;

      (void)new KAutoUnmount( mp );
    }
  }
  else
    assert( 0 );
}

/*******************************************************
 *
 * KExecMimeType
 *
 ******************************************************/

KExecMimeType::KExecMimeType( const char *_type, const char *_icon, const char *_comment, QStrList& _patterns )
  : KMimeType( _type, _icon, _comment, _patterns )
{
}

