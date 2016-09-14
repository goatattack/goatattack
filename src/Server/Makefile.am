if !CARGO_ONLY
include ../Engine/game.mk

bin_PROGRAMS = goatattack-server
goatattack_server_SOURCES = $(sources)
goatattack_server_CXXFLAGS = $(cxxflags) -DDEDICATED_SERVER
goatattack_server_LDADD = $(ldadd)
man6_MANS = man/goatattack-server.6
endif
