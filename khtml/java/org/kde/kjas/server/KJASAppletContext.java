package org.kde.kjas.server;

import java.applet.*;
import java.util.*;
import java.net.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

/**
 * The context in which applets live.
 *
 * <H3>Change Log</H3>
 * <PRE>
 * $Log$
 * Revision 1.10  2000/09/27 11:46:32  sschiman
 * * I've added implementations for the showDocument and showStatus calls to kjas
 * for java applets that want to change the location of a frame. This should fix
 * java menus. (okayed by Lars and mhk)
 *
 * Revision 1.9  2000/09/18 02:33:25  rogozin
 * Fix #10848 and #5668
 *
 * Revision 1.8  2000/05/21 19:27:28  rogozin
 *
 * Fix reload exception
 *
 * Revision 1.7  2000/03/23 03:20:38  rogozin
 *
 * Fix resize issues for jdk 1.1.8.
 * This is was quite a hack.
 *
 * Revision 1.6  2000/03/22 05:19:38  rogozin
 *
 * Window geometry is now handled correctly.
 *
 * Revision 1.5  2000/01/29 04:22:28  rogozin
 * Preliminary support for archive tag.
 * Fix size problem.
 *
 * Revision 1.4  2000/01/27 23:41:57  rogozin
 * All applet parameters are passed to KJAS now
 * Next step - make use of them.
 *
 * Revision 1.3  1999/11/12 02:58:05  rich
 * Updated KJAS server
 *
 * Revision 1.2  1999/10/09 21:55:27  rich
 * More consts
 *
 * Revision 1.1.1.1  1999/07/22 17:28:08  rich
 * This is a current snapshot of my work on adding Java support
 * to KDE. Applets now work!
 *
 * </PRE>
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 */
public class KJASAppletContext implements AppletContext
{
    //* All the applets in this context
    private Hashtable applets;
    private Hashtable stubList;

    private String contextID;

    private KJASAppletClassLoader loader;


    /**
     * Create a KJASAppletContext. This is shared by all applets (though perhaps
     * there should be one for each web page).
     */
    public KJASAppletContext( String _contextID )
    {
        applets = new Hashtable();
        stubList = new Hashtable();

        contextID = _contextID;
    };

    public String getID()
    {
        return contextID;
    }

    public KJASAppletStub getAppletStub( String appletId )
    {
        KJASAppletStub stb = (KJASAppletStub) stubList.get( appletId );
        if ( stb == null )
            throw new IllegalArgumentException( "Invalid appletId passed to getAppletStub() "
                                                + appletId );
        return stb;
    }

    public KJASAppletStub createApplet( String appletID,
                                        String className, URL codeBase,
                                        URL docBase, String jars,
                                        String name, Dimension size )
    {
        try
        {
            if( loader == null )
            {
                loader = KJASAppletClassLoader.createLoader( codeBase );
            }
            else
            {
                loader.addCodeBase( codeBase );
            }

            if( jars != null )
            {
                StringTokenizer parser = new StringTokenizer( jars, ",", false );
                while( parser.hasMoreTokens() )
                {
                    String jar = parser.nextToken().trim();
                    loader.addJar( codeBase, jar );
                }
            }

            Class appletClass = loader.loadClass( className );

            // Load and instantiate applet
            Applet app = (Applet) appletClass.newInstance();
            app.setSize( size );

            KJASAppletStub stub = new KJASAppletStub( this, appletID, app, codeBase, docBase, name );

            applets.put( name, app );
            stubList.put( appletID, stub );

            return stub;
        }
        catch ( ClassNotFoundException e )
        {
            Main.kjas_err( "Could not find the needed class" + e, e );
        }
        catch ( Exception e )
        {
            Main.kjas_err( "Something bad happened: " + e, e );
        }
        return null;
   }

    public void destroy()
    {
        Enumeration e = applets.elements();
        while ( e.hasMoreElements() ) {
            Applet app = (Applet) e.nextElement();
            app.stop();
        }
        applets.clear();
        stubList.clear();
    }

    public void destroyApplet( Applet app )
    {
        app.stop();
    }

    public void show( Applet app, String title )
    {
        if ( applets.contains( app ) )
        {
            Frame f = new Frame( title );
            AppletPanel p = new AppletPanel( app.getSize() );

            p.add("Center", app);
            f.add("Center", p);
            f.pack();

            app.init();
            app.start();

            f.setVisible( true );
        }
    }

    //
    // AppletContext interface
    //
    public Applet getApplet( String name )
    {
        return (Applet) applets.get( name );
    }

    public Enumeration getApplets()
    {
        Enumeration e = applets.elements();
        return e;
    }

    public AudioClip getAudioClip( URL url )
    {
        return null;
    }

    public Image getImage( URL url )
    {
        Toolkit kit = Toolkit.getDefaultToolkit();
        return kit.getImage( url );
    }

    public void showDocument( URL url )
    {
        if( url != null )
            Main.protocol.sendShowDocumentCmd( contextID, url.toString()  );
        else
            System.err.println( "tried showDocument with null url" );
    }

    public void showDocument( URL url, String targetFrame )
    {
        if ( ( url != null ) && ( targetFrame != null ) )
            Main.protocol.sendShowDocumentCmd( contextID, url.toString(), targetFrame );
        else
            System.err.println( "tried showDocument (frames) with invalid params" );
    }

    public void showStatus( String message )
    {
        if( message != null )
            Main.protocol.sendShowStatusCmd( contextID, message );
        else
            System.err.println( "tried showStatus with null message" );
    }

    class AppletPanel
        extends Panel
    {
        Dimension appletSize;

        AppletPanel(Dimension size)
        {
            super(new BorderLayout());
            appletSize = size;
        }

        public Dimension getPreferredSize()
        {
            return appletSize;
        }
    }
}
