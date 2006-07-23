!include "FileFunc.nsh"
!insertmacro Locate

;--------------------------------

!macro BIMAGE IMAGE PARMS
	Push $0
	GetTempFileName $0
	File /oname=$0 "${IMAGE}"
	SetBrandingImage ${PARMS} $0
	Delete $0
	Pop $0
!macroend

;--------------------------------

; The name of the installer
Name "Buggy-Man"
Caption "Buggy-Man"
Icon "res\icon.ico"
UninstallIcon "res\Shit.ico"

; The file to write
OutFile "Buggy-Man 1.0 Install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Buggy-Man

SetDateSave on
SetDatablockOptimize on
CRCCheck on
;BGGradient 000000 800000 FFFFFF
InstallColors FFFF00 0004FF
XPStyle on
AddBrandingImage left 196
InstProgressFlags smooth colored

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Buggy-Man" "Install_Dir"

LicenseText "Readme" "Let's Go!!"
LicenseData "_install\README.txt"
SubCaption 0 " "

;--------------------------------

; Pages

Page license branding
Page components branding
Page directory branding
Page instfiles branding

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

Function branding
	!insertmacro BIMAGE "res\branding.bmp" "";/RESIZETOFIT
FunctionEnd

;--------------------------------

; The stuff to install
Section "Buggy-Man 1.0 (Required)"

  SectionIn RO ;makes option Required
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  File /r _install\*.*

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Buggy-Man "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Buggy-Man" "DisplayName" "Buggy-Man"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Buggy-Man" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Buggy-Man" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Buggy-Man" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Buggy-Man"
  CreateShortCut "$SMPROGRAMS\Buggy-Man\Buggy-Man.lnk" "$INSTDIR\Buggy-Man.exe" "" "$INSTDIR\Buggy-Man.exe" 0
  CreateShortCut "$SMPROGRAMS\Buggy-Man\Manual.lnk" "$INSTDIR\MANUAL.txt" "" "$SYSDIR\SHELL32.dll" 1
  CreateShortCut "$SMPROGRAMS\Buggy-Man\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$DESKTOP\Buggy-Man.lnk" "$INSTDIR\Buggy-Man.exe" "" "$INSTDIR\Buggy-Man.exe" 0
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Buggy-Man"
  DeleteRegKey HKLM SOFTWARE\Buggy-Man

  ; Remove files and uninstaller
  ;RMDir /r $INSTDIR
  ;Delete $INSTDIR\*.*
  ;Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Buggy-Man\*.*"
  Delete "$DESKTOP\Buggy-Man.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Buggy-Man"
  RMDir /r "$INSTDIR"

SectionEnd

