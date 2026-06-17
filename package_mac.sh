#!/bin/bash
set -e

# ANSI Color Codes for beautiful output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}==================================================${NC}"
echo -e "${BLUE}     Unreal Engine macOS Packaging Script         ${NC}"
echo -e "${BLUE}==================================================${NC}"

# Define directories
PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
UPROJECT_PATH="$PROJECT_DIR/VoD.uproject"
RUNUAT_PATH="/Users/Shared/Epic Games/UE_5.7/Engine/Build/BatchFiles/RunUAT.sh"
ARCHIVE_DIR="$PROJECT_DIR/ArchivedBuilds"

# Ensure RunUAT exists
if [ ! -f "$RUNUAT_PATH" ]; then
    echo -e "${RED}Error: RunUAT.sh not found at: $RUNUAT_PATH${NC}"
    exit 1
fi

echo -e "${YELLOW}[1/5] Building and packaging game with RunUAT...${NC}"
# Run the Unreal Automation Tool to build, cook, and stage the game
# Redirecting output log slightly or running it directly
"$RUNUAT_PATH" BuildCookRun \
  -project="$UPROJECT_PATH" \
  -platform=Mac \
  -clientconfig=Shipping \
  -cook \
  -allmaps \
  -build \
  -stage \
  -pak \
  -archive \
  -archivedirectory="$ARCHIVE_DIR"

echo -e "${GREEN}✓ Build and Stage completed successfully.${NC}"

# Find the packaged .app bundle inside the ArchivedBuilds/Mac directory
MAC_BUILD_DIR="$ARCHIVE_DIR/Mac"
if [ ! -d "$MAC_BUILD_DIR" ]; then
    echo -e "${RED}Error: Build output directory not found at: $MAC_BUILD_DIR${NC}"
    exit 1
fi

APP_BUNDLE=$(find "$MAC_BUILD_DIR" -maxdepth 1 -name "*.app" | head -n 1)

if [ -z "$APP_BUNDLE" ]; then
    echo -e "${RED}Error: Packaged .app bundle not found in $MAC_BUILD_DIR${NC}"
    exit 1
fi

APP_NAME=$(basename "$APP_BUNDLE")
FRIENDLY_NAME="Vilage of the damned.app"
FRIENDLY_APP_PATH="$MAC_BUILD_DIR/$FRIENDLY_NAME"

# Rename to friendly name if it's not already named so
if [ "$APP_NAME" != "$FRIENDLY_NAME" ]; then
    echo -e "${YELLOW}Renaming $APP_NAME to $FRIENDLY_NAME...${NC}"
    mv "$APP_BUNDLE" "$FRIENDLY_APP_PATH"
fi

echo -e "${GREEN}✓ Located app bundle: $FRIENDLY_NAME${NC}"

# 2. Re-sign the app bundle
echo -e "${YELLOW}[2/5] Performing ad-hoc code-signing to ensure signature health...${NC}"
codesign --force --deep --sign - "$FRIENDLY_APP_PATH"
echo -e "${GREEN}✓ Ad-hoc signing complete.${NC}"

# 3. Create DMG Staging Folder
echo -e "${YELLOW}[3/5] Setting up staging directories...${NC}"
DMG_STAGING="$ARCHIVE_DIR/DMG_Staging"
ZIP_STAGING="$ARCHIVE_DIR/ZIP_Staging"

# Clean up old staging folders if they exist
rm -rf "$DMG_STAGING" "$ZIP_STAGING"
mkdir -p "$DMG_STAGING"
mkdir -p "$ZIP_STAGING"

# Copy helper files if they exist in project root
FIX_SCRIPT="$PROJECT_DIR/fix_game.sh"
README_FILE="$PROJECT_DIR/README_MAC.md"

if [ -f "$FIX_SCRIPT" ]; then
    cp "$FIX_SCRIPT" "$DMG_STAGING/"
    cp "$FIX_SCRIPT" "$ZIP_STAGING/"
else
    echo -e "${YELLOW}Warning: fix_game.sh not found in project root.${NC}"
fi

if [ -f "$README_FILE" ]; then
    cp "$README_FILE" "$DMG_STAGING/"
    cp "$README_FILE" "$ZIP_STAGING/"
else
    echo -e "${YELLOW}Warning: README_MAC.md not found in project root.${NC}"
fi

# Copy app bundle to DMG staging and ZIP staging
echo -e "Copying game app to staging folders..."
cp -R "$FRIENDLY_APP_PATH" "$DMG_STAGING/"
cp -R "$FRIENDLY_APP_PATH" "$ZIP_STAGING/"

# Create /Applications symlink inside DMG staging folder
ln -sf /Applications "$DMG_STAGING/Applications"

# 4. Create DMG
echo -e "${YELLOW}[4/5] Creating DMG disk image...${NC}"
DMG_OUTPUT="$ARCHIVE_DIR/Vilage_of_the_damned.dmg"
rm -f "$DMG_OUTPUT"

hdiutil create -volname "Vilage of the damned" -srcfolder "$DMG_STAGING" -ov -format UDZO "$DMG_OUTPUT"
echo -e "${GREEN}✓ DMG Disk Image created: $DMG_OUTPUT${NC}"

# Copy to Desktop for easy access
DESKTOP_DIR="/Users/user/Desktop"
if [ -d "$DESKTOP_DIR" ]; then
    echo -e "Copying DMG to Desktop..."
    cp "$DMG_OUTPUT" "$DESKTOP_DIR/Vilage_of_the_damned.dmg"
    echo -e "${GREEN}✓ Copied DMG to Desktop.${NC}"
fi

# 5. Create ZIP using ditto
echo -e "${YELLOW}[5/5] Creating ZIP archive using ditto...${NC}"
ZIP_OUTPUT="$ARCHIVE_DIR/Vilage_of_the_damned.zip"
rm -f "$ZIP_OUTPUT"

# Use ditto to zip the folder, preserving permissions and resource forks
# Rename the top folder inside the zip to be friendly
cd "$ARCHIVE_DIR"
# We rename ZIP_Staging to "Vilage of the damned" temporarily to have the nice name when extracted
mv "ZIP_Staging" "Vilage of the damned"
ditto -c -k --keepParent "Vilage of the damned" "$ZIP_OUTPUT"
# Rename it back
mv "Vilage of the damned" "ZIP_Staging"

echo -e "${GREEN}✓ ZIP Archive created: $ZIP_OUTPUT${NC}"

# Clean up staging folders
rm -rf "$DMG_STAGING" "$ARCHIVE_DIR/ZIP_Staging"

echo -e "\n${GREEN}==================================================${NC}"
echo -e "${GREEN}       Packaging Completed Successfully!         ${NC}"
echo -e "${GREEN}==================================================${NC}"
echo -e "Outputs generated in: ${BLUE}$ARCHIVE_DIR/${NC}"
echo -e "  - ${GREEN}Vilage_of_the_damned.dmg${NC} (Recommended, also copied to Desktop)"
echo -e "  - ${GREEN}Vilage_of_the_damned.zip${NC} (Compressed with macOS metadata)"
echo -e "==================================================${NC}"
