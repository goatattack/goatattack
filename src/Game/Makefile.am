if !CARGO_ONLY
include ../Engine/game.mk

bin_PROGRAMS = goatattack
goatattack_SOURCES = $(sources)
goatattack_CXXFLAGS = $(cxxflags)
goatattack_LDADD = $(ldadd) -lGL -lSDL2 -lSDL2_mixer
iconsdir = $(datarootdir)/icons/hicolor/scalable/apps
dist_icons_DATA = ../../packages/linux/goatattack.svg
desktopdir = $(datarootdir)/applications
dist_desktop_DATA = ../../packages/linux/goatattack.desktop
appdatadir = $(datarootdir)/appdata
dist_appdata_DATA = ../../packages/linux/goatattack.appdata.xml
man6_MANS = man/goatattack.6

uninstall:
	rm -f $(bindir)/goatattack
	rm -fr $(datarootdir)/goatattack
	rm -f $(datarootdir)/icons/hicolor/scalable/apps/goatattack.svg
	rm -f $(datarootdir)/appdata/goatattack.appdata.xml
	rm -f $(datarootdir)/applications/goatattack.desktop
	rm -f $(datarootdir)/man/man6/goatattack.6
endif
