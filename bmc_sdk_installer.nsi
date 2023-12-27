!define APP_VERSION "0.3.1"

; Define the name of the installer
Outfile "BinhoMissionControlSDK-${APP_VERSION}.exe"

; Set the default installation directory
InstallDir $PROGRAMFILES\BinhoMissionControlSDK

; Start the sections
Section "Main Files"

    ; Library and Sample App
    SetOutPath $INSTDIR
    File "staging\bin\bmc_sdk.dll"
    File "staging\lib\bmc_sdk.lib"
    File "staging\docs\README.md"

    ; Bridge
    SetOutPath $INSTDIR\bridge
    File /r "staging\bridge\*.*"

    ; Includes
    SetOutPath $INSTDIR\include
    File /r "staging\include\*.*"

    ; Examples
    SetOutPath $INSTDIR\examples
    File /r "staging\examples\*.*"

SectionEnd
