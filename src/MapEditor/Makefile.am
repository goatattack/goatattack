if !CARGO_ONLY
bin_PROGRAMS = goatattack-mapeditor
goatattack_mapeditor_SOURCES = main.cpp
goatattack_mapeditor_SOURCES += src/CompileThreadBlock.cpp src/CompileThread.cpp src/CompileThreadPixel.cpp src/EditableLight.cpp src/EditableMap.cpp src/EditableObject.cpp src/MapEditor.cpp src/MapEditorLightmap.cpp

goatattack_mapeditor_CXXFLAGS = -I./include -I../shared/include -pedantic -Wextra -Wno-unused-parameter -Wno-unused-result
goatattack_mapeditor_CXXFLAGS += -DDATA_DIRECTORY=$(targetdatadir) -DPNG_SKIP_SETJMP_CHECK
goatattack_mapeditor_LDADD = -L../shared -lshared -lpng -lGL -lSDL2 -lSDL2_mixer -lfreetype

if INTERNAL_ZLIB
goatattack_mapeditor_SOURCES += ../shared/zlib/adler32.c ../shared/zlib/crc32.c ../shared/zlib/inffast.c ../shared/zlib/inflate.c ../shared/zlib/inftrees.c ../shared/zlib/zutil.c
goatattack_mapeditor_CXXFLAGS += -I../shared/zlib
else
goatattack_mapeditor_LDADD += -lz
endif

iconsdir = $(datarootdir)/icons/hicolor/scalable/apps
dist_icons_DATA = ../../packages/linux/goatattack-mapeditor.svg
desktopdir = $(datarootdir)/applications
dist_desktop_DATA = ../../packages/linux/goatattack-mapeditor.desktop
appdatadir = $(datarootdir)/appdata
dist_appdata_DATA = ../../packages/linux/goatattack-mapeditor.appdata.xml
man6_MANS = man/goatattack-mapeditor.6
endif
