##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=MapEditor
ConfigurationName      :=Debug
WorkspacePath          := "/home/flynn/Programming/goatattack/codelite"
ProjectPath            := "/home/flynn/Programming/goatattack/codelite/MapEditor"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Philippe Widmer
Date                   :=20/09/15
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
Preprocessors          :=$(PreprocessorSwitch)__unix__ $(PreprocessorSwitch)DATA_DIRECTORY=${HOME}/Programming/goatattack/data 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="MapEditor.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)../../src/MapEditor/include $(IncludeSwitch)../../src/shared/include $(IncludeSwitch)../../src/shared/zlib $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)png $(LibrarySwitch)pthread $(LibrarySwitch)rt $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_mixer $(LibrarySwitch)GL 
ArLibs                 :=  "png" "pthread" "rt" "SDL2" "SDL2_mixer" "GL" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -pedantic -Wno-unused-result -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/MapEditor_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CompileThread.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CompileThreadBlock.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CompileThreadPixel.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_EditableLight.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_EditableMap.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_EditableObject.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_MapEditor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_MapEditorLightmap.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_AABB.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_Animation.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Audio.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_AudioNull.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_AudioSDL.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Background.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Characterset.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Configuration.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Directory.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_Globals.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Gui.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_GuiObject.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Icon.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_KeyValue.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Lightmap.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Map.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Movable.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Music.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_NPC.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Object.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_PNG.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Properties.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Resources.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Sound.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Subsystem.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SubsystemNull.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SubsystemSDL.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_Thread.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Tile.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TileGraphic.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TileGraphicGL.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TileGraphicNull.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Tileset.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Timing.cpp$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/src_UDPSocket.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Utils.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_ZipReader.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_Zip.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_ZipWriter.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SHA256.cpp$(ObjectSuffix) $(IntermediateDirectory)/zlib_adler32.c$(ObjectSuffix) $(IntermediateDirectory)/zlib_crc32.c$(ObjectSuffix) $(IntermediateDirectory)/zlib_inffast.c$(ObjectSuffix) $(IntermediateDirectory)/zlib_inflate.c$(ObjectSuffix) $(IntermediateDirectory)/zlib_inftrees.c$(ObjectSuffix) $(IntermediateDirectory)/zlib_zutil.c$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo $(Objects1) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/MapEditor_main.cpp$(ObjectSuffix): ../../src/MapEditor/main.cpp $(IntermediateDirectory)/MapEditor_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MapEditor_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MapEditor_main.cpp$(DependSuffix): ../../src/MapEditor/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MapEditor_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MapEditor_main.cpp$(DependSuffix) -MM "../../src/MapEditor/main.cpp"

$(IntermediateDirectory)/MapEditor_main.cpp$(PreprocessSuffix): ../../src/MapEditor/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MapEditor_main.cpp$(PreprocessSuffix) "../../src/MapEditor/main.cpp"

$(IntermediateDirectory)/src_CompileThread.cpp$(ObjectSuffix): ../../src/MapEditor/src/CompileThread.cpp $(IntermediateDirectory)/src_CompileThread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/CompileThread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_CompileThread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_CompileThread.cpp$(DependSuffix): ../../src/MapEditor/src/CompileThread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_CompileThread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_CompileThread.cpp$(DependSuffix) -MM "../../src/MapEditor/src/CompileThread.cpp"

$(IntermediateDirectory)/src_CompileThread.cpp$(PreprocessSuffix): ../../src/MapEditor/src/CompileThread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_CompileThread.cpp$(PreprocessSuffix) "../../src/MapEditor/src/CompileThread.cpp"

$(IntermediateDirectory)/src_CompileThreadBlock.cpp$(ObjectSuffix): ../../src/MapEditor/src/CompileThreadBlock.cpp $(IntermediateDirectory)/src_CompileThreadBlock.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/CompileThreadBlock.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_CompileThreadBlock.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_CompileThreadBlock.cpp$(DependSuffix): ../../src/MapEditor/src/CompileThreadBlock.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_CompileThreadBlock.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_CompileThreadBlock.cpp$(DependSuffix) -MM "../../src/MapEditor/src/CompileThreadBlock.cpp"

$(IntermediateDirectory)/src_CompileThreadBlock.cpp$(PreprocessSuffix): ../../src/MapEditor/src/CompileThreadBlock.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_CompileThreadBlock.cpp$(PreprocessSuffix) "../../src/MapEditor/src/CompileThreadBlock.cpp"

$(IntermediateDirectory)/src_CompileThreadPixel.cpp$(ObjectSuffix): ../../src/MapEditor/src/CompileThreadPixel.cpp $(IntermediateDirectory)/src_CompileThreadPixel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/CompileThreadPixel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_CompileThreadPixel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_CompileThreadPixel.cpp$(DependSuffix): ../../src/MapEditor/src/CompileThreadPixel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_CompileThreadPixel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_CompileThreadPixel.cpp$(DependSuffix) -MM "../../src/MapEditor/src/CompileThreadPixel.cpp"

$(IntermediateDirectory)/src_CompileThreadPixel.cpp$(PreprocessSuffix): ../../src/MapEditor/src/CompileThreadPixel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_CompileThreadPixel.cpp$(PreprocessSuffix) "../../src/MapEditor/src/CompileThreadPixel.cpp"

$(IntermediateDirectory)/src_EditableLight.cpp$(ObjectSuffix): ../../src/MapEditor/src/EditableLight.cpp $(IntermediateDirectory)/src_EditableLight.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/EditableLight.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_EditableLight.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_EditableLight.cpp$(DependSuffix): ../../src/MapEditor/src/EditableLight.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_EditableLight.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_EditableLight.cpp$(DependSuffix) -MM "../../src/MapEditor/src/EditableLight.cpp"

$(IntermediateDirectory)/src_EditableLight.cpp$(PreprocessSuffix): ../../src/MapEditor/src/EditableLight.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_EditableLight.cpp$(PreprocessSuffix) "../../src/MapEditor/src/EditableLight.cpp"

$(IntermediateDirectory)/src_EditableMap.cpp$(ObjectSuffix): ../../src/MapEditor/src/EditableMap.cpp $(IntermediateDirectory)/src_EditableMap.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/EditableMap.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_EditableMap.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_EditableMap.cpp$(DependSuffix): ../../src/MapEditor/src/EditableMap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_EditableMap.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_EditableMap.cpp$(DependSuffix) -MM "../../src/MapEditor/src/EditableMap.cpp"

$(IntermediateDirectory)/src_EditableMap.cpp$(PreprocessSuffix): ../../src/MapEditor/src/EditableMap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_EditableMap.cpp$(PreprocessSuffix) "../../src/MapEditor/src/EditableMap.cpp"

$(IntermediateDirectory)/src_EditableObject.cpp$(ObjectSuffix): ../../src/MapEditor/src/EditableObject.cpp $(IntermediateDirectory)/src_EditableObject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/EditableObject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_EditableObject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_EditableObject.cpp$(DependSuffix): ../../src/MapEditor/src/EditableObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_EditableObject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_EditableObject.cpp$(DependSuffix) -MM "../../src/MapEditor/src/EditableObject.cpp"

$(IntermediateDirectory)/src_EditableObject.cpp$(PreprocessSuffix): ../../src/MapEditor/src/EditableObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_EditableObject.cpp$(PreprocessSuffix) "../../src/MapEditor/src/EditableObject.cpp"

$(IntermediateDirectory)/src_MapEditor.cpp$(ObjectSuffix): ../../src/MapEditor/src/MapEditor.cpp $(IntermediateDirectory)/src_MapEditor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/MapEditor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MapEditor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MapEditor.cpp$(DependSuffix): ../../src/MapEditor/src/MapEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MapEditor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MapEditor.cpp$(DependSuffix) -MM "../../src/MapEditor/src/MapEditor.cpp"

$(IntermediateDirectory)/src_MapEditor.cpp$(PreprocessSuffix): ../../src/MapEditor/src/MapEditor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MapEditor.cpp$(PreprocessSuffix) "../../src/MapEditor/src/MapEditor.cpp"

$(IntermediateDirectory)/src_MapEditorLightmap.cpp$(ObjectSuffix): ../../src/MapEditor/src/MapEditorLightmap.cpp $(IntermediateDirectory)/src_MapEditorLightmap.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/MapEditor/src/MapEditorLightmap.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MapEditorLightmap.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MapEditorLightmap.cpp$(DependSuffix): ../../src/MapEditor/src/MapEditorLightmap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MapEditorLightmap.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MapEditorLightmap.cpp$(DependSuffix) -MM "../../src/MapEditor/src/MapEditorLightmap.cpp"

$(IntermediateDirectory)/src_MapEditorLightmap.cpp$(PreprocessSuffix): ../../src/MapEditor/src/MapEditorLightmap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MapEditorLightmap.cpp$(PreprocessSuffix) "../../src/MapEditor/src/MapEditorLightmap.cpp"

$(IntermediateDirectory)/src_AABB.cpp$(ObjectSuffix): ../../src/shared/src/AABB.cpp $(IntermediateDirectory)/src_AABB.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/AABB.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_AABB.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_AABB.cpp$(DependSuffix): ../../src/shared/src/AABB.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_AABB.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_AABB.cpp$(DependSuffix) -MM "../../src/shared/src/AABB.cpp"

$(IntermediateDirectory)/src_AABB.cpp$(PreprocessSuffix): ../../src/shared/src/AABB.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_AABB.cpp$(PreprocessSuffix) "../../src/shared/src/AABB.cpp"

$(IntermediateDirectory)/src_Animation.cpp$(ObjectSuffix): ../../src/shared/src/Animation.cpp $(IntermediateDirectory)/src_Animation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Animation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Animation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Animation.cpp$(DependSuffix): ../../src/shared/src/Animation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Animation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Animation.cpp$(DependSuffix) -MM "../../src/shared/src/Animation.cpp"

$(IntermediateDirectory)/src_Animation.cpp$(PreprocessSuffix): ../../src/shared/src/Animation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Animation.cpp$(PreprocessSuffix) "../../src/shared/src/Animation.cpp"

$(IntermediateDirectory)/src_Audio.cpp$(ObjectSuffix): ../../src/shared/src/Audio.cpp $(IntermediateDirectory)/src_Audio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Audio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Audio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Audio.cpp$(DependSuffix): ../../src/shared/src/Audio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Audio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Audio.cpp$(DependSuffix) -MM "../../src/shared/src/Audio.cpp"

$(IntermediateDirectory)/src_Audio.cpp$(PreprocessSuffix): ../../src/shared/src/Audio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Audio.cpp$(PreprocessSuffix) "../../src/shared/src/Audio.cpp"

$(IntermediateDirectory)/src_AudioNull.cpp$(ObjectSuffix): ../../src/shared/src/AudioNull.cpp $(IntermediateDirectory)/src_AudioNull.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/AudioNull.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_AudioNull.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_AudioNull.cpp$(DependSuffix): ../../src/shared/src/AudioNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_AudioNull.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_AudioNull.cpp$(DependSuffix) -MM "../../src/shared/src/AudioNull.cpp"

$(IntermediateDirectory)/src_AudioNull.cpp$(PreprocessSuffix): ../../src/shared/src/AudioNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_AudioNull.cpp$(PreprocessSuffix) "../../src/shared/src/AudioNull.cpp"

$(IntermediateDirectory)/src_AudioSDL.cpp$(ObjectSuffix): ../../src/shared/src/AudioSDL.cpp $(IntermediateDirectory)/src_AudioSDL.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/AudioSDL.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_AudioSDL.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_AudioSDL.cpp$(DependSuffix): ../../src/shared/src/AudioSDL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_AudioSDL.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_AudioSDL.cpp$(DependSuffix) -MM "../../src/shared/src/AudioSDL.cpp"

$(IntermediateDirectory)/src_AudioSDL.cpp$(PreprocessSuffix): ../../src/shared/src/AudioSDL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_AudioSDL.cpp$(PreprocessSuffix) "../../src/shared/src/AudioSDL.cpp"

$(IntermediateDirectory)/src_Background.cpp$(ObjectSuffix): ../../src/shared/src/Background.cpp $(IntermediateDirectory)/src_Background.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Background.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Background.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Background.cpp$(DependSuffix): ../../src/shared/src/Background.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Background.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Background.cpp$(DependSuffix) -MM "../../src/shared/src/Background.cpp"

$(IntermediateDirectory)/src_Background.cpp$(PreprocessSuffix): ../../src/shared/src/Background.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Background.cpp$(PreprocessSuffix) "../../src/shared/src/Background.cpp"

$(IntermediateDirectory)/src_Characterset.cpp$(ObjectSuffix): ../../src/shared/src/Characterset.cpp $(IntermediateDirectory)/src_Characterset.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Characterset.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Characterset.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Characterset.cpp$(DependSuffix): ../../src/shared/src/Characterset.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Characterset.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Characterset.cpp$(DependSuffix) -MM "../../src/shared/src/Characterset.cpp"

$(IntermediateDirectory)/src_Characterset.cpp$(PreprocessSuffix): ../../src/shared/src/Characterset.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Characterset.cpp$(PreprocessSuffix) "../../src/shared/src/Characterset.cpp"

$(IntermediateDirectory)/src_Configuration.cpp$(ObjectSuffix): ../../src/shared/src/Configuration.cpp $(IntermediateDirectory)/src_Configuration.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Configuration.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Configuration.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Configuration.cpp$(DependSuffix): ../../src/shared/src/Configuration.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Configuration.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Configuration.cpp$(DependSuffix) -MM "../../src/shared/src/Configuration.cpp"

$(IntermediateDirectory)/src_Configuration.cpp$(PreprocessSuffix): ../../src/shared/src/Configuration.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Configuration.cpp$(PreprocessSuffix) "../../src/shared/src/Configuration.cpp"

$(IntermediateDirectory)/src_Directory.cpp$(ObjectSuffix): ../../src/shared/src/Directory.cpp $(IntermediateDirectory)/src_Directory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Directory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Directory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Directory.cpp$(DependSuffix): ../../src/shared/src/Directory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Directory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Directory.cpp$(DependSuffix) -MM "../../src/shared/src/Directory.cpp"

$(IntermediateDirectory)/src_Directory.cpp$(PreprocessSuffix): ../../src/shared/src/Directory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Directory.cpp$(PreprocessSuffix) "../../src/shared/src/Directory.cpp"

$(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix): ../../src/shared/src/Exception.cpp $(IntermediateDirectory)/src_Exception.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Exception.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Exception.cpp$(DependSuffix): ../../src/shared/src/Exception.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Exception.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Exception.cpp$(DependSuffix) -MM "../../src/shared/src/Exception.cpp"

$(IntermediateDirectory)/src_Exception.cpp$(PreprocessSuffix): ../../src/shared/src/Exception.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Exception.cpp$(PreprocessSuffix) "../../src/shared/src/Exception.cpp"

$(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix): ../../src/shared/src/Font.cpp $(IntermediateDirectory)/src_Font.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Font.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Font.cpp$(DependSuffix): ../../src/shared/src/Font.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Font.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Font.cpp$(DependSuffix) -MM "../../src/shared/src/Font.cpp"

$(IntermediateDirectory)/src_Font.cpp$(PreprocessSuffix): ../../src/shared/src/Font.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Font.cpp$(PreprocessSuffix) "../../src/shared/src/Font.cpp"

$(IntermediateDirectory)/src_Globals.cpp$(ObjectSuffix): ../../src/shared/src/Globals.cpp $(IntermediateDirectory)/src_Globals.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Globals.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Globals.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Globals.cpp$(DependSuffix): ../../src/shared/src/Globals.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Globals.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Globals.cpp$(DependSuffix) -MM "../../src/shared/src/Globals.cpp"

$(IntermediateDirectory)/src_Globals.cpp$(PreprocessSuffix): ../../src/shared/src/Globals.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Globals.cpp$(PreprocessSuffix) "../../src/shared/src/Globals.cpp"

$(IntermediateDirectory)/src_Gui.cpp$(ObjectSuffix): ../../src/shared/src/Gui.cpp $(IntermediateDirectory)/src_Gui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Gui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Gui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Gui.cpp$(DependSuffix): ../../src/shared/src/Gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Gui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Gui.cpp$(DependSuffix) -MM "../../src/shared/src/Gui.cpp"

$(IntermediateDirectory)/src_Gui.cpp$(PreprocessSuffix): ../../src/shared/src/Gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Gui.cpp$(PreprocessSuffix) "../../src/shared/src/Gui.cpp"

$(IntermediateDirectory)/src_GuiObject.cpp$(ObjectSuffix): ../../src/shared/src/GuiObject.cpp $(IntermediateDirectory)/src_GuiObject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/GuiObject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_GuiObject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_GuiObject.cpp$(DependSuffix): ../../src/shared/src/GuiObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_GuiObject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_GuiObject.cpp$(DependSuffix) -MM "../../src/shared/src/GuiObject.cpp"

$(IntermediateDirectory)/src_GuiObject.cpp$(PreprocessSuffix): ../../src/shared/src/GuiObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_GuiObject.cpp$(PreprocessSuffix) "../../src/shared/src/GuiObject.cpp"

$(IntermediateDirectory)/src_Icon.cpp$(ObjectSuffix): ../../src/shared/src/Icon.cpp $(IntermediateDirectory)/src_Icon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Icon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Icon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Icon.cpp$(DependSuffix): ../../src/shared/src/Icon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Icon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Icon.cpp$(DependSuffix) -MM "../../src/shared/src/Icon.cpp"

$(IntermediateDirectory)/src_Icon.cpp$(PreprocessSuffix): ../../src/shared/src/Icon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Icon.cpp$(PreprocessSuffix) "../../src/shared/src/Icon.cpp"

$(IntermediateDirectory)/src_KeyValue.cpp$(ObjectSuffix): ../../src/shared/src/KeyValue.cpp $(IntermediateDirectory)/src_KeyValue.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/KeyValue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_KeyValue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_KeyValue.cpp$(DependSuffix): ../../src/shared/src/KeyValue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_KeyValue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_KeyValue.cpp$(DependSuffix) -MM "../../src/shared/src/KeyValue.cpp"

$(IntermediateDirectory)/src_KeyValue.cpp$(PreprocessSuffix): ../../src/shared/src/KeyValue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_KeyValue.cpp$(PreprocessSuffix) "../../src/shared/src/KeyValue.cpp"

$(IntermediateDirectory)/src_Lightmap.cpp$(ObjectSuffix): ../../src/shared/src/Lightmap.cpp $(IntermediateDirectory)/src_Lightmap.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Lightmap.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Lightmap.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Lightmap.cpp$(DependSuffix): ../../src/shared/src/Lightmap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Lightmap.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Lightmap.cpp$(DependSuffix) -MM "../../src/shared/src/Lightmap.cpp"

$(IntermediateDirectory)/src_Lightmap.cpp$(PreprocessSuffix): ../../src/shared/src/Lightmap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Lightmap.cpp$(PreprocessSuffix) "../../src/shared/src/Lightmap.cpp"

$(IntermediateDirectory)/src_Map.cpp$(ObjectSuffix): ../../src/shared/src/Map.cpp $(IntermediateDirectory)/src_Map.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Map.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Map.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Map.cpp$(DependSuffix): ../../src/shared/src/Map.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Map.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Map.cpp$(DependSuffix) -MM "../../src/shared/src/Map.cpp"

$(IntermediateDirectory)/src_Map.cpp$(PreprocessSuffix): ../../src/shared/src/Map.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Map.cpp$(PreprocessSuffix) "../../src/shared/src/Map.cpp"

$(IntermediateDirectory)/src_Movable.cpp$(ObjectSuffix): ../../src/shared/src/Movable.cpp $(IntermediateDirectory)/src_Movable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Movable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Movable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Movable.cpp$(DependSuffix): ../../src/shared/src/Movable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Movable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Movable.cpp$(DependSuffix) -MM "../../src/shared/src/Movable.cpp"

$(IntermediateDirectory)/src_Movable.cpp$(PreprocessSuffix): ../../src/shared/src/Movable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Movable.cpp$(PreprocessSuffix) "../../src/shared/src/Movable.cpp"

$(IntermediateDirectory)/src_Music.cpp$(ObjectSuffix): ../../src/shared/src/Music.cpp $(IntermediateDirectory)/src_Music.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Music.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Music.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Music.cpp$(DependSuffix): ../../src/shared/src/Music.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Music.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Music.cpp$(DependSuffix) -MM "../../src/shared/src/Music.cpp"

$(IntermediateDirectory)/src_Music.cpp$(PreprocessSuffix): ../../src/shared/src/Music.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Music.cpp$(PreprocessSuffix) "../../src/shared/src/Music.cpp"

$(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix): ../../src/shared/src/Mutex.cpp $(IntermediateDirectory)/src_Mutex.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Mutex.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Mutex.cpp$(DependSuffix): ../../src/shared/src/Mutex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Mutex.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Mutex.cpp$(DependSuffix) -MM "../../src/shared/src/Mutex.cpp"

$(IntermediateDirectory)/src_Mutex.cpp$(PreprocessSuffix): ../../src/shared/src/Mutex.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Mutex.cpp$(PreprocessSuffix) "../../src/shared/src/Mutex.cpp"

$(IntermediateDirectory)/src_NPC.cpp$(ObjectSuffix): ../../src/shared/src/NPC.cpp $(IntermediateDirectory)/src_NPC.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/NPC.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_NPC.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_NPC.cpp$(DependSuffix): ../../src/shared/src/NPC.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_NPC.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_NPC.cpp$(DependSuffix) -MM "../../src/shared/src/NPC.cpp"

$(IntermediateDirectory)/src_NPC.cpp$(PreprocessSuffix): ../../src/shared/src/NPC.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_NPC.cpp$(PreprocessSuffix) "../../src/shared/src/NPC.cpp"

$(IntermediateDirectory)/src_Object.cpp$(ObjectSuffix): ../../src/shared/src/Object.cpp $(IntermediateDirectory)/src_Object.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Object.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Object.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Object.cpp$(DependSuffix): ../../src/shared/src/Object.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Object.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Object.cpp$(DependSuffix) -MM "../../src/shared/src/Object.cpp"

$(IntermediateDirectory)/src_Object.cpp$(PreprocessSuffix): ../../src/shared/src/Object.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Object.cpp$(PreprocessSuffix) "../../src/shared/src/Object.cpp"

$(IntermediateDirectory)/src_PNG.cpp$(ObjectSuffix): ../../src/shared/src/PNG.cpp $(IntermediateDirectory)/src_PNG.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/PNG.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_PNG.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_PNG.cpp$(DependSuffix): ../../src/shared/src/PNG.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_PNG.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_PNG.cpp$(DependSuffix) -MM "../../src/shared/src/PNG.cpp"

$(IntermediateDirectory)/src_PNG.cpp$(PreprocessSuffix): ../../src/shared/src/PNG.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_PNG.cpp$(PreprocessSuffix) "../../src/shared/src/PNG.cpp"

$(IntermediateDirectory)/src_Properties.cpp$(ObjectSuffix): ../../src/shared/src/Properties.cpp $(IntermediateDirectory)/src_Properties.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Properties.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Properties.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Properties.cpp$(DependSuffix): ../../src/shared/src/Properties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Properties.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Properties.cpp$(DependSuffix) -MM "../../src/shared/src/Properties.cpp"

$(IntermediateDirectory)/src_Properties.cpp$(PreprocessSuffix): ../../src/shared/src/Properties.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Properties.cpp$(PreprocessSuffix) "../../src/shared/src/Properties.cpp"

$(IntermediateDirectory)/src_Resources.cpp$(ObjectSuffix): ../../src/shared/src/Resources.cpp $(IntermediateDirectory)/src_Resources.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Resources.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Resources.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Resources.cpp$(DependSuffix): ../../src/shared/src/Resources.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Resources.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Resources.cpp$(DependSuffix) -MM "../../src/shared/src/Resources.cpp"

$(IntermediateDirectory)/src_Resources.cpp$(PreprocessSuffix): ../../src/shared/src/Resources.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Resources.cpp$(PreprocessSuffix) "../../src/shared/src/Resources.cpp"

$(IntermediateDirectory)/src_Sound.cpp$(ObjectSuffix): ../../src/shared/src/Sound.cpp $(IntermediateDirectory)/src_Sound.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Sound.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Sound.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Sound.cpp$(DependSuffix): ../../src/shared/src/Sound.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Sound.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Sound.cpp$(DependSuffix) -MM "../../src/shared/src/Sound.cpp"

$(IntermediateDirectory)/src_Sound.cpp$(PreprocessSuffix): ../../src/shared/src/Sound.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Sound.cpp$(PreprocessSuffix) "../../src/shared/src/Sound.cpp"

$(IntermediateDirectory)/src_Subsystem.cpp$(ObjectSuffix): ../../src/shared/src/Subsystem.cpp $(IntermediateDirectory)/src_Subsystem.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Subsystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Subsystem.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Subsystem.cpp$(DependSuffix): ../../src/shared/src/Subsystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Subsystem.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Subsystem.cpp$(DependSuffix) -MM "../../src/shared/src/Subsystem.cpp"

$(IntermediateDirectory)/src_Subsystem.cpp$(PreprocessSuffix): ../../src/shared/src/Subsystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Subsystem.cpp$(PreprocessSuffix) "../../src/shared/src/Subsystem.cpp"

$(IntermediateDirectory)/src_SubsystemNull.cpp$(ObjectSuffix): ../../src/shared/src/SubsystemNull.cpp $(IntermediateDirectory)/src_SubsystemNull.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/SubsystemNull.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SubsystemNull.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SubsystemNull.cpp$(DependSuffix): ../../src/shared/src/SubsystemNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SubsystemNull.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SubsystemNull.cpp$(DependSuffix) -MM "../../src/shared/src/SubsystemNull.cpp"

$(IntermediateDirectory)/src_SubsystemNull.cpp$(PreprocessSuffix): ../../src/shared/src/SubsystemNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SubsystemNull.cpp$(PreprocessSuffix) "../../src/shared/src/SubsystemNull.cpp"

$(IntermediateDirectory)/src_SubsystemSDL.cpp$(ObjectSuffix): ../../src/shared/src/SubsystemSDL.cpp $(IntermediateDirectory)/src_SubsystemSDL.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/SubsystemSDL.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SubsystemSDL.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SubsystemSDL.cpp$(DependSuffix): ../../src/shared/src/SubsystemSDL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SubsystemSDL.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SubsystemSDL.cpp$(DependSuffix) -MM "../../src/shared/src/SubsystemSDL.cpp"

$(IntermediateDirectory)/src_SubsystemSDL.cpp$(PreprocessSuffix): ../../src/shared/src/SubsystemSDL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SubsystemSDL.cpp$(PreprocessSuffix) "../../src/shared/src/SubsystemSDL.cpp"

$(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix): ../../src/shared/src/TCPSocket.cpp $(IntermediateDirectory)/src_TCPSocket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/TCPSocket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TCPSocket.cpp$(DependSuffix): ../../src/shared/src/TCPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TCPSocket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TCPSocket.cpp$(DependSuffix) -MM "../../src/shared/src/TCPSocket.cpp"

$(IntermediateDirectory)/src_TCPSocket.cpp$(PreprocessSuffix): ../../src/shared/src/TCPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TCPSocket.cpp$(PreprocessSuffix) "../../src/shared/src/TCPSocket.cpp"

$(IntermediateDirectory)/src_Thread.cpp$(ObjectSuffix): ../../src/shared/src/Thread.cpp $(IntermediateDirectory)/src_Thread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Thread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Thread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Thread.cpp$(DependSuffix): ../../src/shared/src/Thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Thread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Thread.cpp$(DependSuffix) -MM "../../src/shared/src/Thread.cpp"

$(IntermediateDirectory)/src_Thread.cpp$(PreprocessSuffix): ../../src/shared/src/Thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Thread.cpp$(PreprocessSuffix) "../../src/shared/src/Thread.cpp"

$(IntermediateDirectory)/src_Tile.cpp$(ObjectSuffix): ../../src/shared/src/Tile.cpp $(IntermediateDirectory)/src_Tile.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Tile.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Tile.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Tile.cpp$(DependSuffix): ../../src/shared/src/Tile.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Tile.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Tile.cpp$(DependSuffix) -MM "../../src/shared/src/Tile.cpp"

$(IntermediateDirectory)/src_Tile.cpp$(PreprocessSuffix): ../../src/shared/src/Tile.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Tile.cpp$(PreprocessSuffix) "../../src/shared/src/Tile.cpp"

$(IntermediateDirectory)/src_TileGraphic.cpp$(ObjectSuffix): ../../src/shared/src/TileGraphic.cpp $(IntermediateDirectory)/src_TileGraphic.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/TileGraphic.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TileGraphic.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TileGraphic.cpp$(DependSuffix): ../../src/shared/src/TileGraphic.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TileGraphic.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TileGraphic.cpp$(DependSuffix) -MM "../../src/shared/src/TileGraphic.cpp"

$(IntermediateDirectory)/src_TileGraphic.cpp$(PreprocessSuffix): ../../src/shared/src/TileGraphic.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TileGraphic.cpp$(PreprocessSuffix) "../../src/shared/src/TileGraphic.cpp"

$(IntermediateDirectory)/src_TileGraphicGL.cpp$(ObjectSuffix): ../../src/shared/src/TileGraphicGL.cpp $(IntermediateDirectory)/src_TileGraphicGL.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/TileGraphicGL.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TileGraphicGL.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TileGraphicGL.cpp$(DependSuffix): ../../src/shared/src/TileGraphicGL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TileGraphicGL.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TileGraphicGL.cpp$(DependSuffix) -MM "../../src/shared/src/TileGraphicGL.cpp"

$(IntermediateDirectory)/src_TileGraphicGL.cpp$(PreprocessSuffix): ../../src/shared/src/TileGraphicGL.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TileGraphicGL.cpp$(PreprocessSuffix) "../../src/shared/src/TileGraphicGL.cpp"

$(IntermediateDirectory)/src_TileGraphicNull.cpp$(ObjectSuffix): ../../src/shared/src/TileGraphicNull.cpp $(IntermediateDirectory)/src_TileGraphicNull.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/TileGraphicNull.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TileGraphicNull.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TileGraphicNull.cpp$(DependSuffix): ../../src/shared/src/TileGraphicNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TileGraphicNull.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TileGraphicNull.cpp$(DependSuffix) -MM "../../src/shared/src/TileGraphicNull.cpp"

$(IntermediateDirectory)/src_TileGraphicNull.cpp$(PreprocessSuffix): ../../src/shared/src/TileGraphicNull.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TileGraphicNull.cpp$(PreprocessSuffix) "../../src/shared/src/TileGraphicNull.cpp"

$(IntermediateDirectory)/src_Tileset.cpp$(ObjectSuffix): ../../src/shared/src/Tileset.cpp $(IntermediateDirectory)/src_Tileset.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Tileset.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Tileset.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Tileset.cpp$(DependSuffix): ../../src/shared/src/Tileset.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Tileset.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Tileset.cpp$(DependSuffix) -MM "../../src/shared/src/Tileset.cpp"

$(IntermediateDirectory)/src_Tileset.cpp$(PreprocessSuffix): ../../src/shared/src/Tileset.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Tileset.cpp$(PreprocessSuffix) "../../src/shared/src/Tileset.cpp"

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

$(IntermediateDirectory)/src_Utils.cpp$(ObjectSuffix): ../../src/shared/src/Utils.cpp $(IntermediateDirectory)/src_Utils.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Utils.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Utils.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Utils.cpp$(DependSuffix): ../../src/shared/src/Utils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Utils.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Utils.cpp$(DependSuffix) -MM "../../src/shared/src/Utils.cpp"

$(IntermediateDirectory)/src_Utils.cpp$(PreprocessSuffix): ../../src/shared/src/Utils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Utils.cpp$(PreprocessSuffix) "../../src/shared/src/Utils.cpp"

$(IntermediateDirectory)/src_ZipReader.cpp$(ObjectSuffix): ../../src/shared/src/ZipReader.cpp $(IntermediateDirectory)/src_ZipReader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/ZipReader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ZipReader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ZipReader.cpp$(DependSuffix): ../../src/shared/src/ZipReader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ZipReader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ZipReader.cpp$(DependSuffix) -MM "../../src/shared/src/ZipReader.cpp"

$(IntermediateDirectory)/src_ZipReader.cpp$(PreprocessSuffix): ../../src/shared/src/ZipReader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ZipReader.cpp$(PreprocessSuffix) "../../src/shared/src/ZipReader.cpp"

$(IntermediateDirectory)/src_Zip.cpp$(ObjectSuffix): ../../src/shared/src/Zip.cpp $(IntermediateDirectory)/src_Zip.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/Zip.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Zip.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Zip.cpp$(DependSuffix): ../../src/shared/src/Zip.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Zip.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Zip.cpp$(DependSuffix) -MM "../../src/shared/src/Zip.cpp"

$(IntermediateDirectory)/src_Zip.cpp$(PreprocessSuffix): ../../src/shared/src/Zip.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Zip.cpp$(PreprocessSuffix) "../../src/shared/src/Zip.cpp"

$(IntermediateDirectory)/src_ZipWriter.cpp$(ObjectSuffix): ../../src/shared/src/ZipWriter.cpp $(IntermediateDirectory)/src_ZipWriter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/ZipWriter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ZipWriter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ZipWriter.cpp$(DependSuffix): ../../src/shared/src/ZipWriter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ZipWriter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ZipWriter.cpp$(DependSuffix) -MM "../../src/shared/src/ZipWriter.cpp"

$(IntermediateDirectory)/src_ZipWriter.cpp$(PreprocessSuffix): ../../src/shared/src/ZipWriter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ZipWriter.cpp$(PreprocessSuffix) "../../src/shared/src/ZipWriter.cpp"

$(IntermediateDirectory)/src_SHA256.cpp$(ObjectSuffix): ../../src/shared/src/SHA256.cpp $(IntermediateDirectory)/src_SHA256.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/src/SHA256.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SHA256.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SHA256.cpp$(DependSuffix): ../../src/shared/src/SHA256.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SHA256.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SHA256.cpp$(DependSuffix) -MM "../../src/shared/src/SHA256.cpp"

$(IntermediateDirectory)/src_SHA256.cpp$(PreprocessSuffix): ../../src/shared/src/SHA256.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SHA256.cpp$(PreprocessSuffix) "../../src/shared/src/SHA256.cpp"

$(IntermediateDirectory)/zlib_adler32.c$(ObjectSuffix): ../../src/shared/zlib/adler32.c $(IntermediateDirectory)/zlib_adler32.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/zlib/adler32.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/zlib_adler32.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/zlib_adler32.c$(DependSuffix): ../../src/shared/zlib/adler32.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/zlib_adler32.c$(ObjectSuffix) -MF$(IntermediateDirectory)/zlib_adler32.c$(DependSuffix) -MM "../../src/shared/zlib/adler32.c"

$(IntermediateDirectory)/zlib_adler32.c$(PreprocessSuffix): ../../src/shared/zlib/adler32.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/zlib_adler32.c$(PreprocessSuffix) "../../src/shared/zlib/adler32.c"

$(IntermediateDirectory)/zlib_crc32.c$(ObjectSuffix): ../../src/shared/zlib/crc32.c $(IntermediateDirectory)/zlib_crc32.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/zlib/crc32.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/zlib_crc32.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/zlib_crc32.c$(DependSuffix): ../../src/shared/zlib/crc32.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/zlib_crc32.c$(ObjectSuffix) -MF$(IntermediateDirectory)/zlib_crc32.c$(DependSuffix) -MM "../../src/shared/zlib/crc32.c"

$(IntermediateDirectory)/zlib_crc32.c$(PreprocessSuffix): ../../src/shared/zlib/crc32.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/zlib_crc32.c$(PreprocessSuffix) "../../src/shared/zlib/crc32.c"

$(IntermediateDirectory)/zlib_inffast.c$(ObjectSuffix): ../../src/shared/zlib/inffast.c $(IntermediateDirectory)/zlib_inffast.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/zlib/inffast.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/zlib_inffast.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/zlib_inffast.c$(DependSuffix): ../../src/shared/zlib/inffast.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/zlib_inffast.c$(ObjectSuffix) -MF$(IntermediateDirectory)/zlib_inffast.c$(DependSuffix) -MM "../../src/shared/zlib/inffast.c"

$(IntermediateDirectory)/zlib_inffast.c$(PreprocessSuffix): ../../src/shared/zlib/inffast.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/zlib_inffast.c$(PreprocessSuffix) "../../src/shared/zlib/inffast.c"

$(IntermediateDirectory)/zlib_inflate.c$(ObjectSuffix): ../../src/shared/zlib/inflate.c $(IntermediateDirectory)/zlib_inflate.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/zlib/inflate.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/zlib_inflate.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/zlib_inflate.c$(DependSuffix): ../../src/shared/zlib/inflate.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/zlib_inflate.c$(ObjectSuffix) -MF$(IntermediateDirectory)/zlib_inflate.c$(DependSuffix) -MM "../../src/shared/zlib/inflate.c"

$(IntermediateDirectory)/zlib_inflate.c$(PreprocessSuffix): ../../src/shared/zlib/inflate.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/zlib_inflate.c$(PreprocessSuffix) "../../src/shared/zlib/inflate.c"

$(IntermediateDirectory)/zlib_inftrees.c$(ObjectSuffix): ../../src/shared/zlib/inftrees.c $(IntermediateDirectory)/zlib_inftrees.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/zlib/inftrees.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/zlib_inftrees.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/zlib_inftrees.c$(DependSuffix): ../../src/shared/zlib/inftrees.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/zlib_inftrees.c$(ObjectSuffix) -MF$(IntermediateDirectory)/zlib_inftrees.c$(DependSuffix) -MM "../../src/shared/zlib/inftrees.c"

$(IntermediateDirectory)/zlib_inftrees.c$(PreprocessSuffix): ../../src/shared/zlib/inftrees.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/zlib_inftrees.c$(PreprocessSuffix) "../../src/shared/zlib/inftrees.c"

$(IntermediateDirectory)/zlib_zutil.c$(ObjectSuffix): ../../src/shared/zlib/zutil.c $(IntermediateDirectory)/zlib_zutil.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/flynn/Programming/goatattack/src/shared/zlib/zutil.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/zlib_zutil.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/zlib_zutil.c$(DependSuffix): ../../src/shared/zlib/zutil.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/zlib_zutil.c$(ObjectSuffix) -MF$(IntermediateDirectory)/zlib_zutil.c$(DependSuffix) -MM "../../src/shared/zlib/zutil.c"

$(IntermediateDirectory)/zlib_zutil.c$(PreprocessSuffix): ../../src/shared/zlib/zutil.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/zlib_zutil.c$(PreprocessSuffix) "../../src/shared/zlib/zutil.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


