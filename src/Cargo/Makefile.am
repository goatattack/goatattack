if !INSTALL_CARGO
noinst_PROGRAMS = goatattack-cargo
else
bin_PROGRAMS = goatattack-cargo
endif

goatattack_cargo_SOURCES = main.cpp

goatattack_cargo_CXXFLAGS = -I./include -I../shared/include -pedantic -Wextra -Wno-unused-parameter -Wno-unused-result

goatattack_cargo_LDADD = -L../shared -lshared -lpng

if INTERNAL_ZLIB
goatattack_cargo_CXXFLAGS += -I../shared/zlib
else
goatattack_cargo_LDADD += -lz
endif

goatattack_cargo_CXXFLAGS += -D__STDC_FORMAT_MACROS

if INSTALL_CARGO
man6_MANS = man/goatattack-cargo.6
endif

if BUILD_PAK
all: goatattack-cargo
	./goatattack-cargo --pack ../../data/free ../../data/base.pak
if NON_FREE_PAK
	./goatattack-cargo --pack ../../data/non-free ../../data/base-non-free.pak
endif

endif
