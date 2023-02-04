#!/usr/bin/env sh
#
# ###################################################################################
# File: img2bin.sh
# Project: img2bin - converts image into RGB565 binary or C/C++ data structure
# File Created: Wednesday, 1st February 2023 4:00:58 pm
# Author: sk
# Last Modified: Friday, 3rd February 2023 7:10:36 pm
# Modified By: Sergey Ko
# ###################################################################################
# CHANGELOG:
# ###################################################################################
#
echo "\033[2J"
TW=$(stty size | awk '{print $2}')
N=$(((TW/2)-23))
DR=$(cd $(dirname "$0") && pwd)
SN="fbcpX"
SS=""
AUTO=0
AI=0
PR="installation"
UNS=0
SERV=0
ARGS=$@
for n in $ARGS; do
    if [ "$n" = "-y" ]; then
        AUTO=1
    elif [ "$n" = "-u" ]; then
        PR="uninstall"
        UNS=1
    elif [ "$n" = "-s" ]; then
        SERV=1
    fi
done
BLD="$DR/build"
SRC="$DR/src"
BIN="/usr/local/bin"
ETC="/etc/fbcpx"
SVS="/etc/systemd/system"
echo 
echo "\033[32m███████╗██████╗  ██████╗██████╗       ██╗  ██╗" | { perl -pe "s/^/' 'x$N/e"; }
echo         "██╔════╝██╔══██╗██╔════╝██╔══██╗      ╚██╗██╔╝" | { perl -pe "s/^/' 'x$N/e"; }
echo         "█████╗  ██████╔╝██║     ██████╔╝█████╗ ╚███╔╝ " | { perl -pe "s/^/' 'x$N/e"; }
echo         "██╔══╝  ██╔══██╗██║     ██╔═══╝ ╚════╝ ██╔██╗ " | { perl -pe "s/^/' 'x$N/e"; }
echo         "██║     ██████╔╝╚██████╗██║           ██╔╝ ██╗" | { perl -pe "s/^/' 'x$N/e"; }
echo         "╚═╝     ╚═════╝  ╚═════╝╚═╝           ╚═╝  ╚═╝" | { perl -pe "s/^/' 'x$N/e"; }
echo         "                  display drivers for SPI LCDs" | { perl -pe "s/^/' 'x$N/e"; }
echo
printf "%0.s=" $(seq 1 $TW)
if [ "$(id -u)" -ne 0 ]; then 
    echo "\n"
    echo "               \033[1;31mPlease run as root\033[0m\033[32m" | { perl -pe "s/^/' 'x$N/e"; }
    echo
    printf "%0.s=" $(seq 1 $TW)
    echo "\033[0m"
    exit 1
fi
if [ $AUTO -ne 1 ]; then
    echo "\n"
    echo "      The driver has been checked to work on the following systems:\n\
            - Raspberry Pi 3 Model B Rev 1.2 with DietPi (v8.12 - 8.13.2)\n\
            - Raspberry Pi Zero W with DietPi (v8.12 - 8.13.2)"
if [ -f "$BIN/fbcpx" ]; then
    echo
    echo "      (i) fbcpX already installed on this system"
    echo
    AI=1
    SS="$(systemctl is-active $SN)"
    if [ $UNS -ne 1 ] && [ -f "$SVS/$SN.service" ]; then
        SERV=1
    fi
    PR="reinstall/update"
fi
    echo "      Todo:"
if [ $UNS -ne 1 ]; then
    echo "          1. build executable file"
    echo "          2. install executable and configuration directory:\033[0m"
    echo "              - fbcpx \033[35m->\033[0m $BIN"
    echo "              - $SN.conf \033[35m->\033[0m $ETC"
    if [ $SERV -eq 1 ]; then
    echo "              - $SN.service \033[35m->\033[0m $SVS"
    echo "          \033[32m3. activate service"
    fi
else
    echo "          \033[32m1. stop/deactivate service"
    echo "          2. remove installed files and directories"
    echo "          3. clean build"
fi
    echo
    echo "      Options:"
    if [ $SERV -ne 1 ]; then
    echo "          -s : install service"
    fi
    echo "          -y : skip this intro"
    echo "          -u : uninstall and clean"
    echo
    echo "      \033[33moriginal repo.:  https://github.com/juj/fbcp-ili9341\033[0m"
    echo "      \033[33mimage converter: https://github.com/way5/image-converter-for-embedded\033[0m"
    echo "\033[32m"
    printf "%0.s=" $(seq 1 $TW)
    echo "\033[0m"
    echo
    echo "Press \033[1mENTER\033[0m to continue with \033[4;31m$PR\033[0m, or \033[1mCTRL+C\033[0m to exit..."
    read rsn1
else
    echo "\033[0m"
fi
if [ $UNS -ne 1 ]; then
    if [ $AI -eq 1 ]; then 
        if [ "$SS" = "active" ]; then
            systemctl stop $SN
        fi
        # do something with prev install
    fi
    ## install
    if [ $SERV -eq 1 ]; then
        if [ ! -d "$SVS" ]; then
            echo "\033[31m$SVS doesn't exists, check path\033[0m\n"
            exit 0
        fi
        cp "$BLD/$SN.service" $SVS
    fi
    if [ ! -d "$ETC" ]; then
        mkdir "$ETC"
    fi
    if [ ! -d "$BLD" ]; then
        mkdir "$BLD"
    fi
    cp "$BLD/$SN.conf" $ETC
    cmake -S "$SRC" -B "$BLD"
    make -j 3 -C "$DR/build"
    cp "$BLD/fbcpx" $BIN
    chmod a+x "$BIN/fbcpx"
    cp "$DR/splash/default.bin" "$ETC/splash.bin"
    echo "\n\033[32m- using default splash screen bitmap\033[0m\n"
    if [ $SERV -eq 1 ]; then
        echo "\n\033[32m- initializing service\033[0m\n"
        systemctl daemon-reload
        systemctl enable $SN
        systemctl start $SN
        echo "\n\033[32m- operation status\033[0m\n"
        systemctl status $SN
        SS="$(systemctl is-active $SN)"
        if [ "$SS" = "active" ]; then
            echo "\n\033[32m- success\033[0m\n"
        else
            echo "\n\033[1;31m- failed to start service\033[0m\n"
        fi
    else
        echo "\n\033[32m- success\033[0m\n"
    fi
else
    ## removing
    if [ $SERV -eq 1 ]; then
        echo "\033[38;2;255;200;0m-s option has no effect, uninstalling...\033[0m"
    fi
    if [ -f "$SVS/$SN.service" ]; then
        echo "\n\033[32m- disabling service\033[0m\n"
        systemctl stop $SN > /dev/null 2>&1
        SS="$(systemctl is-active $SN)"
        if [ "$SS" = "active" ]; then
            echo "\n\033[1;31m- failed to stop service\033[0m\n"
            exit 1
        fi
        systemctl disable $SN
        systemctl daemon-reload
    fi
    echo "\n\033[32m- cleanup\033[0m\n"
    rm "$BIN/fbcpx" > /dev/null 2>&1
    rm "$SVS/$SN.service" > /dev/null 2>&1
    rm -rf "$ETC" > /dev/null 2>&1
    make -C "$DR/build" clean
    echo "\n\033[32m- success\033[0m\n"
fi

exit 0