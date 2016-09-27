gamedatadir = $(pkgdatadir)
gamedata_DATA = base.pak
if NON_FREE_PAK
gamedata_DATA += base-non-free.pak
endif

no-data-clean:

save:
	cp base.pak base.pak_dist
if NON_FREE_PAK
	cp base-non-free.pak base-non-free.pak_dist
endif

restore:
	mv base.pak_dist base.pak
if NON_FREE_PAK
	mv base-non-free.pak_dist base-non-free.pak
endif

clean:
	rm -f *.pak
