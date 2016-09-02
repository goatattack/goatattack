AC_INIT([goatattack], [0.4.4-dev], [pw@earthwave.ch])

AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_LANG(C++)
AC_LANG(C)

AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_RANLIB

AC_CONFIG_MACRO_DIR([m4])

CXX_FLAGS="${CXX_FLAGS} -pedantic"

AC_HEADER_STDC

AC_CHECK_LIB([pthread], [pthread_create], [], [missing_libraries="$missing_libraries libpthread"])
AC_CHECK_LIB([rt], [clock_gettime], [], [missing_libraries="$missing_libraries librt"])
AC_CHECK_LIB([png], [png_init_io], [], [missing_libraries="$missing_libraries libpng"])

# --- compile args ---
AC_ARG_ENABLE([dedicated-server], [  --enable-dedicated-server  Build dedicated server [[default=no]]], [DEDICATED_SERVER="yes"], [])
AC_ARG_ENABLE([master-server], [  --enable-master-server  Build master server [[default=no]]], [MASTER_SERVER="yes"], [])
AC_ARG_ENABLE([internal-zlib], [  --enable-internal-zlib  Build with the project copy of zlib [[default=no]]], [INTERNAL_ZLIB="yes"], [])
AC_ARG_ENABLE([map-editor], [  --enable-map-editor     Build map editor [[default=no]]], [MAP_EDITOR="yes"], [])
AM_CONDITIONAL([DEDICATED_SERVER], [test "x$DEDICATED_SERVER" = xyes])
AM_CONDITIONAL([MASTER_SERVER], [test "x$MASTER_SERVER" = xyes])
AM_CONDITIONAL([INTERNAL_ZLIB], [test "x$INTERNAL_ZLIB" = xyes])
AM_CONDITIONAL([MAP_EDITOR], [test "x$MAP_EDITOR" = xyes])

# --- check additional libraries ---
if test "x$DEDICATED_SERVER" != "xyes"; then
	AC_CHECK_LIB([SDL2], [SDL_Init], [], [missing_libraries="$missing_libraries libSDL2"])
	AC_CHECK_LIB([GL], [glClear], [], [missing_libraries="$missing_libraries libGL"])
	AC_CHECK_LIB([SDL2_mixer], [Mix_Init], [], [missing_libraries="$missing_libraries libSDL2_mixer"])
fi

# --- check zlib ---
if test "x$INTERNAL_ZLIB" != "xyes"; then
	AC_CHECK_LIB([z], [gzread], [], [missing_libraries="$missing_libraires libz (you can use --enable-internal-zlib)"])
fi

# --- bail out on errors ---
if test ! -z "$missing_libraries"; then
	AC_MSG_ERROR([missing libraries: $missing_libraries])
fi

if test ! -z "$missing_headers"; then
	AC_MSG_ERROR([missing header files: $missing_headers])
fi

# --- ready ---
AC_CONFIG_FILES([Makefile src/shared/Makefile src/Cargo/Makefile src/Engine/Makefile])
if test "x$MAP_EDITOR" = "xyes"; then
	if test "x$DEDICATED_SERVER" != "xyes"; then
		AC_CONFIG_FILES([src/MapEditor/Makefile])
	else
		AC_MSG_ERROR([the map editor cannot be built in a dedicated environment])
	fi
fi
if test "x$MASTER_SERVER" = "xyes"; then
	AC_CONFIG_FILES([src/MasterServer/Makefile])
fi
AC_CONFIG_FILES([data/Makefile])
AC_OUTPUT

# --- summary ---
AC_MSG_NOTICE([])
AC_MSG_NOTICE([--- SUMMARY ---])
if test "x$DEDICATED_SERVER" = "xyes"; then
    AC_MSG_NOTICE([ * goat attack dedicated server build:])
	AC_MSG_NOTICE([   -> game engine (dedicated)])
else
    AC_MSG_NOTICE([ * goat attack normal build:])
	AC_MSG_NOTICE([   -> game engine])
	if test "x$MAP_EDITOR" = "xyes"; then
		AC_MSG_NOTICE([   -> map editor])
	fi
fi
if test "x$MASTER_SERVER" = "xyes"; then
	AC_MSG_NOTICE([   -> master server])
fi
if test "x$INTERNAL_ZLIB" = "xyes"; then
	AC_MSG_NOTICE([])
	AC_MSG_NOTICE([ * taking project copy of zlib])
fi
AC_MSG_NOTICE([])
