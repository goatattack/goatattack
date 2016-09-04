bin_PROGRAMS = goatattack
goatattack_SOURCES = main.cpp
goatattack_SOURCES += src/Client.cpp src/ClientEvent.cpp src/ClientServer.cpp src/Game.cpp src/KeyBinding.cpp src/LANBroadcaster.cpp src/MainMenu.cpp src/MasterQueryClient.cpp src/MasterQuery.cpp src/MessageSequencer.cpp src/OptionsMenu.cpp src/Player.cpp src/Server.cpp src/Tournament.cpp src/TournamentCTF.cpp src/TournamentDM.cpp src/TournamentDraw.cpp src/TournamentFactory.cpp src/TournamentNPC.cpp src/TournamentPhysics.cpp src/TournamentSpectator.cpp src/TournamentStates.cpp src/TournamentTDM.cpp src/TournamentSR.cpp src/TournamentCTC.cpp src/TournamentGOH.cpp src/TournamentTeam.cpp src/TournamentWeapons.cpp src/GameserverInformation.cpp src/ServerLogger.cpp src/ButtonNavigator.cpp src/ServerAdmin.cpp src/MapConfiguration.cpp

goatattack_CXXFLAGS = -I./include -I../shared/include -Wno-unused-result -pedantic -Wextra

goatattack_LDADD = -L../shared -lshared -lpng

if INTERNAL_ZLIB
goatattack_CXXFLAGS += -I../shared/zlib
else
goatattack_LDADD += -lz
endif

if DEDICATED_SERVER
goatattack_CXXFLAGS += -DDEDICATED_SERVER
man6_MANS = man/dedicated/goatattack.6
else
goatattack_LDADD += -lGL -lSDL2 -lSDL2_mixer
iconsdir = $(datarootdir)/icons/hicolor/scalable/apps
dist_icons_DATA = ../../packages/linux/goatattack.svg
desktopdir = $(datarootdir)/applications
dist_desktop_DATA = ../../packages/linux/goatattack.desktop
appdatadir = $(datarootdir)/appdata
dist_appdata_DATA = ../../packages/linux/goatattack.appdata.xml
man6_MANS = man/game/goatattack.6
endif

goatattack_CXXFLAGS += -DDATA_DIRECTORY=$(pkgdatadir) -D__STDC_FORMAT_MACROS

uninstall:
	rm -f $(bindir)/goatattack
	rm -fr $(datarootdir)/goatattack
	rm -f $(datarootdir)/icons/hicolor/scalable/apps/goatattack.svg
	rm -f $(datarootdir)/appdata/goatattack.appdata.xml
	rm -f $(datarootdir)/applications/goatattack.desktop
	rm -f $(datarootdir)/man/man6/goatattack.6
