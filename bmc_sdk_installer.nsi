; Initialize OUTFILE with a default value
!ifndef OUTFILE
    !define OUTFILE "bmc_sdk_installer.exe"
!endif

; Define the name of the installer
Outfile "${OUTFILE}"

; Set the default installation directory
InstallDir $PROGRAMFILES\BinhoMissionControlSDK

; Start the sections
Section "Main Files"

    ; Library and Sample App
    SetOutPath $INSTDIR
    File "staging\bin\bmc_sdk.dll"
    File "staging\lib\bmc_sdk.lib"
    File "staging\docs\README.md"

    ; Includes
    SetOutPath $INSTDIR\include
    File /r "staging\include\*.*"

    ; Examples
    SetOutPath $INSTDIR\examples
    File /r "staging\examples\*.*"

SectionEnd
