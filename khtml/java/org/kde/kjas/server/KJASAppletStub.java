package org.kde.kjas.server;

import java.applet.*;
import java.util.*;
import java.net.*;

/**
 * The stub used by Applets to communicate with their environment.
 *
 * <H3>Change Log</H3>
 * <PRE>
 * $Log$
 * Revision 1.4  2000/09/16 15:03:41  rogozin
 * Fixed #9920.
 * It still doesn't display URLS, but the applet is loading.
 *
 * Revision 1.3  2000/03/22 05:19:38  rogozin
 *
 * Window geometry is now handled correctly.
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
public class KJASAppletStub implements AppletStub
{
    //** The containing context.
    KJASAppletContext context;

    //** The applet this stub is attached to.
    Applet applet;

    //** Maps parameter names to values
    Hashtable params;

    //** The URL of the class file from which applet was loaded.
    URL codeBase;

    //** The document that referenced the applet
    URL docBase;

    //** Is the applet active?
    boolean active;

    //** The name of this applet instance
    String name;

    //** The id of this applet- for use in callbacks
    String appletID;

    /**
     * Create an AppletStub for the specified applet. The stub will be in
     * the specified context and will automatically attach itself to the
     * passed applet.
     */
    public KJASAppletStub( KJASAppletContext _context, String _appletID,
                           Applet _applet, URL _codeBase, URL _docBase, String _name )
    {
        context  = _context;
        applet   = _applet;
        codeBase = _codeBase;
        docBase  = _docBase;
        name     = _name;
        appletID = _appletID;

        params = new Hashtable();
        applet.setStub( this );
    }

   public Applet getApplet()
   {
      return applet;
   }

    public void setParameter( String name, String value )
    {
        name = name.toLowerCase();

        if ( ( name != null ) && ( value != null ) )
            params.put( name, value );
    }

    /**
     * Called by the Applet when it wants to resize itself.
     */
    public void appletResize( int width, int height )
    {
        if ( (width > 0) && (height > 0))
        {
            Main.protocol.sendResizeAppletCmd( context.getID(), appletID, width, height );
        }
        else
            System.err.println( "Warning: applet attempted to resize itself to " + width + "," + height );
    }

    /**
     * Returns the context that created this AppletStub.
     */
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

    /**
     * Get the value of the named parameter.
     */
    public String getParameter( String name )
    {
        return (String) params.get( name.toLowerCase() );
    }

    public boolean isActive()
    {
        return active;
    }
}
