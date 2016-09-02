noinst_PROGRAMS = goatattack-cargo
goatattack_cargo_SOURCES = main.cpp
goatattack_cargo_SOURCES += src/Cargo.cpp

goatattack_cargo_CXXFLAGS = -I./include -I../shared/include -Wno-unused-result -pedantic -Wextra

goatattack_cargo_LDADD = -L../shared -lshared -lpng

if INTERNAL_ZLIB
goatattack_cargo_CXXFLAGS += -I../shared/zlib
else
goatattack_cargo_LDADD += -lz
endif

goatattack_cargo_CXXFLAGS += -D__STDC_FORMAT_MACROS
