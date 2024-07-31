#!/bin/bash

# Configuration
APPLE_ID="$APPLE_ID"
TEAM_ID="$TEAM_ID"
NOTARIZATION_PASSWORD="$NOTARIZATION_PASSWORD"
BUILD_CERTIFICATE_BASE64="$BUILD_CERTIFICATE_BASE64"
P12_PASSWORD="$P12_PASSWORD"
KEYCHAIN_PASSWORD="$KEYCHAIN_PASSWORD"
BUNDLE_ID="com.binhollc.bmc_cpp_sdk"

# Validate input arguments
if [ $# -ne 3 ]; then
    echo "Usage: $0 <path-to-target> <temp-dir> <path-to-output-tar>"
    exit 1
fi

TARGET_PATH="$1"
TEMP_DIR="$2"
OUTPUT_TAR_PATH="$3"

CERTIFICATE_PATH="$TEMP_DIR/build_certificate.p12"
KEYCHAIN_PATH="$TEMP_DIR/app-signing.keychain-db"
ZIP_OUTPUT_PATH="$TEMP_DIR/signed.zip"

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

# Function to zip the folder for notarization
zip_folder() {
    echo "Zipping folder $1 into $2"
    (cd "$1" && zip -r "$2" .)
}

# Function to notarize the app
notarize_app() {
    echo "Notarizing $1"
    xcrun notarytool submit "$1" --apple-id "$APPLE_ID" --team-id "$TEAM_ID" --password "$NOTARIZATION_PASSWORD" --wait
}

# Staple the notarization ticket
staple_binaries() {
    echo "Stapling $1"
    find "$1" -type f \( -name "*.dylib" -o -name "*.so" -o -perm +111 \) -print0 | while IFS= read -r -d $'\0' file; do
        echo "Stapling $file"
        xcrun stapler staple "$file"
    done
}

tar_target() {
  echo "Tar target $1 into $2"
  (cd "$1" && tar -cvf "$2" .)
}

# Main script execution
create_keychain
sign_binaries "$TARGET_PATH"
zip_folder "$TARGET_PATH" "$ZIP_OUTPUT_PATH"
notarize_app "$ZIP_OUTPUT_PATH"
staple_binaries "$TARGET_PATH"
tar_target "$TARGET_PATH" "$OUTPUT_TAR_PATH"

# Clean up
echo "Cleaning up keychain"
security delete-keychain "$KEYCHAIN_PATH"

echo "Signing and notarization process completed."