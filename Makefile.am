AUTOMAKE_OPTIONS = gnu
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src/Engine
LDADD = -lpthread -lrt
if MAP_EDITOR
SUBDIRS += src/MapEditor
endif
if MASTER_SERVER
SUBDIRS += src/MasterServer
endif
SUBDIRS += data
man6_MANS = man/goatattack.6
EXTRA_DIST = src/Engine/include/* src/MapEditor/include/* src/MasterServer/include/* src/shared/include/* src/shared/zlib/*.h data/* man
