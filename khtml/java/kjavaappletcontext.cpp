#include <kjavaappletcontext.moc>
#include <kjavaappletserver.h>
#include <kjavaapplet.h>

// For future expansion
struct KJavaAppletContextPrivate
{

};

KJavaAppletContext::KJavaAppletContext( KJavaAppletServer *server )
  : QObject()
{
   if ( server != 0 )
      this->server = server;
   else {
      this->server = KJavaAppletServer::getDefaultServer();
   }

   static int contextIdSource = 0;

   setContextId( contextIdSource );
   this->server->createContext( contextIdSource );

   contextIdSource++;
}

KJavaAppletContext::~KJavaAppletContext()
{
    server->destroyContext( id );
}

KJavaAppletContext *KJavaAppletContext::getDefaultContext()
{
   static KJavaAppletContext *context = 0;

   if ( context == 0 ) {
      context = new KJavaAppletContext();
      CHECK_PTR( context );
   }

   return context;
}

int KJavaAppletContext::contextId()
{
   return id;
}

void KJavaAppletContext::setContextId( int id )
{
   this->id = id;
}

void KJavaAppletContext::create( KJavaApplet *applet )
{
    static int appletId = 0;

    server->createApplet( id, appletId,
			  applet->appletName(),
			  applet->appletClass(),
			  applet->baseURL() );
    applet->setAppletId( appletId );
    appletId++;
}

void KJavaAppletContext::destroy( KJavaApplet *applet )
{
  int appletId = applet->appletId();
  server->destroyApplet( id, appletId );
}

void KJavaAppletContext::setParameter( KJavaApplet *applet,
				       const QString &name, const QString &value )
{
    server->setParameter( id, applet->appletId(),
			  name, value );
}

void KJavaAppletContext::show( KJavaApplet *applet, const QString &title )
{
    server->showApplet( id, applet->appletId(), title );
}

void KJavaAppletContext::start( KJavaApplet *applet )
{
    server->startApplet( id, applet->appletId() );
}

void KJavaAppletContext::stop (KJavaApplet *applet )
{
    server->stopApplet( id, applet->appletId() );
}
