#!/bin/bash

PROJECT_NAME="binho_cpp_sdk"

# Configuration
APPLE_ID="$APPLE_ID"
TEAM_ID="$TEAM_ID"
NOTARIZATION_PASSWORD="$NOTARIZATION_PASSWORD"
BUILD_CERTIFICATE_BASE64="$BUILD_CERTIFICATE_BASE64"
P12_PASSWORD="$P12_PASSWORD"
KEYCHAIN_PASSWORD="$KEYCHAIN_PASSWORD"
BUNDLE_ID="com.binhollc.$PROJECT_NAME"

# Validate input arguments
if [ $# -ne 3 ]; then
    echo "Usage: $0 <path-to-target> <temp-dir> <output-dmg-name>"
    exit 1
fi

TARGET_PATH="$1"
TEMP_DIR="$2"
DMG_OUTPUT_NAME="$3"

CERTIFICATE_PATH="$TEMP_DIR/build_certificate.p12"
KEYCHAIN_PATH="$TEMP_DIR/app-signing.keychain-db"
DIST_DIR="$TEMP_DIR/dist"
WRAPPER_DIR="$DIST_DIR/$PROJECT_NAME"
DMG_OUTPUT_PATH="$TEMP_DIR/$DMG_OUTPUT_NAME"

# Create a temporary keychain
create_keychain() {
    echo "Creating temporary keychain"
    security create-keychain -p "$KEYCHAIN_PASSWORD" "$KEYCHAIN_PATH"
    security set-keychain-settings -lut 21600 "$KEYCHAIN_PATH"
    security unlock-keychain -p "$KEYCHAIN_PASSWORD" "$KEYCHAIN_PATH"

    echo "Decoding and importing certificate"
    echo -n "$BUILD_CERTIFICATE_BASE64" | base64 --decode > "$CERTIFICATE_PATH"

    if [ ! -f "$CERTIFICATE_PATH" ]; then
        echo "Error: Certificate file not created"
        exit 1
    fi

    security import "$CERTIFICATE_PATH" -P "$P12_PASSWORD" -A -t cert -f pkcs12 -k "$KEYCHAIN_PATH"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to import certificate"
        exit 1
    fi

    rm "$CERTIFICATE_PATH"
    security list-keychain -d user -s "$KEYCHAIN_PATH"
}

# Sign binaries
sign_binaries() {
    find "$1" -type f \( -name "*.dylib" -o -name "*.so" -o -perm +111 \) -print0 | while IFS= read -r -d $'\0' file; do
        echo "Signing $file"
        codesign --deep --force --verbose --options runtime --timestamp --keychain "$KEYCHAIN_PATH" --sign "$TEAM_ID" "$file"
    done
}

# Wrap contents under distribution folder
prepare_wrapper_folder() {
    echo "Creating wrapper folder ${WRAPPER_DIR}"
    mkdir -p "$WRAPPER_DIR"
    cp -R "$TARGET_PATH"/* "$WRAPPER_DIR/"
}

# Create DMG from the wrapper
create_dmg() {
    echo "Creating DMG from $DIST_DIR into $DMG_OUTPUT_PATH"
    hdiutil create -volname "Binho SDK Tools" -srcfolder "$DIST_DIR" -ov -format UDZO "$DMG_OUTPUT_PATH"
}

# Notarize the DMG
notarize_app() {
    echo "Notarizing $1"
    xcrun notarytool submit "$1" --apple-id "$APPLE_ID" --team-id "$TEAM_ID" --password "$NOTARIZATION_PASSWORD" --wait
}

# Staple the notarization ticket
staple_dmg() {
    echo "Stapling $1"
    xcrun stapler staple "$1"
}

# Main script execution
create_keychain
sign_binaries "$TARGET_PATH"
prepare_wrapper_folder
create_dmg
notarize_app "$DMG_OUTPUT_PATH"
staple_dmg "$DMG_OUTPUT_PATH"

# Clean up
echo "Cleaning up keychain"
security delete-keychain "$KEYCHAIN_PATH"

echo "Signing and notarization process completed. DMG ready at:"
echo "$DMG_OUTPUT_PATH"
