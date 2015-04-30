#!/bin/bash

VERBOSE=false
# it can be also a name of the host
TIZEN_IP="192.168.41.64"
PACKAGE_DIR=
LAUNCH=false

# getopt section
TEMP=`getopt -o v,h,i:,d:,l --long verbose,help,tizen-ip:,package-dir:,launch -n 'flash.sh' -- "$@"`
eval set -- "$TEMP"
while true; do
    case "$1" in
    -v|--verbose)
        shift;
        VERBOSE=true
        ;;
    -l|--launch)
        shift;
        LAUNCH=true
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
    -d|--package-dir)
        PACKAGE_DIR=$2
        shift 2;
        ;;
    --)
        shift;
        break;
        ;;
    esac
done

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

unalias gbs > /dev/null 2>&1

# remove all rpms from target
echo "${red}Cleaning previous installation${reset}"

# clean all previous packages
ssh root@$TIZEN_IP zypper -n -q rm navit 
ssh root@$TIZEN_IP zypper -n -q rm navit-debugsource 
ssh root@$TIZEN_IP zypper -n -q rm navit-debuginfo 
ssh root@$TIZEN_IP zypper -n -q rm gpsd 
ssh root@$TIZEN_IP zypper -n -q rm gpsd-devel 
ssh root@$TIZEN_IP zypper -n -q rm libdbus-c++
ssh root@$TIZEN_IP zypper -n -q rm qt5-qtwayland
ssh root@$TIZEN_IP zypper -n -q rm nxe
ssh root@$TIZEN_IP zypper -n -q rm nxe-debuginfo
ssh root@$TIZEN_IP zypper -n -q rm nxe-debugsource
ssh root@$TIZEN_IP rm /root/navit_packages -rf

try ssh root@$TIZEN_IP mkdir -p /root/navit_packages/
try scp $PACKAGE_DIR/gpsd-*.rpm root@$TIZEN_IP:/root/navit_packages/
try scp $PACKAGE_DIR/libdbus-c++-*.rpm root@$TIZEN_IP:/root/navit_packages/
try scp $PACKAGE_DIR/qt5-qtwayland-*.rpm root@$TIZEN_IP:/root/navit_packages/
try scp $PACKAGE_DIR/navit-*.rpm root@$TIZEN_IP:/root/navit_packages/
try scp $PACKAGE_DIR/nxe-*.rpm root@$TIZEN_IP:/root/navit_packages/

try ssh root@$TIZEN_IP rpm -ivh /root/navit_packages/gpsd*.rpm
try ssh root@$TIZEN_IP rpm -ivh /root/navit_packages/libdbus*.rpm
try ssh root@$TIZEN_IP rpm -ivh /root/navit_packages/qt5-qtwayland*.rpm
try ssh root@$TIZEN_IP rpm -ivh /root/navit_packages/navit*.rpm
try ssh root@$TIZEN_IP rpm -ivh /root/navit_packages/nxe*.rpm

ssh root@$TIZEN_IP rm /root/navit_packages -rf

if [ "$LAUNCH" == true ]; then
    ssh app@$TIZEN_IP XDG_RUNTIME_DIR=/run/user/5000 QT_WAYLAND_DISABLE_WINDOWDECORATION=1 nxe-app
fi
