package org.kde.kjas.server;

import java.applet.*;
import java.util.*;
import java.net.*;
import java.awt.*;


/**
 * The stub used by Applets to communicate with their environment.
 *
 */
public class KJASAppletStub extends Frame
    implements AppletStub
{
    KJASAppletContext context;    // The containing context.
    Hashtable         params;     // Maps parameter names to values
    URL               codeBase;   // The URL directory where files are
    URL               docBase;    // The document that referenced the applet
    boolean           active;     // Is the applet active?
    String            appletName; // The name of this applet instance
    String            appletID;   // The id of this applet- for use in callbacks
    Dimension         appletSize;
    String            windowName;
    String            className;
    Class             appletClass;
    Thread            classLoadingThread;
    Thread            setupWindowThread;

    KJASAppletClassLoader loader;
    KJASAppletPanel       panel;
    Applet                app;

    /**
     * Create an AppletStub for the specified applet. The stub will be in
     * the specified context and will automatically attach itself to the
     * passed applet.
     */
    public KJASAppletStub( KJASAppletContext _context, String _appletID,
                           URL _codeBase, URL _docBase,
                           String _appletName, String _className,
                           Dimension _appletSize, Hashtable _params,
                           String _windowName, KJASAppletClassLoader _loader )
    {
        super( _windowName );

        context    = _context;
        appletID   = _appletID;
        codeBase   = _codeBase;
        docBase    = _docBase;
        appletName = _appletName;
        className  = _className;
        appletSize = _appletSize;
        params     = _params;

        windowName = _windowName;
        loader     = _loader;
    }

    /*************************************************************************
     *********************** Runnable Interface ******************************
     *************************************************************************/
    public void setupWindow()
    {
        setupWindowThread = new Thread
        (
            new Runnable()
            {
                public void run()
                {
                    setupPanel();
                }
            }
        );

        setupWindowThread.start();
    }

    public void downloadClass()
    {
        classLoadingThread = new Thread
        (
            new Runnable()
            {
                public void run()
                {
                    try
                    {
                        getAppletClass();
                    }
                    catch( ClassNotFoundException e )
                    {
                        Main.kjas_err( "Could not load class", e );
                    }
                }
            }
        );

        classLoadingThread.start();
    }

    public void initApplet()
    {
        new Thread
        (
            new Runnable()
            {
                public void run()
                {
                    createApplet();
                }
            }
        ).start();
    }

    private void setupPanel()
    {
        panel = new KJASAppletPanel( appletSize );
        add( "Center", panel );
        pack();
        show();
    }

    private void getAppletClass()
        throws ClassNotFoundException
    {
        appletClass = loader.loadClass( className );
    }

    private void createApplet()
    {
        while( setupWindowThread.isAlive() || classLoadingThread.isAlive() )
        {
            Main.kjas_debug( "Applet #" + appletID +
                             ": waiting for setup threads to finish, going to sleep" );
            try
            {
                Thread.sleep( 500 );
            } catch( InterruptedException e )
            {
                Main.kjas_debug( "thread could not sleep" );
            }
        }

        try
        {
            Main.kjas_debug( "Applet #" + appletID + ": class is loaded" );
            app = (Applet) appletClass.newInstance();
            app.setStub( this );
            app.resize( appletSize );
            app.setVisible( false ); //hide();
            panel.add( "Center", app );
            panel.validate();

            app.init();
            panel.validate();

            app.resize( appletSize );
            app.start();  //We're already in a thread, so don't need to create a new one
            panel.validate();
            app.setVisible( true ); //show();
        }
        catch( InstantiationException e )
        {
            Main.kjas_err( "Could not instantiate applet", e );
        }
        catch( IllegalAccessException e )
        {
            Main.kjas_err( "Could not instantiate applet", e );
        }
    }

    public void startApplet()
    {
        app.start();
    }

    public void stopApplet()
    {
        app.stop();
    }

    public void die()
    {
        app.stop();
        dispose();
    }

    public Applet getApplet()
    {
        return app;
    }

    public Dimension getAppletSize()
    {
        return appletSize;
    }


    /*************************************************************************
     ********************** AppletStub Interface *****************************
     *************************************************************************/
    public void appletResize( int width, int height )
    {
        if ( (width >= 0) && (height >= 0))
        {
            Main.kjas_debug( "Applet #" + appletID + ": appletResize to : (" + width + ", " + height + ")" );
            Main.protocol.sendResizeAppletCmd( context.getID(), appletID, width, height );
            appletSize = new Dimension( width, height );

            app.resize( appletSize );
            panel.setAppletSize( appletSize );
            pack();
        }
        else
            System.err.println( "Applet #" + appletID + ": applet attempted to resize itself to " + width + "," + height );
    }

    public AppletContext getAppletContext()
    {
        return context;
    }

    public URL getCodeBase()
    {
        return codeBase;
    }

    public URL getDocumentBase()
    {
        return docBase;
    }

    public String getAppletName()
    {
        return appletName;
    }

    public String getParameter( String name )
    {
        return (String) params.get( name.toUpperCase() );
    }

    public boolean isActive()
    {
        return active;
    }

    /*************************************************************************
     ************************* Layout methods ********************************
     *************************************************************************/
    class KJASAppletPanel extends Panel
    {
        private Dimension size;

        public KJASAppletPanel( Dimension _size )
        {
            super( new BorderLayout() );
            size = _size;
        }

        public void setAppletSize( Dimension _size )
        {
            size = _size;
        }

        public Dimension getPreferredSize()
        {
            return size;
        }

        public Dimension getMinimumSize()
        {
            return size;
        }

//        public Dimension preferredSize()
//        {
//            return size;
//        }
//
//        public Dimension minimumSize()
//        {
//            return size;
//        }
    }

}
