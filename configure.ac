AC_INIT([goatattack], [0.4.5-dev], [pw@earthwave.ch])

AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_LANG(C++)
AC_LANG(C)

AC_PROG_CXX
AM_PROG_CC_C_O

AC_CONFIG_MACRO_DIR([m4])

AC_HEADER_STDC
AM_SILENT_RULES([yes])

# --- append freetype2 standard include path ---
CPPFLAGS="$CPPFLAGS -I/usr/include/freetype2"

# --- check headers ---
AC_CHECK_HEADERS([ft2build.h], [], [missing_headers="$missing_headers ft2build.h"])

# --- check libraries ---
AC_CHECK_LIB([pthread], [pthread_create], [], [missing_libraries="$missing_libraries libpthread"])
AC_CHECK_LIB([rt], [clock_gettime], [], [missing_libraries="$missing_libraries librt"])
AC_CHECK_LIB([png], [png_init_io], [], [missing_libraries="$missing_libraries libpng"])
AC_CHECK_LIB(freetype, FT_Init_FreeType, [], [missing_libraries="$missing_libraries libfreetype"])

# --- compile args ---
AC_ARG_ENABLE([build-pak],        [  --disable-build-pak     Disable building base.pak [[default=no]]], [], [BUILD_PAK="yes"])
AC_ARG_ENABLE([non-free-pak],     [  --enable-non-free-pak   Build non free pak [[default=no]]], [NON_FREE_PAK="yes"], [])
AC_ARG_ENABLE([dedicated-server], [  --enable-dedicated-server  Build dedicated server [[default=no]]], [DEDICATED_SERVER="yes"], [])
AC_ARG_ENABLE([master-server],    [  --enable-master-server  Build master server [[default=no]]], [MASTER_SERVER="yes"], [])
AC_ARG_ENABLE([internal-zlib],    [  --enable-internal-zlib  Build with the project copy of zlib [[default=no]]], [INTERNAL_ZLIB="yes"], [])
AC_ARG_ENABLE([map-editor],       [  --enable-map-editor     Build map editor [[default=no]]], [MAP_EDITOR="yes"], [])
AC_ARG_ENABLE([cargo-only],       [  --enable-cargo-only     Build cargo tool only [[default=no]]], [CARGO_ONLY="yes"], [])
AC_ARG_ENABLE([install-cargo],    [  --enable-install-cargo  Installing cargo [[default=no]]], [INSTALL_CARGO="yes"], [])
AC_ARG_ENABLE([safe-alignment],   [  --enable-safe-alignment Copy memory to aligned location [[default=no]]], [SAFE_ALIGNMENT="yes"], [])
AC_ARG_WITH([targetdatadir], [AS_HELP_STRING([--with-targetdatadir=DIR], [data directory on the target])], [targetdatadir=$withval], [targetdatadir="\${pkgdatadir}"])
AC_SUBST([targetdatadir], [$targetdatadir])

AM_CONDITIONAL([DEDICATED_SERVER], [test "x$DEDICATED_SERVER" = xyes])
AM_CONDITIONAL([MASTER_SERVER], [test "x$MASTER_SERVER" = xyes])
AM_CONDITIONAL([INTERNAL_ZLIB], [test "x$INTERNAL_ZLIB" = xyes])
AM_CONDITIONAL([MAP_EDITOR], [test "x$MAP_EDITOR" = xyes])
AM_CONDITIONAL([BUILD_PAK], [test "x$BUILD_PAK" = xyes])
AM_CONDITIONAL([NON_FREE_PAK], [test "x$NON_FREE_PAK" = xyes])
AM_CONDITIONAL([SAFE_ALIGNMENT], [test "x$SAFE_ALIGNMENT" = xyes])

# --- cargo ---
AM_CONDITIONAL([CARGO_ONLY], [test "x$CARGO_ONLY" = xyes])
AM_CONDITIONAL([INSTALL_CARGO], [test "x$INSTALL_CARGO" = xyes])

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
AC_CONFIG_FILES([Makefile src/shared/Makefile src/Cargo/Makefile])
if test "x$DEDICATED_SERVER" != "xyes"; then
	AC_CONFIG_FILES([src/Game/Makefile])
else
	AC_CONFIG_FILES([src/Server/Makefile])
fi
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
if test "x$CARGO_ONLY" = "xyes"; then
	AC_MSG_NOTICE([ * goat attack cargo build:])
	AC_MSG_NOTICE([   -> only cargo tool will be compiled])
else
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
fi

AC_MSG_NOTICE([])
if test "x$INTERNAL_ZLIB" = "xyes"; then
	AC_MSG_NOTICE([ * taking project copy of zlib])
fi
if test "x$SAFE_ALIGNMENT" = "xyes"; then
	AC_MSG_NOTICE([ * safe alignment enabled])
fi
if test "x$BUILD_PAK" != "xyes"; then
	AC_MSG_NOTICE([ * base.pak will not be created])
else
	if test "x$NON_FREE_PAK" == "xyes"; then
		AC_MSG_NOTICE([ * base-non-free.pak will be created])
	else
		AC_MSG_NOTICE([ * base-non-free.pak will NOT be created])
	fi
fi
if test "x$INSTALL_CARGO" != "xyes"; then
	AC_MSG_NOTICE([ * cargo will not be installed])
fi
AC_MSG_NOTICE([])
