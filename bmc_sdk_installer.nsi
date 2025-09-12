; Initialize OUTFILE with a default value
!ifndef OUTFILE
    !define OUTFILE "bmc_sdk_installer.exe"
!endif

; Define the name of the installer
Outfile "${OUTFILE}"

; Set the default installation directory
InstallDir c:\bmc_cpp_sdk

; Remove install directory if it exists before installation

Section -PreInstall
    IfFileExists "$INSTDIR\*.*" 0 done_remove
    MessageBox MB_YESNO|MB_ICONQUESTION "The installation directory $INSTDIR already exists. Do you want to remove it and continue?" IDYES do_remove IDNO cancel_install
    do_remove:
        RMDir /r "$INSTDIR"
        Goto done_remove
    cancel_install:
        Abort
    done_remove:
SectionEnd

; Text shown in the directory selection dialog
DirText "Please select the folder where the package will be installed:"

; Pages for the installer
Page directory           ; Directory selection page
Page instfiles           ; Installation progress page

Section "Main Files"

    ; Library and Sample App
    SetOutPath $INSTDIR
    File "staging\bin\bmc_sdk.dll"
    File "staging\lib\bmc_sdk.lib"
    File "staging\lib\bmc_sdk_static.lib"
    File "staging\docs\README.md"

    ; Includes
    SetOutPath $INSTDIR\include
    File /r "staging\include\*.*"

    ; Examples
    SetOutPath $INSTDIR\examples
    File /r "staging\examples\*.*"

SectionEnd
