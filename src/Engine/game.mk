sources = ../Engine/main.cpp
sources += ../Engine/src/Client.cpp ../Engine/src/ClientEvent.cpp ../Engine/src/ClientServer.cpp ../Engine/src/Game.cpp ../Engine/src/KeyBinding.cpp ../Engine/src/LANBroadcaster.cpp ../Engine/src/MainMenu.cpp ../Engine/src/MasterQueryClient.cpp ../Engine/src/MasterQuery.cpp ../Engine/src/MessageSequencer.cpp ../Engine/src/OptionsMenu.cpp ../Engine/src/Player.cpp ../Engine/src/Server.cpp ../Engine/src/Tournament.cpp ../Engine/src/TournamentCTF.cpp ../Engine/src/TournamentDM.cpp ../Engine/src/TournamentDraw.cpp ../Engine/src/TournamentFactory.cpp ../Engine/src/TournamentNPC.cpp ../Engine/src/TournamentPhysics.cpp ../Engine/src/TournamentSpectator.cpp ../Engine/src/TournamentStates.cpp ../Engine/src/TournamentTDM.cpp ../Engine/src/TournamentSR.cpp ../Engine/src/TournamentCTC.cpp ../Engine/src/TournamentGOH.cpp ../Engine/src/TournamentTeam.cpp ../Engine/src/TournamentWeapons.cpp ../Engine/src/GameserverInformation.cpp ../Engine/src/ServerLogger.cpp ../Engine/src/ButtonNavigator.cpp ../Engine/src/ServerAdmin.cpp ../Engine/src/MapConfiguration.cpp ../Engine/src/Lagometer.cpp
cxxflags = -I../Engine/include -I../shared/include -Wno-unused-result -pedantic -Wextra -DDATA_DIRECTORY=$(targetdatadir) -D__STDC_FORMAT_MACROS
if SAFE_ALIGNMENT
cxxflags += -DSAFE_ALIGNMENT
endif
ldadd = -L../shared -lshared -lpng
