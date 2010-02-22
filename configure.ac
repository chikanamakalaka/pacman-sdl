#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lSDL_image':
AC_CHECK_LIB([SDL_image], [main])
# FIXME: Replace `main' with a function in `-lguichan':
AC_CHECK_LIB([guichan], [main])
# FIXME: Replace `main' with a function in `-lguichan_sdl':
AC_CHECK_LIB([guichan_sdl], [main])
AC_CHECK_LIB([boost_signals], [main])
AC_CHECK_LIB([boost_program_options], [main])
AC_CHECK_LIB([GL], [main])
AC_CHECK_LIB([SDL], [main])
AC_CHECK_LIB([SDL_mixer], [main])
AC_CHECK_LIB([SDL_ttf], [main])
AC_CHECK_LIB([guichan_opengl], [main])
AC_CHECK_LIB([sqlite3], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stddef.h stdint.h stdlib.h string.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/time.h unistd.h SDL/SDL.h SDL/SDL_mixer.h SDL/SDL_ttf.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime fdatasync ftruncate getcwd gettimeofday localtime_r memmove memset mkdir pow strchr strerror strstr])

AC_CONFIG_FILES([Makefile po/Makefile.in
                 src/Makefile
                 src/guichan/Makefile
                 src/guichan/opengl/Makefile
                 src/guichan/sdl/Makefile
                 src/guichan/widgets/Makefile
                 src/xmlguichan/Makefile])
m4_pattern_allow([^AC_PROG_LIBTOOL$]) 
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AM_INIT_AUTOMAKE([pileofblocks], [1.0.0])

AC_OUTPUT
