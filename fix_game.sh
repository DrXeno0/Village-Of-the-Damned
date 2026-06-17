#!/bin/bash

# ANSI Color Codes for beautiful output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}==================================================${NC}"
echo -e "${BLUE}          macOS Packaged Game Fixer               ${NC}"
echo -e "${BLUE}==================================================${NC}"
echo -e "This script resolves macOS Gatekeeper (\"damaged\" or \"unidentified developer\")"
echo -e "issues and restores missing executable permissions for shared games."
echo ""

# Find the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Target app variable
TARGET_APP=""

# 1. Look for any .app bundle in the same directory as the script
APP_BUNDLES=("$SCRIPT_DIR"/*.app)

# Check if the array expansion has actual files and not a literal *.app
if [ -e "${APP_BUNDLES[0]}" ]; then
    # If there's only one .app bundle, use it as default
    if [ ${#APP_BUNDLES[@]} -eq 1 ]; then
        TARGET_APP="${APP_BUNDLES[0]}"
        echo -e "${GREEN}Found game:${NC} $(basename "$TARGET_APP") in script directory."
    else
        echo -e "${YELLOW}Multiple .app bundles found in the script directory:${NC}"
        select opt in "${APP_BUNDLES[@]}" "Choose another location"; do
            if [ "$opt" = "Choose another location" ]; then
                break
            elif [ -n "$opt" ]; then
                TARGET_APP="$opt"
                break
            fi
        done
    fi
fi

# 2. If no .app bundle found or user wants to specify manually
if [ -z "$TARGET_APP" ]; then
    echo -e "${YELLOW}No game bundle automatically selected.${NC}"
    echo -e "Please drag and drop the ${YELLOW}.app${NC} file (e.g. VoD.app or Vilage of the damned.app) into this window and press [ENTER]:"
    read -r USER_INPUT
    
    # Strip outer single/double quotes if present
    USER_INPUT="${USER_INPUT#\'}"
    USER_INPUT="${USER_INPUT%\'}"
    USER_INPUT="${USER_INPUT#\"}"
    USER_INPUT="${USER_INPUT%\"}"
    
    # Replace backslash-escaped characters (macOS drag and drop escapes spaces, braces, etc. using backslash)
    CLEANED_PATH=$(echo "$USER_INPUT" | sed 's/\\\(.\)/\1/g')
    
    if [ -d "$CLEANED_PATH" ] && [[ "$CLEANED_PATH" == *.app ]]; then
        TARGET_APP="$CLEANED_PATH"
    else
        # Try finding it relative to script directory
        if [ -d "$SCRIPT_DIR/$CLEANED_PATH" ] && [[ "$CLEANED_PATH" == *.app ]]; then
            TARGET_APP="$SCRIPT_DIR/$CLEANED_PATH"
        fi
    fi
fi

# 3. Check if we found a valid app
if [ -z "$TARGET_APP" ] || [ ! -d "$TARGET_APP" ]; then
    echo -e "${RED}Error: Could not locate a valid .app bundle.${NC}"
    echo -e "Make sure the game is extracted (unzipped) and you provide the correct path."
    exit 1
fi

echo -e "\n${BLUE}Applying fixes to:${NC} $TARGET_APP"

# A. Remove the macOS Quarantine Attribute
echo -e "${YELLOW}[1/2] Removing quarantine flag (fixing Gatekeeper block)...${NC}"
if xattr -cr "$TARGET_APP" 2>/dev/null; then
    echo -e "  ${GREEN}✓ Quarantine flag cleared successfully.${NC}"
else
    # Try running with sudo if permission denied
    echo -e "  ${YELLOW}Permissions restricted. Attempting with administrative privileges (sudo)...${NC}"
    echo -e "  Please enter your macOS password if prompted:"
    if sudo xattr -cr "$TARGET_APP"; then
        echo -e "  ${GREEN}✓ Quarantine flag cleared (sudo).${NC}"
    else
        echo -e "  ${RED}✗ Failed to clear quarantine flag. Please check permissions.${NC}"
    fi
fi

# B. Restore Executable Permissions
echo -e "${YELLOW}[2/2] Restoring executable permissions...${NC}"

# Find all binaries in Contents/MacOS and set executable flag
MAC_OS_DIR="$TARGET_APP/Contents/MacOS"
if [ -d "$MAC_OS_DIR" ]; then
    if chmod +x "$MAC_OS_DIR"/* 2>/dev/null; then
        echo -e "  ${GREEN}✓ Executable permissions restored for internal binaries.${NC}"
    else
        # Try with sudo
        if sudo chmod +x "$MAC_OS_DIR"/*; then
            echo -e "  ${GREEN}✓ Executable permissions restored (sudo).${NC}"
        else
            echo -e "  ${RED}✗ Failed to set executable permissions.${NC}"
        fi
    fi
else
    # Fallback: make the whole app package contents executable
    if chmod -R +x "$TARGET_APP" 2>/dev/null; then
        echo -e "  ${GREEN}✓ Recursive permissions restored.${NC}"
    else
        sudo chmod -R +x "$TARGET_APP"
    fi
fi

echo -e "\n${GREEN}==================================================${NC}"
echo -e "${GREEN}             Fixes Applied Successfully!          ${NC}"
echo -e "${GREEN}==================================================${NC}"
echo -e "You can now launch the game by double-clicking it."
echo -e "If you still face issues, please consult the README_MAC.md file."
echo ""
