
/**
* kimgio.h -- Implementation of interface to the KDE Image IO library.
* Sirtaj Singh Kang <taj@kde.org>, 23 Sep 1998.
*
* $Id$
*
* This library is distributed under the conditions of the GNU LGPL.
*/

#ifdef HAVE_CONFIG_H
#include"config.h"
#endif

#include <qdir.h>
#include <kapp.h>
#include <kstddirs.h>
#include <qstring.h>
#include <qregexp.h>

#include <ltdl.h>
#include "kimageio.h"
#include "kimageiofactory.h"
#include <klocale.h>
#include <kglobal.h>
#include <kmimetype.h>
#include <ksycocaentry.h>
#include <ksycoca.h>

#include <qimage.h>

class KImageIOFormat : public KSycocaEntry
{
  K_SYCOCATYPE( KST_KImageIOFormat, KSycocaEntry )

public:
  /**
   * Read a KImageIOFormat description file
   */
  KImageIOFormat( const QString & path);
  
  /**
   * @internal construct a ImageIOFormat from a stream
   */ 
  KImageIOFormat( QDataStream& _str, int offset);

  virtual ~KImageIOFormat();

  virtual QString name() const { return mType; }

  virtual bool isValid() const { return true; } 

  /**
   * @internal
   * Load the image format from a stream.
   */
  virtual void load(QDataStream& ); 

  /**
   * @internal
   * Save the image format to a stream.
   */
  virtual void save(QDataStream& );

  /**
   * @internal 
   * Calls image IO function
   */
  void callLibFunc( bool read, QImageIO *);

public:  
  QString mType;
  QString mHeader;
  QString mFlags;
  bool bRead;
  bool bWrite;
  QStringList mSuffices;
  QString mPattern;
  QString mMimetype;
  QString mLib;
  QStringList rPaths;
  bool bLibLoaded;
  void (*mReadFunc)(QImageIO *);
  void (*mWriteFunc)(QImageIO *);
};

class KImageIOFormatList : public QList<KImageIOFormat>
{
public:
   KImageIOFormatList() { }
};


KImageIOFormat::KImageIOFormat( const QString &path)
{
   bLibLoaded = false;
   mReadFunc = 0;
   mWriteFunc = 0;
   KConfig config(path, true, false);

   config.setGroup("Image Format");
   mType = config.readEntry("Type");
   mHeader = config.readEntry("Header");
   KURL::decode(mHeader); // Use URL-like encoding!
   mFlags = config.readEntry("Flags");
   bRead = config.readBoolEntry("Read");
   bWrite = config.readBoolEntry("Write");
   mSuffices = config.readListEntry("Suffices");
   mPattern = config.readEntry("Pattern");
   mMimetype = config.readEntry("Mimetype");
   mLib = config.readEntry("Library");
   rPaths = config.readListEntry("rPaths");
}

KImageIOFormat::KImageIOFormat( QDataStream& _str, int offset) : 
	KSycocaEntry( _str, offset)
{
   bLibLoaded = false;
   mReadFunc = 0;
   mWriteFunc = 0;
   load( _str );
}

KImageIOFormat::~KImageIOFormat() 
{
}

void
KImageIOFormat::load( QDataStream& _str)
{
   Q_INT8 iRead, iWrite;
   _str >> mType >> mHeader >> mFlags >> iRead >> iWrite
        >> mSuffices >> mMimetype >> mLib;
   bRead = (iRead != 0);
   bWrite = (iWrite != 0);
}

void
KImageIOFormat::save( QDataStream& _str)
{
   KSycocaEntry::save( _str );
   Q_INT8 iRead = bRead ? 1 : 0;
   Q_INT8 iWrite = bWrite ? 1 : 0;

   _str << mType << mHeader << mFlags << iRead << iWrite
        << mSuffices << mMimetype << mLib;
}

void 
KImageIOFormat::callLibFunc( bool read, QImageIO *iio)
{
   if (!bLibLoaded)
   {
      if (mLib.isEmpty())
      {
         iio->setStatus(1); // Error
         return;
      }
      lt_dlhandle libhandle = lt_dlopen(mLib.ascii());
      if (libhandle == 0) {
         iio->setStatus(1); // error
         warning("couldn't dlopen %s (%s)",
			mLib.ascii(), lt_dlerror());
         return;
      }
      bLibLoaded = true;
      QString funcName;
      if (bRead)
      {
         funcName = "kimgio_"+mType.lower()+"_read";
         lt_ptr_t func = lt_dlsym(libhandle, funcName.ascii());
 
         if (func == NULL) {
            iio->setStatus(1); // error
            warning("couln't find %s (%s)", funcName.ascii(), lt_dlerror());
         }
         mReadFunc = (void (*)(QImageIO *))func;
      }
      if (bWrite)
      {
         funcName = "kimgio_"+mType.lower()+"_write";
         lt_ptr_t func = lt_dlsym(libhandle, funcName.ascii());
 
         if (func == NULL) {
            iio->setStatus(1); // error
            warning("couln't find %s (%s)", funcName.ascii(), lt_dlerror());
         }
         mWriteFunc = (void (*)(QImageIO *))func;
      }
      
   }
   if (read)
      if (mReadFunc)
         mReadFunc(iio);
      else
         iio->setStatus(1); // Error
   else
      if (mWriteFunc)
         mWriteFunc(iio);
      else
         iio->setStatus(1); // Error
}


KImageIOFactory *KImageIOFactory::_self = 0;
KImageIOFormatList *KImageIOFactory::formatList = 0;

KImageIOFactory::KImageIOFactory() : KSycocaFactory( KST_KImageIO )
{
  _self = this;
  if (m_str)
  {
     // read from database
     (*m_str) >> mReadPattern >> mWritePattern >> rPath;
     if (!formatList)
     {
        formatList = new KImageIOFormatList();
        lt_dlinit(); // Do this only once!
        // Add rPaths.
        for(QStringList::Iterator it = rPath.begin();
            it != rPath.end(); ++it)
           lt_dladdsearchdir( (*it).ascii());  
     }
     load();
  }
  else
  {
     // Build database
     if (!formatList)
     {
        formatList = new KImageIOFormatList();
     }
     m_resourceList->append("lib");
     QStringList list = KGlobal::dirs()->findAllResources("services", "*.kimgio");
     for (QStringList::ConstIterator it = list.begin(); 
          it != list.end(); ++it) 
     {
        KImageIOFormat *format = new KImageIOFormat(*it);
        addEntry(format);
        formatList->append( format );
        rPath += format->rPaths;
     }
     rPath.sort();
     // Remove duplicates from rPath:
     QString last;
     for(QStringList::Iterator it = rPath.begin();
         it != rPath.end(); )
     {
        QStringList::Iterator it2 = it++;
        if (*it2 == last)
        {
           // remove duplicate
           rPath.remove(it2);
        }
        else
        {
           last = *it2;
        }
     }
     mReadPattern = createPattern( KImageIO::Reading );
     mWritePattern = createPattern( KImageIO::Writing );
  }
}

QString
KImageIOFactory::createPattern( KImageIO::Mode _mode)
{
  QStringList patterns;
  QString allPatterns;
  QString wildCard("*.");
  QString seperator("|");
  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (((_mode == KImageIO::Reading) && format->bRead) ||
         ((_mode == KImageIO::Writing) && format->bWrite))
     {
        QString pattern;
        QStringList suffices = format->mSuffices;
        for( QStringList::ConstIterator it = suffices.begin();
             it != suffices.end();
             ++it)
        {
           if (!pattern.isEmpty()) 
              pattern += " ";
           pattern = pattern + wildCard+(*it);
           if (!allPatterns.isEmpty()) 
              allPatterns += " ";
           allPatterns = allPatterns + wildCard +(*it);
        }
        if (!pattern.isEmpty())
        {
           pattern = pattern + seperator + format->mPattern;
           patterns.append(pattern); 
        }
     }
  }
  allPatterns = allPatterns + seperator + i18n("All pictures");
  patterns.sort();
  patterns.prepend(allPatterns);

  QString pattern = patterns.join(QString::fromLatin1("\n"));
  return pattern;
}

void 
KImageIOFactory::readImage( QImageIO *iio)
{
   (void) self(); // Make sure we exist
   const char *fm = iio->format();
   if (!fm)
      fm = QImageIO::imageFormat( iio->ioDevice());
   fprintf(stderr, "KImageIO: readImage() format = %s\n", fm );

   KImageIOFormat *format;
   for( format = formatList->first(); format; 
        format = formatList->next() )
   {
      if (format->mType == fm)
         break;
   }
   if (!format || !format->bRead)
   {
      iio->setStatus(1); // error
      return;
   } 

   format->callLibFunc( true, iio);
}

void 
KImageIOFactory::writeImage( QImageIO *iio)
{
   (void) self(); // Make sure we exist
   const char *fm = iio->format();
   if (!fm)
      fm = QImageIO::imageFormat( iio->ioDevice());
   fprintf(stderr, "KImageIO: writeImage() format = %s\n", fm );

   KImageIOFormat *format;
   for( format = formatList->first(); format; 
        format = formatList->next() )
   {
      if (format->mType == fm)
         break;
   }
   if (!format || !format->bWrite)
   {
      iio->setStatus(1); // error
      return;
   } 

   format->callLibFunc( false, iio);
}

void
KImageIOFactory::load()
{
   m_str->device()->at(m_endEntryOffset);
   Q_INT32 entryCount;
   (*m_str) >> entryCount;

   Q_INT32 *offsetList = new Q_INT32[entryCount];
   for(int i = 0; i < entryCount; i++)
   {
      (*m_str) >> offsetList[i];
   }

   for(int i = 0; i < entryCount; i++)
   {
      KSycocaType type;
      QDataStream *str = KSycoca::self()->findEntry(offsetList[i], type);
      if (type == KST_KImageIOFormat)
      {
         KImageIOFormat *format = new KImageIOFormat(*str, offsetList[i]);

         // Since Qt doesn't allow us to unregister image formats
         // we have to make sure not to add them a second time.
         // This typically happens when the sycoca database was updated 
         // we need to reread it.
         for( KImageIOFormat *_format = formatList->first(); _format; 
              _format = formatList->next() )
         {
            if (format->mType == _format->mType)
            {
               // Already in list
               delete format;
               format = 0;
               break;
            }
         }
         if (!format)
            continue;
         if (!format->mHeader.isEmpty() && !format->mLib.isEmpty())
         {
            void (*readFunc)(QImageIO *);
            void (*writeFunc)(QImageIO *);
            if (format->bRead)
               readFunc = readImage;
            else
               readFunc = 0;
            if (format->bWrite)
               writeFunc = writeImage;
            else
               writeFunc = 0;
            QImageIO::defineIOHandler( format->mType.ascii(), 
                                       format->mHeader.ascii(),
                                       format->mFlags.ascii(), 
                                       readFunc, writeFunc);
         }
         formatList->append( format );
      }
   }
   delete [] offsetList;
}

KImageIOFactory::~KImageIOFactory()
{
  _self = 0;

  // We would like to:
  // * Free all KImageIOFormats.
  // * Unload libs
  // * Remove Qt IO handlers.
  // But we can't remove IO handlers, so we better keep all KImageIOFormats
  // in memory so that we can make sure not register IO handlers again whenever
  // the sycoca database updates (Such event deletes this factory)
}

void
KImageIOFactory::saveHeader(QDataStream &str)
{
   KSycocaFactory::saveHeader(str);
  
   str << mReadPattern << mWritePattern << rPath;
}

void KImageIO::registerFormats()
{
   (void) KImageIOFactory::self();
}

QString 
KImageIO::pattern(Mode _mode) 
{
  if (_mode == Reading)
     return KImageIOFactory::self()->mReadPattern;
  else
     return KImageIOFactory::self()->mWritePattern;
}

bool KImageIO::canWrite(const QString& type)
{
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (format->mType == type)
	return format->bWrite;
  }
  return false;
}

bool KImageIO::canRead(const QString& type)
{
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (format->mType == type)
	return format->bRead;
  }
  return false;
}

QStringList KImageIO::types(Mode _mode ) {
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  KImageIOFormat *format;
  QStringList types;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (((_mode == Reading) && format->bRead) ||
         ((_mode == Writing) && format->bWrite))
        types.append(format->mType);
  }

  return types;
}

QString KImageIO::suffix(const QString& type)
{
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (format->mType == type)
	return format->mSuffices[0];
  }

  return QString::null;
}

QString KImageIO::type(const QString& filename)
{
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  QString suffix = filename;
  int dot = suffix.findRev('.');
  if (dot >= 0)
    suffix = suffix.mid(dot + 1);

  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (format->mSuffices.contains(suffix))
	return format->mType;
  }

  return QString::null;
}

QStringList KImageIO::mimeTypes( Mode _mode )
{
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  QStringList mimeList;
  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (((_mode == Reading) && format->bRead) ||
         ((_mode == Writing) && format->bWrite))
        mimeList.append ( format->mMimetype );
  }

  return mimeList;
}

bool KImageIO::isSupported( const QString& _mimeType, Mode _mode )
{
  KImageIOFormatList *formatList = KImageIOFactory::self()->formatList; 
  KImageIOFormat *format;
  for( format = formatList->first(); format; 
       format = formatList->next() )
  {
     if (format->mMimetype = _mimeType)
     {
        if (((_mode == Reading) && format->bRead) ||
            ((_mode == Writing) && format->bWrite))
           return true; 
     }
  }

  return false;
}

QString KImageIO::mimeType( const QString& _filename )
{
  return KMimeType::findByURL( _filename )->name();
}
