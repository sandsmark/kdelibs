
data_DATA = x-dvi.desktop x-tex-gf.desktop x-gzip.desktop \
	x-object.desktop x-tex-pk.desktop pdf.desktop \
	x-tar.desktop x-trash.desktop x-zip.desktop \
	x-core.desktop x-font.desktop x-java.desktop \
	x-jar.desktop x-arj.desktop x-tarz.desktop \
	postscript.desktop octet-stream.desktop \
	x-shellscript.desktop x-desktop.desktop \
	x-executable.desktop x-archive.desktop x-lha.desktop \
	x-tbz.desktop x-tgz.desktop x-awk.desktop \
	x-perl.desktop x-compress.desktop x-cpio.desktop \
	x-rpm.desktop msword.desktop msword2.desktop x-sharedlib.desktop \
	wordperfect.desktop x-troff.desktop x-troff-man.desktop \
	x-python.desktop x-python-bytecode.desktop \
	x-bzip.desktop x-bzip2.desktop mspowerpoint.desktop \
	msexcel.desktop smil.desktop x-rar.desktop x-zoo.desktop \
	x-tzo.desktop x-lzop.desktop \
	x-applixword.desktop x-applixspread.desktop x-applixgraphics.desktop \
	x-truetype-font.desktop msexcel2.desktop \
	x-shockwave-flash.desktop x-designer.desktop x-linguist.desktop \
	x-ogg.desktop

datadir = $(kde_mimedir)/application

install-data-local: uninstall.desktop
	$(mkinstalldirs) $(datadir)
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(datadir)/x-applix.desktop

EXTRA_DIST = $(data_DATA)
