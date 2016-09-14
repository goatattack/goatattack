AUTOMAKE_OPTIONS = gnu
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src/shared src/Cargo
LDADD = -lpthread -lrt
if DEDICATED_SERVER
SUBDIRS += src/Dedicated
else
SUBDIRS += src/Game
endif
if MAP_EDITOR
SUBDIRS += src/MapEditor
endif
if MASTER_SERVER
SUBDIRS += src/MasterServer
endif
SUBDIRS += data
EXTRA_DIST = src/Cargo/include/* src/Engine/include/* src/MapEditor/include/* src/MasterServer/include/* src/shared/include/* src/shared/zlib/*.h data/*

no-data-clean:
	cd data && make save && cd ..
	make clean
	cd data && make restore && cd ..

