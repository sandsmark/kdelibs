CUPSSUBDIR = @CUPSSUBDIR@
LIB_KDEPRINT_CUPS = @LIB_KDEPRINT_CUPS@
LIBVERSION = 1:0

SUBDIRS = $(CUPSSUBDIR) pdq . standalone pics

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/kdeprint/cups -I$(top_srcdir)/kdeprint/pdq -I$(top_srcdir)/kfile -I$(top_srcdir)/kio $(all_includes)

lib_LTLIBRARIES = libkdeprint.la
libkdeprint_la_SOURCES = kdialogimpl.cpp kprintcopiespage.cpp kprintdialog.cpp kprinter.cpp kprinteritem.cpp \
			 optionset.cpp kprintdialogbase.ui kprintcopiespagebase.ui kprintdialogpage.cpp \
			 kprinterpropertydialog.cpp kprintsizecolorpagebase.ui kprintsizecolorpage.cpp \
			 klpddialogimpl.cpp klpdprinter.cpp previewshell.cpp kjoblister.cpp
libkdeprint_la_METASOURCES = AUTO
libkdeprint_la_LDFLAGS = -version-info $(LIBVERSION) $(all_libraries)
libkdeprint_la_LIBADD = $(LIB_KDEPRINT_CUPS) $(top_builddir)/kdeprint/pdq/libkdeprint_pdq.la $(LIB_KPARTS) $(LIB_KFILE)

noinst_HEADERS = kprintcopiespage.h kprintdialog.h \
		 kprinterpropertydialog.h kprintsizecolorpage.h klpddialogimpl.h klpdprinter.h \
		 previewshell.h
include_HEADERS = kprinter.h


kdeprint_HEADERS = optionset.h kjob.h kjoblister.h kprinteritem.h kdialogimpl.h kprintdialogpage.h
kdeprintdir = $(includedir)/kdeprint

uidata_DATA = previewshellui.rc
uidatadir = $(kde_confdir)/ui

rcdata_DATA = kdeprintrc
rcdatadir = $(kde_confdir)

UIFILES = kprintdialogbase.ui kprintcopiespagebase.ui kprintsizecolorpagebase.ui
HFILES = $(UIFILES:.ui=.h)

all-recursive: $(HFILES)