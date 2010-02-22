SUBDIRS = src

icondir = ./
icon_DATA = pileofblocks-logo.png

## We dist autogen.sh since this is an example program
## Real-world programs do not need to distribute autogen.sh
EXTRA_DIST = config.rpath m4/ChangeLog  \
	pileofblocks.desktop 

dist-hook:
	mkdir $(distdir)/usr/
	mkdir $(distdir)/usr/games
	mkdir $(distdir)/usr/local
	mkdir $(distdir)/usr/local/games
	mkdir $(distdir)/usr/local/games/pileofblocks
	mkdir $(distdir)/usr/local/games/pileofblocks/images
	mkdir $(distdir)/usr/local/games/pileofblocks/menus
	mkdir $(distdir)/usr/local/games/pileofblocks/music
	mkdir $(distdir)/usr/local/games/pileofblocks/sounds
	mkdir $(distdir)/var
	mkdir $(distdir)/var/games
	mkdir $(distdir)/var/games/pileofblocks
	cp -a $(srcdir)/src/pileofblocks $(distdir)/usr/games/
	cp -a $(srcdir)/bin/usr/local/games/pileofblocks/images/* $(distdir)/usr/local/games/pileofblocks/images/
	cp -a $(srcdir)/bin/usr/local/games/pileofblocks/menus/* $(distdir)/usr/local/games/pileofblocks/menus/
	cp -a $(srcdir)/bin/usr/local/games/pileofblocks/music/* $(distdir)/usr/local/games/pileofblocks/music/
	cp -a $(srcdir)/bin/usr/local/games/pileofblocks/sounds/* $(distdir)/usr/local/games/pileofblocks/sounds/
	cp -a $(srcdir)/bin/var/games/pileofblocks/* $(distdir)/var/games/pileofblocks

	rm -rf `find $(distdir) -name .svn`

#copy to staging for deb creation
	rm -rf staging/usr
	rm -rf staging/var
	mkdir staging/usr
	cp -a  $(distdir)/usr staging/
	mkdir staging/var
	cp -a $(distdir)/var staging/

	



Applicationsdir = $(datadir)/gnome/apps/Applications
Applications_DATA = pileofblocks.desktop

ACLOCAL_AMFLAGS = -I m4
