#!/bin/sh
test -f Valentina-Installer.dmg && rm Valentina-Installer.dmg
create-dmg \
  --volname "Valentina Installer" \
  --volicon "../dist/Valentina.icns" \
  --background "../dist/dmg_background.png" \
  --window-pos 200 120 \
  --window-size 800 400 \
  --icon-size 100 \
  --icon "Valentina.app" 200 190 \
  --hide-extension "Valentina.app" \
  --app-drop-link 600 185 \
  "Valentina-Installer.dmg" \
  "/Users/dos095-russ/Downloads/myapp/"
