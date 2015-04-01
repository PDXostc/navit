#!/bin/bash

VERBOSE=true
NO_HTML=true
NO_GUI=true
# it can be also a name of the host
TIZEN_IP="192.168.41.64"
GBS_ROOT="$HOME/GBS-ROOT"
BUILD_NAVIT=true
BUILD_NXE=true
BUILD_DEBUGINFO=true

# getopt section
TEMP=`getopt -o v,h,i:,g:,n,x,d,b --long verbose,help,tizen-ip:,gbs-root:,no-navit,no-nxe,no-debuginfo,build-gui -n 'flash.sh' -- "$@"`
eval set -- "$TEMP"
while true; do
    case "$1" in
    -v|--verbose)
        shift;
        VERBOSE=true
        ;;
    -h|--help)
        shift;
        echo "usage $0 -h -b -v"
        exit 0
        ;;
    -i|--tizen-ip)
        TIZEN_IP=$2
        shift 2;
        ;;
    -g|--gbs_root)
        GBS_ROOT=$2
        shift 2;
        ;;
    -n|--no-navit)
        BUILD_NAVIT=false;
        shift ;
        ;;
    -x|--no-nxe)
        BUILD_NXE=false;
        shift ;
        ;;
    -d|--no-debuginfo)
        BUILD_DEBUGINFO=false;
        shift ;
        ;;
    -b|--build-gui)
        NO_GUI=false;
        shift ;
        ;;
    --)
        shift;
        break;
        ;;
    esac
done

NXE_VERSION=0.1.1
NAVIT_VERSION=

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

unalias gbs > /dev/null 2>&1

# remove all rpms from target
echo "${red}Cleaning previous installation${reset}"

if [ "$BUILD_NXE" = true ]; then
    try ssh root@$TIZEN_IP rm /root/nxe* -rf
    # remove nxe
    ssh root@$TIZEN_IP zypper -n -q rm nxe > /dev/null 2>&1
    ssh root@$TIZEN_IP zypper -n -q rm nxe-debugsource > /dev/null 2>&1
fi

if [ "$BUILD_NAVIT" = true ]; then
    ssh root@$TIZEN_IP zypper -n -q rm navit > /dev/null 2>&1
    ssh root@$TIZEN_IP zypper -n -q rm navit-debugsource > /dev/null 2>&1
    if [ "$BUILD_DEBUGINFO" = true ]; then
        ssh root@$TIZEN_IP zypper -n -q rm navit-debuginfo > /dev/null 2>&1
    fi
fi

if [ "$BUILD_NAVIT" = true ]; then
    ssh root@$TIZEN_IP rm /root/navit* -rf
    # build navit

    echo "${red}Building navit${reset}"
    if [ "$VERBOSE" = true ]; then
        try gbs build -A i586 --spec navit_qt5.spec --include-all
    else
        try gbs build -A i586 --spec navit_qt5.spec --include-all > /dev/null 2>&1
    fi

    try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-0.5.0.6011svn-1.i686.rpm root@$TIZEN_IP:/root
    if [ "$BUILD_DEBUGINFO" = true ]; then
        try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-debuginfo-0.5.0.6011svn-1.i686.rpm root@$TIZEN_IP:/root
    fi
    try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-debugsource-0.5.0.6011svn-1.i686.rpm root@$TIZEN_IP:/root
fi

if [ "$BUILD_NXE" = true ]; then
    # build nxe
    echo "${red}Building nxe${reset}"
    if [ "$VERBOSE" = true ]; then
        try gbs build -A i586 --spec nxe.spec --include-all
    else
        try gbs build -A i586 --spec nxe.spec --include-all > /dev/null 2>&1
    fi

    try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/nxe-0.1-1.i686.rpm root@$TIZEN_IP:/root
    try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/nxe-debugsource-0.1-1.i686.rpm root@$TIZEN_IP:/root
fi

# install both
if [ "$BUILD_NAVIT" = true ]; then
    try ssh root@$TIZEN_IP rpm -ivh /root/navit*
fi

if [ "$BUILD_NXE" = true ]; then
    try ssh root@$TIZEN_IP rpm -ivh /root/nxe*
fi

if [ "$NO_HTML" = false ]; then
    # example
    cd Navigation/NXE/exampleapp
    try ./make_xpk.sh app ~/mykey.pem
    try scp app.xpk app@$TIZEN_IP:/home/app

    # stop xwalk
    try ssh app@$TIZEN_IP pkgcmd -u -n cjhbbeknomcehebnhobpolialjjnalad -q
    try ssh app@$TIZEN_IP pkgcmd -i -t xpk -p /home/app/app.xpk -q
    rm app.xpk
fi


# HTML5 GUI application build
if [ "$NO_GUI" = false ]; then
    echo "${red}Buuilding GUI application (please read gui_html5/README.txt)${reset}"
    echo "${red}Checking project dependencies [nodejs, npm, bower]${reset}"
    hash nodejs 2>/dev/null || { echo >&2 "I require nodejs but it's not installed.  Aborting."; exit 1; }
    hash npm 2>/dev/null || { echo >&2 "I require npm but it's not installed.  Aborting."; exit 1; }
    hash bower 2>/dev/null || { echo >&2 "I require bower but it's not installed.  Aborting."; exit 1; }
    cd gui_html5
    echo "${red}Installing application dependencies${reset}"
    npm install
    bower install
    echo "${red}Deploying GUI application${reset}"
    grunt deploy
fi
