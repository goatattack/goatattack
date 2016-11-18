if !CARGO_ONLY
bin_PROGRAMS = goatattack-masterserver
goatattack_masterserver_SOURCES = main.cpp
goatattack_masterserver_SOURCES += src/MasterServer.cpp
goatattack_masterserver_CXXFLAGS = -I ./include -I ../shared/include -pedantic -Wextra -Wno-unused-parameter -Wno-unused-result
goatattack_masterserver_LDADD = -L../shared -lshared
man6_MANS = man/goatattack-masterserver.6
endif
