#ifndef __kservices_h__
#define __kservices_h__

#include <list>

#include <qlist.h>
#include <qstrlist.h>

#include <ksimpleconfig.h>

class KServiceTypeProfile;

/** Represents a service, i.e. an application bound to one or several mimetypes
 * as written in its kdelnk file.
 *
 * IMPORTANT : to use the public static methods of this class, you must do 
 * the following registry initialisation (in main() for instance)
 * <pre>
 * #include <kregistry.h>
 * #include <kregfactories.h> 
 *
 *   KRegistry registry;
 *   registry.addFactory( new KServiceFactory );
 *   registry.load();
 * </pre>
 */
class KService
{
public:
  struct Offer
  {
    KService* m_pService;

    bool allowAsDefault();
    int preference();
    
    ///////
    // For internal use only. These variables are needed for sorting
    ///////
    QString m_strServiceType;
    KServiceTypeProfile *m_pServiceTypeProfile;
    
    bool operator< ( Offer& _o );
  };
  
  KService( const char *_name, const char *_exec, const char *_icon,
	    const QStrList& _lstServiceTypes, const char *_comment = 0L,
	    bool _allow_as_default = true, const char *_path = 0L,
	    const char *_terminal = 0L, const char *_file = 0L, 
	    bool _put_in_list = true );
  ~KService();
  
  const char* name() { return m_strName; }
  const char* exec() { return m_strExec; }
  const char* icon() { return m_strIcon; }
  const char* terminalOptions() { return m_strTerminalOptions; }
  const char* path() { return m_strPath; }
  const char* comment() { return m_strComment; }
  const char* file() { return m_strFile; };
  QStrList& serviceTypes();
  bool hasServiceType( const char *_service );
  bool allowAsDefault() { return m_bAllowAsDefault; }
  
  /**
   * @param _result is filled with all matching offers. The list is sorted accrodingly
   *        to the users profile ( @ref KServiceTypeProfile ).
   */
  static void findServiceByServiceType( const char* _servicetype, list<Offer>& _result );

  static KService* findByName( const char *_name );
  
  /**
   * @param _file is only used while displaying error messages.
   */
  static KService* parseService( const char *_file, KSimpleConfig &config, bool _put_in_list = true );

  /**
   * Return the whole list of services. Useful to display them.
   */
  static QList <KService>* allServices() { return s_lstServices; }
  
protected:
  static void initServices( const char * _path );
  static void initStatic();

  QString m_strName;
  QString m_strExec;
  QString m_strIcon;
  QString m_strTerminalOptions;
  QString m_strPath;
  QString m_strComment;
  QString m_strFile;
  QStrList m_lstServiceTypes;
  bool m_bAllowAsDefault;
  
  static QList<KService>* s_lstServices;
};

#endif
