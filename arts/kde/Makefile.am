INCLUDES                    =   $(all_includes) -I$(top_srcdir)/arts \
			        -I$(top_srcdir)/arts/mcop -I$(top_srcdir)/arts/flow \
				-I$(top_builddir)/arts/flow -I$(top_srcdir)/arts/soundserver \
				-I$(top_builddir)/arts/soundserver -I$(top_builddir)/arts/kde/lib \
				-I$(top_srcdir)/kio
lib_LTLIBRARIES  	    =	libartskde.la
libartskde_la_SOURCES	    =	artskde.cc kioinputstream_impl.cpp kplayobject.cc \
				kplayobjectfactory.cc kartsfloatwatch.cpp
libartskde_la_LIBADD        =   $(LIB_KDECORE) $(LIB_KIO) $(LIB_KSYCOCA)
libartskde_la_LDFLAGS       =   -avoid-version -module $(all_libraries)
libartskde_la_METASOURCES   =	AUTO

artskdeincludedir	    =	$(includedir)/arts
artskdeinclude_HEADERS      = 	kplayobject.h kplayobjectfactory.h kartsfloatwatch.h

kartsfloatwatch.cpp: artskde.h
artskde.cc: artskde.h
artskde.mcoptype: artskde.h
artskde.mcopclass: artskde.h
artskde.h: $(top_srcdir)/arts/kde/artskde.idl $(MCOPIDL)
	    $(MCOPIDL) -I$(top_srcdir)/arts/flow -I$(top_srcdir)/arts/soundserver -t $(MCOPINC) $(top_srcdir)/arts/kde/artskde.idl
