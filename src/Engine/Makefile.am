bin_PROGRAMS = goatattack
goatattack_SOURCES = main.cpp
goatattack_SOURCES += src/Client.cpp src/ClientEvent.cpp src/ClientServer.cpp src/Game.cpp src/KeyBinding.cpp src/LANBroadcaster.cpp src/MainMenu.cpp src/MasterQueryClient.cpp src/MasterQuery.cpp src/MessageSequencer.cpp src/OptionsMenu.cpp src/Player.cpp src/Server.cpp src/Tournament.cpp src/TournamentCTF.cpp src/TournamentDM.cpp src/TournamentDraw.cpp src/TournamentFactory.cpp src/TournamentNPC.cpp src/TournamentPhysics.cpp src/TournamentSpectator.cpp src/TournamentStates.cpp src/TournamentTDM.cpp src/TournamentSR.cpp src/TournamentCTC.cpp src/TournamentGOH.cpp src/TournamentTeam.cpp src/TournamentWeapons.cpp src/GameserverInformation.cpp src/ServerLogger.cpp src/ButtonNavigator.cpp src/ServerAdmin.cpp src/MapConfiguration.cpp
goatattack_SOURCES += ../shared/src/AABB.cpp ../shared/src/Animation.cpp ../shared/src/Audio.cpp ../shared/src/AudioNull.cpp ../shared/src/AudioSDL.cpp ../shared/src/Background.cpp ../shared/src/Characterset.cpp ../shared/src/Configuration.cpp ../shared/src/Directory.cpp ../shared/src/Exception.cpp ../shared/src/Font.cpp ../shared/src/Globals.cpp ../shared/src/Gui.cpp ../shared/src/GuiObject.cpp ../shared/src/Icon.cpp ../shared/src/KeyValue.cpp ../shared/src/Lightmap.cpp ../shared/src/Map.cpp ../shared/src/Movable.cpp ../shared/src/Music.cpp ../shared/src/Mutex.cpp ../shared/src/NPC.cpp ../shared/src/Object.cpp ../shared/src/PNG.cpp ../shared/src/Properties.cpp ../shared/src/Resources.cpp ../shared/src/Sound.cpp ../shared/src/Subsystem.cpp ../shared/src/SubsystemNull.cpp ../shared/src/SubsystemSDL.cpp ../shared/src/TCPSocket.cpp ../shared/src/Thread.cpp ../shared/src/Tile.cpp ../shared/src/TileGraphic.cpp ../shared/src/TileGraphicGL.cpp ../shared/src/TileGraphicNull.cpp ../shared/src/Tileset.cpp ../shared/src/Timing.cpp ../shared/src/UDPSocket.cpp ../shared/src/Utils.cpp ../shared/src/Zip.cpp ../shared/src/ZipReader.cpp ../shared/src/ZipWriter.cpp ../shared/src/CRC64.cpp ../shared/src/TextMessageSystem.cpp

goatattack_CXXFLAGS = -I./include -I../shared/include -Wno-unused-result

goatattack_LDADD = -lpng

if INTERNAL_ZLIB
goatattack_SOURCES += ../shared/zlib/adler32.c ../shared/zlib/crc32.c ../shared/zlib/inffast.c ../shared/zlib/inflate.c ../shared/zlib/inftrees.c ../shared/zlib/zutil.c
goatattack_CXXFLAGS += -I../shared/zlib
else
goatattack_LDADD += -lz
endif

if DEDICATED_SERVER
goatattack_CXXFLAGS += -DDEDICATED_SERVER
else
goatattack_LDADD += -lGL -lSDL2 -lSDL2_mixer
iconsdir = $(datarootdir)/icons/hicolor/64x64/apps
dist_icons_DATA = ../../packages/linux/goatattack.png
desktopdir = $(datarootdir)/applications
dist_desktop_DATA = ../../packages/linux/goatattack.desktop
endif

goatattack_CXXFLAGS += -DDATA_DIRECTORY=$(pkgdatadir) -D__STDC_FORMAT_MACROS
