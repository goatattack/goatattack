!include "MUI2.nsh"
!include "FileFunc.nsh"

SetCompressor /FINAL /SOLID lzma

;define MUI parameters
!define MUI_ABORTWARNING
!define MUI_ICON "installer data\icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "installer data\Panel.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "installer data\InstallerLogo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_UNICON "installer data\icon.ico"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "installer data\Panel.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "installer data\InstallerLogo.bmp"
!define MUI_FINISHPAGE_LINK "www.goatattack.net"
!define MUI_FINISHPAGE_LINK_LOCATION "www.goatattack.net"

;define key words
!define INSTALLATIONNAME "GoatAttack"
!define ARP "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}"


Name "Goat Attack"
OutFile "Build\Goat Attack Installer.exe"
InstallDir $PROGRAMFILES\${INSTALLATIONNAME}


Var StartMenuFolder

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${INSTALLATIONNAME}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

;set installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "installer data\GNU license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU $INSTALLATIONNAME $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;set languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"


;retrieve GoatAttack.exe file version number
!getdllversion "Resources\GoatAttack.exe" goatv_
!define VERSION "${goatv_1}.${goatv_2}.${goatv_3}.${goatv_4}"


;set installer metadata
VIProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${INSTALLATIONNAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "goatattack.net"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "InernalName" "Goat Attack Installer.exe"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Goat Attack"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C) 2015"


;check for previous installed versions
Function .onInit
  ReadRegStr $R0 HKLM "${ARP}" "UninstallString"
  StrCmp $R0 "" done

  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "${INSTALLATIONNAME} is already installed. $\n$\nClick `OK` to remove the \
  previous version or `Cancel` to cancel this upgrade." \
  IDOK uninst
  Abort

;Run the uninstaller
uninst:
  ReadRegStr $R1 HKLM "${ARP}" "InstallLocation"
  ClearErrors
  ExecWait '$R0 _?=$R1'
  
  IfErrors no_remove_uninstaller done
  no_remove_uninstaller:

done:
FunctionEnd


;main section to create unistaller and registry entries
Section ""
  SetOutPath $INSTDIR
  WriteUninstaller $INSTDIR\Uninstall.exe
  
  WriteRegStr HKLM "${ARP}" "DisplayName" "Goat Attack"
  WriteRegStr HKLM "${ARP}" "InstallLocation" '$INSTDIR'
  WriteRegStr HKLM "${ARP}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "${ARP}" "DisplayIcon" '"$INSTDIR\icon.ico"'
  WriteRegDWORD HKLM "${ARP}" "NoModify" 1
  WriteRegDWORD HKLM "${ARP}" "NoRepair" 1
SectionEnd


; These are the files that are needed to install Goat Attack.
Section -Prerequisites
  IfFileExists $SYSDIR\msvcp120.dll endCplusRestrib beginCplusRestrib
  Goto endCplusRestrib
  beginCplusRestrib:
  MessageBox MB_OK "Your system does not appear to have Visual C++ 2013 Redistributable installed.$\n$\nPress OK to install it."
  SetOutPath $INSTDIR\Prerequisites
  File "Prerequisites\vcredist_x86.exe"
  SetOutPath $INSTDIR
  ExecWait "$INSTDIR\Prerequisites\vcredist_x86.exe"
  RMDir /r /REBOOTOK "$INSTDIR\Prerequisites"
  endCplusRestrib:
SectionEnd


;installation of Goat Attack game part
Section "Goat Attack" Section1
  SectionIn RO

  ;copy program files
  SetOutPath $INSTDIR\data
  File /r "Resources\data\*"
  SetOutPath $INSTDIR
  File "Resources\libFLAC-8.dll"
  File "Resources\libmikmod-2.dll"
  File "Resources\libmodplug-1.dll"
  File "Resources\libogg-0.dll"
  File "Resources\libpng16.dll"
  File "Resources\libvorbis-0.dll"
  File "Resources\libvorbisfile-3.dll"
  File "Resources\SDL2.dll"
  File "Resources\SDL2_mixer.dll"
  File "Resources\smpeg2.dll"
  File "Resources\icon.ico"
  File "Resources\GoatAttack.exe"

  ;write start menu entries
  !insertmacro MUI_STARTMENU_WRITE_BEGIN $INSTALLATIONNAME

    ;Create shortcuts
    SetShellVarContext all
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\GoatAttack.lnk" "$INSTDIR\GoatAttack.exe"

  !insertmacro MUI_STARTMENU_WRITE_END
  
  ;calculate size of installation folder
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "${ARP}" "EstimatedSize" "$0"
  
  ;Add a firewall exception for Goat Attack - All Networks - All IP Version - Enabled
  ;Will be implemented later!
SectionEnd


;installation of the Map Editor
Section /o "Map Editor" Section2
  File "Resources\MapEditor.exe"

  ;write start menu entries
  !insertmacro MUI_STARTMENU_WRITE_BEGIN $INSTALLATIONNAME

    ;Create shortcuts
    SetShellVarContext all
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\MapEditor.lnk" "$INSTDIR\MapEditor.exe"

  !insertmacro MUI_STARTMENU_WRITE_END
  
  ;calculate size of installation folder
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "${ARP}" "EstimatedSize" "$0"
SectionEnd


; Descriptions for each selectable component
LangString DESC_Section1 ${LANG_ENGLISH} "Installation of the game: Goat Attack."
LangString DESC_Section1 ${LANG_GERMAN} "Installation des Spiels: Goat Attack."
LangString DESC_Section2 ${LANG_ENGLISH} "Installation of the Map Editor to create and modify own Maps."
LangString DESC_Section2 ${LANG_GERMAN} "Installiert den Map Editor, mit welchem eigene Maps erstellt und angepasst werden können."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Section1} $(DESC_Section1)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section2} $(DESC_Section2)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;set uninstaller package
Section "Uninstall"
  DeleteRegKey HKLM "${ARP}"
  RMDir /r /REBOOTOK $INSTDIR
  
  !insertmacro MUI_STARTMENU_GETFOLDER $INSTALLATIONNAME $StartMenuFolder

  SetShellVarContext all
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\GoatAttack.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\MapEditor.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  ;Remove firewall exception for Goat Attack
  ;Will be implemented later!
SectionEnd