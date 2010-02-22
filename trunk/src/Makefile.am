#SUBDIRS = guichan xmlguichan
SUBDIRS = xmlguichan

INCLUDES = -I$(top_srcdir) -I$(includedir)  -I$(oldincludedir)/SDL

bin_PROGRAMS = pileofblocks

pileofblocks_SOURCES =         \
        main.cpp           \
        SceneNode.cpp         \
        SceneGraph.cpp 


#pileofblocks_DEPENDENCIES = \
#	guichan/libguichan.la\
#	xmlguichan/libxmlguichan.la


pileofblocks_DEPENDENCIES = \
	xmlguichan/libxmlguichan.la

#-lguichan -lguichan_sdl -lguichan_opengl

pileofblocks_LDADD =$(LIBOBJS) -lGL -lGLU -lSDL_image -lboost_signals -lboost_program_options \
	-lSDL -lSDL_mixer -lSDL_ttf -lsqlite3 -lguichan -lguichan_sdl -lguichan_opengl xmlguichan/.libs/libxmlguichan.a
