; Define the name of the installer
Outfile "BinhoMissionControlSDK.exe"

; Set the default installation directory
InstallDir $PROGRAMFILES\BinhoMissionControlSDK

; Start the sections
Section "Main Files"

    ; Library and Sample App
    SetOutPath $INSTDIR
    File "staging\bmc_sdk.dll"
    File "staging\sample_app.exe"

    ; Bridge
    SetOutPath $INSTDIR\bridge
    File /r "staging\bridge\*.*"

    ; Includes
    SetOutPath $INSTDIR\include
    File /r "staging\include\*.*"

SectionEnd
