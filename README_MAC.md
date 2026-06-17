# Guide to Launching the Game on macOS

When you send a packaged Unreal Engine game to another Mac via a zip file, email, or file sharing service, macOS applies strict security checks (Gatekeeper) and resets file permissions. This causes the game to fail to open.

This guide provides simple solutions to fix these macOS-specific issues.

---

## Common Errors You Might Encounter

1. **"The application cannot be opened"** or **"Permission Denied"**
   - **Reason:** The file transfer process stripped the "executable" permission flag from the game binary.
2. **"Game is damaged and can't be opened. You should move it to the Trash."** or **"Developer cannot be verified"**
   - **Reason:** macOS Gatekeeper flags the game with a quarantine attribute (`com.apple.quarantine`) because it was downloaded or received from another computer.

---

## Solution 1: Automatic Fix (Recommended)

We have provided a helper script, `fix_game.sh`, to resolve these issues automatically.

### Steps to Run the Fix:
1. Extract (unzip) the game if you haven't already. You should see the game bundle (e.g., `VoD.app` or `Vilage of the damned.app`).
2. Keep the `fix_game.sh` script in the same folder as the game bundle.
3. Open the **Terminal** app on your Mac (you can find it in Applications > Utilities, or search with Spotlight).
4. Run the script by typing `bash ` and then dragging and dropping the `fix_game.sh` file from Finder into the Terminal window, then press **Enter**.
   - It will look like this in Terminal:
     ```bash
     bash /path/to/fix_game.sh
     ```
5. The script will automatically detect the game and apply the necessary fixes.
   - *Note:* If the game files are in a protected directory, the script might ask for your macOS administrator password to apply the fix.

---

## Solution 2: Manual Fix (Via Terminal)

If you prefer not to run the helper script, you can apply the commands manually.

1. Open the **Terminal** app.
2. Run the following command to remove the Gatekeeper quarantine lock (replace the path with the actual path to your game `.app` bundle):
   ```bash
   xattr -cr "/path/to/your/game.app"
   ```
   *(Tip: You can type `xattr -cr ` and then drag the `.app` bundle from Finder into the Terminal to automatically insert the path).*

3. Run the following command to restore executable permissions to the game's binary:
   ```bash
   chmod +x "/path/to/your/game.app/Contents/MacOS/"*
   ```

---

## Troubleshooting

### "Permission denied" or password prompt
If you receive a permission error while running the script or manual commands, prefix them with `sudo` to run with administrator rights:
```bash
sudo xattr -cr "/path/to/your/game.app"
sudo chmod +x "/path/to/your/game.app/Contents/MacOS/"*
```
You will be prompted to type your Mac's lock-screen password. Note that no characters will appear on screen as you type; this is normal macOS security behavior. Just type your password and press **Enter**.

### The game starts but has performance issues
Ensure you have copied the game out of any temporary download/extraction folders and placed it into your `Applications` folder or another local directory before running it.
