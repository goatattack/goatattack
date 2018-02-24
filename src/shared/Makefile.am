noinst_LIBRARIES = libshared.a
libshared_a_SOURCES = ../shared/src/AABB.cpp ../shared/src/Animation.cpp ../shared/src/Audio.cpp ../shared/src/AudioNull.cpp ../shared/src/AudioSDL.cpp ../shared/src/Background.cpp ../shared/src/Characterset.cpp ../shared/src/Configuration.cpp ../shared/src/Directory.cpp ../shared/src/Exception.cpp ../shared/src/Font.cpp ../shared/src/Globals.cpp ../shared/src/Gui.cpp ../shared/src/GuiObject.cpp ../shared/src/Icon.cpp ../shared/src/KeyValue.cpp ../shared/src/Lightmap.cpp ../shared/src/Map.cpp ../shared/src/Movable.cpp ../shared/src/Music.cpp ../shared/src/Mutex.cpp ../shared/src/NPC.cpp ../shared/src/Object.cpp ../shared/src/PNG.cpp ../shared/src/Properties.cpp ../shared/src/Resources.cpp ../shared/src/Sound.cpp ../shared/src/Subsystem.cpp ../shared/src/SubsystemNull.cpp ../shared/src/SubsystemSDL.cpp ../shared/src/TCPSocket.cpp ../shared/src/Thread.cpp ../shared/src/Tile.cpp ../shared/src/TileGraphic.cpp ../shared/src/TileGraphicGL.cpp ../shared/src/TileGraphicNull.cpp ../shared/src/Tileset.cpp ../shared/src/Timing.cpp ../shared/src/UDPSocket.cpp ../shared/src/Utils.cpp ../shared/src/Zip.cpp ../shared/src/ZipReader.cpp ../shared/src/ZipWriter.cpp ../shared/src/CRC64.cpp ../shared/src/TextMessageSystem.cpp ../shared/src/FileReader.cpp ../shared/src/Cargo.cpp ../shared/src/MultiReader.cpp ../shared/src/Shader.cpp ../shared/src/ShaderNull.cpp ../shared/src/ShaderGL.cpp ../shared/src/I18N.cpp ../shared/src/PathManager.cpp

libshared_a_SOURCES += ../shared/i18n/I18NEnglish.cpp ../shared/i18n/I18NGerman.cpp ../shared/i18n/I18NFrench.cpp ../shared/i18n/I18NPortuguese.cpp ../shared/i18n/I18NSpanish.cpp

libshared_a_CXXFLAGS = -I./include -I../shared/include -pedantic -Wextra -Wno-unused-parameter -Wno-unused-result -DPNG_SKIP_SETJMP_CHECK

if DEDICATED_SERVER
libshared_a_CXXFLAGS += -DDEDICATED_SERVER
else
libshared_a_CXXFLAGS += -DGL_GLEXT_PROTOTYPES
endif

if INTERNAL_ZLIB
libshared_a_SOURCES += ../shared/zlib/adler32.c ../shared/zlib/crc32.c ../shared/zlib/deflate.c ../shared/zlib/inffast.c ../shared/zlib/inflate.c ../shared/zlib/inftrees.c ../shared/zlib/trees.c ../shared/zlib/zutil.c
libshared_a_CXXFLAGS += -I../shared/zlib
endif

libshared_a_CXXFLAGS += -D__STDC_FORMAT_MACROS
