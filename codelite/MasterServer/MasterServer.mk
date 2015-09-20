##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=MasterServer
ConfigurationName      :=Debug
WorkspacePath          := "/home/flynn/Programming/goatattack/codelite"
ProjectPath            := "/home/flynn/Programming/goatattack/codelite/MasterServer"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Philippe Widmer
Date                   :=27/08/15
CodeLitePath           :="/home/flynn/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="MasterServer.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)../../src/MasterServer/include $(IncludeSwitch)../../src/shared/include $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -pedantic -Wno-unused-result -g -O0 -Wall $(Preprocessors)
CFLAGS   := -pedantic -Wno-unused-result -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/MasterServer_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_MasterServer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Timing.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_UDPSocket.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/MasterServer_main.cpp$(ObjectSuffix): ../../src/MasterServer/main.cpp $(IntermediateDirectory)/MasterServer_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MasterServer/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MasterServer_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MasterServer_main.cpp$(DependSuffix): ../../src/MasterServer/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MasterServer_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MasterServer_main.cpp$(DependSuffix) -MM "../../src/MasterServer/main.cpp"

$(IntermediateDirectory)/MasterServer_main.cpp$(PreprocessSuffix): ../../src/MasterServer/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MasterServer_main.cpp$(PreprocessSuffix) "../../src/MasterServer/main.cpp"

$(IntermediateDirectory)/src_MasterServer.cpp$(ObjectSuffix): ../../src/MasterServer/src/MasterServer.cpp $(IntermediateDirectory)/src_MasterServer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MasterServer/src/MasterServer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MasterServer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MasterServer.cpp$(DependSuffix): ../../src/MasterServer/src/MasterServer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MasterServer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MasterServer.cpp$(DependSuffix) -MM "../../src/MasterServer/src/MasterServer.cpp"

$(IntermediateDirectory)/src_MasterServer.cpp$(PreprocessSuffix): ../../src/MasterServer/src/MasterServer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MasterServer.cpp$(PreprocessSuffix) "../../src/MasterServer/src/MasterServer.cpp"

$(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix): ../../src/shared/src/Exception.cpp $(IntermediateDirectory)/src_Exception.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Exception.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Exception.cpp$(DependSuffix): ../../src/shared/src/Exception.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Exception.cpp$(DependSuffix) -MM "../../src/shared/src/Exception.cpp"

$(IntermediateDirectory)/src_Exception.cpp$(PreprocessSuffix): ../../src/shared/src/Exception.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Exception.cpp$(PreprocessSuffix) "../../src/shared/src/Exception.cpp"

$(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix): ../../src/shared/src/Mutex.cpp $(IntermediateDirectory)/src_Mutex.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Mutex.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Mutex.cpp$(DependSuffix): ../../src/shared/src/Mutex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Mutex.cpp$(DependSuffix) -MM "../../src/shared/src/Mutex.cpp"

$(IntermediateDirectory)/src_Mutex.cpp$(PreprocessSuffix): ../../src/shared/src/Mutex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Mutex.cpp$(PreprocessSuffix) "../../src/shared/src/Mutex.cpp"

$(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix): ../../src/shared/src/TCPSocket.cpp $(IntermediateDirectory)/src_TCPSocket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/TCPSocket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TCPSocket.cpp$(DependSuffix): ../../src/shared/src/TCPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TCPSocket.cpp$(DependSuffix) -MM "../../src/shared/src/TCPSocket.cpp"

$(IntermediateDirectory)/src_TCPSocket.cpp$(PreprocessSuffix): ../../src/shared/src/TCPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TCPSocket.cpp$(PreprocessSuffix) "../../src/shared/src/TCPSocket.cpp"

$(IntermediateDirectory)/src_Timing.cpp$(ObjectSuffix): ../../src/shared/src/Timing.cpp $(IntermediateDirectory)/src_Timing.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Timing.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Timing.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Timing.cpp$(DependSuffix): ../../src/shared/src/Timing.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Timing.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Timing.cpp$(DependSuffix) -MM "../../src/shared/src/Timing.cpp"

$(IntermediateDirectory)/src_Timing.cpp$(PreprocessSuffix): ../../src/shared/src/Timing.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Timing.cpp$(PreprocessSuffix) "../../src/shared/src/Timing.cpp"

$(IntermediateDirectory)/src_UDPSocket.cpp$(ObjectSuffix): ../../src/shared/src/UDPSocket.cpp $(IntermediateDirectory)/src_UDPSocket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/UDPSocket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_UDPSocket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_UDPSocket.cpp$(DependSuffix): ../../src/shared/src/UDPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_UDPSocket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_UDPSocket.cpp$(DependSuffix) -MM "../../src/shared/src/UDPSocket.cpp"

$(IntermediateDirectory)/src_UDPSocket.cpp$(PreprocessSuffix): ../../src/shared/src/UDPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_UDPSocket.cpp$(PreprocessSuffix) "../../src/shared/src/UDPSocket.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


