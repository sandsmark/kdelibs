/*
	A dummy source file for documenting the library.
	Michael Koch <koch@kde.org>
	$Id$
	Distributed under the LGPL.
*/

/**
* @libdoc The DCOP Desktop COmmunication Protocol library
*
* DCOP is a simple IPC/RPC mechanism built to operate over sockets.
* Either unix domain sockets or tcp/ip sockets are supported. DCOP is
* built on top of the Inter Client Exchange (ICE) protocol, which comes
* standard as a part of X11R6 and later. It also depends on Qt, but
* beyond that it does not require any other libraries. Because of this,
* it is extremely lightweight, enabling it to be linked into all KDE
* applications with low overhead.
* 
* Model:
* 
* The model is simple.  Each application using DCOP is a client.  They
* communicate to each other through a DCOP server, which functions like
* a traffic director, dispatching messages/calls to the proper
* destinations.  All clients are peers of each other.
* 
* Two types of actions are possible with DCOP: "send and forget"
* messages, which do not block, and "calls," which block waiting for
* some data to be returned.
* 
* Any data that will be sent is serialized (marshalled, for you CORBA
* types) using the built-in QDataStream operators available in all of the
* Qt classes.  This is fast and easy.  Currently, there is no
* type checking or parameter checking available for RPC, but this may be
* provided at some time in the future in the form of a simple IDL-like
* compiler (NOTE: 5 days later the IDL compiler is already started; 
* look in dcopidl/).  Until that is available, you will have to code
* some things by hand that normally the compiler or CORBA take care of
* automatically, but it is not a lot of work.
* 
* Establishing the Connection:
* 
* KApplication has gained a method called "KApplication::dcopClient()"
* which returns a pointer to a DCOPClient instance.  The first time this
* method is called, the client class will be created.  DCOPClients have
* unique identifiers attached to them which are based on what
* KApplication::name() returns.  In fact, if there is only a single
* instance of the program running, the appId will be equal to
* KApplication::name().
* 
* To actually enable DCOP communication to begin, you must use
* DCOPClient::attach().  This will attempt to attach to the DCOP server.
* If no server is found or there is any other type of error, attach()
* will return false.  Applications which are DCOP-enabled should
* probably do something like this at startup time:
*
* <pre> 
* client = kApp->dcopClient();
* if (!client->attach()) {
*   QMessageBox::error(this, i18n("Error connecting to DCOP server"),
*                      i18n("There was an error connecting to the Desktop\n"
*                           "communications server.  Please make sure that\n"
*                           "the 'dcopserver' process has been started, and\n"
*                           "then try again.\n"));
*   exit(1);
* }
* </pre>
* 
* After connecting with the server via @ref DCOPClient::attach, you need to
* register this appId with the server so it knows about you.  Otherwise,
* you are communicating anonymously.  Use the
* DCOPClient::registerAs(const QCString &name) to do so.  In the simple
* case:
* 
* <pre>
* // returns the appId that is actually registered, which _may_ be
* // different from what you passed
* 
* appId = client->registerAs(kApp->name());
* </pre>
* 
* If you never retrieve the DCOPClient pointer from KApplication, the
* object will not be created and thus there will be no memory overhead.
* 
* You may also detach from the server by calling DCOPClient::detach().
* If you wish to attach again you will need to re-register as well.  If
* you only wish to change the ID under which you are registered, simply
* call DCOPClient::registerAs() with the new name.
* 
* Sending Data to a Remote Application:
* 
* To actually communicate, you have one of two choices.  You may either
* call the "send" or the "call" method.  Both methods require three
* identification parameters: an application identifier, a remote object,
* a remote function. Sending is asynchronous (i.e. it returns immediately)
* and may or may not result in your own application being sent a message at
* some point in the future. Then "send" requires one and "call" requires 
* two data parameters.
* 
* The remote object must be specified as an object hierarchy.  That is,
* if the toplevel object is called "fooObject" and has the child
* "barObject", you would reference this object as "fooObject/barObject".
* Functions must be described by a full function signature.  If the
* remote function is called "doIt", and it takes an int, it would be
* described as "doIt(int)".  Please note that the return type is not
* specified here, as it is not part of the function signature (or at
* least the C++ understanding of a function signature).  You will get
* the return type of a function back as an extra parameter to
* DCOPClient::call().  See the section on call() for more details.
* 
* In order to actually get the data to the remote client, it must be
* "serialized" via a QDataStream 
*/
