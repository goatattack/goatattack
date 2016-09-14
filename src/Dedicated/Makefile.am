if !CARGO_ONLY
include ../Engine/game.mk

bin_PROGRAMS = goatattack-dedicated
goatattack_dedicated_SOURCES = $(sources)
goatattack_dedicated_CXXFLAGS = $(cxxflags) -DDEDICATED_SERVER
goatattack_dedicated_LDADD = $(ldadd)
man6_MANS = man/goatattack-dedicated.6
endif
