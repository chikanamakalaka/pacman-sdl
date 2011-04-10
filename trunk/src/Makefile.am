#SUBDIRS = guichan xmlguichan
SUBDIRS = xmlguichan

INCLUDES = -I$(top_srcdir) -I$(includedir)  -I$(oldincludedir)/SDL

bin_PROGRAMS = pacman

AM_CXXFLAGS = -pg

pacman_SOURCES =         \
        main.cpp           \
        SceneNode.cpp         \
        SceneGraph.cpp 


#pacman_DEPENDENCIES = \
#	guichan/libguichan.la\
#	xmlguichan/libxmlguichan.la


pacman_DEPENDENCIES = \
	xmlguichan/libxmlguichan.la

#-lguichan -lguichan_sdl -lguichan_opengl

pacman_LDADD =$(LIBOBJS) -lGL -lGLU -lSDL_image -lboost_signals -lboost_program_options \
	-lSDL -lSDL_mixer -lSDL_ttf -lsqlite3 -lguichan -lguichan_sdl -lguichan_opengl xmlguichan/.libs/libxmlguichan.a
