#!/usr/bin/env bash

# A script which can make vscode users happy

! [ -e '../RIOT' ] && exit 1
make compile-commands
mkdir -p '../.vscode'
settings='../.vscode/settings.json'
key='"C_Cpp.default.compileCommands"'
value="\"$PWD/compile_commands.json\""
if ! [ -e "$settings" ]; then
    echo '{' >> "$settings"
    echo "    $key: $value," >> "$settings"
    echo '}' >> "$settings"
elif ! grep "$key" "$settings" > /dev/null; then
    sed -i -e '$i\' -e "    $key: $value," "$settings"
else
    sed -i "s|$key:.*,|$key: $value,|" "$settings"
fi
