AUTOMAKE_OPTIONS = gnu
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src/shared src/Engine
LDADD = -lpthread -lrt
if MAP_EDITOR
SUBDIRS += src/MapEditor
endif
if MASTER_SERVER
SUBDIRS += src/MasterServer
endif
SUBDIRS += src/Cargo
SUBDIRS += data
EXTRA_DIST = src/Cargo/include/* src/Engine/include/* src/MapEditor/include/* src/MasterServer/include/* src/shared/include/* src/shared/zlib/*.h data/* man

no-data-clean:
	cd data && make save && cd ..
	make clean
	cd data && make restore && cd ..

