#!/bin/bash

VERBOSE=false
# it can be also a name of the host
GBS_ROOT="$HOME/GBS-ROOT"

BUILD_DIR=$HOME/tmp/navit/releases
RELEASE_VERSION=

# getopt section
TEMP=`getopt -o v,h,r:,b: --long verbose,help,release:,builddir -n 'flash.sh' -- "$@"`
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
    -r|--release)
        RELEASE_VERSION=$2
        shift 2;
        ;;
    -b|--builddir)
        BUILD_DIR=$2
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

if [ "$RELEASE_VERSION" = "" ]; then
    die "No release version"
fi

if [ -d "$BUILD_DIR/$RELEASE_VERSION" ]; then
    rm $BUILD_DIR/$RELEASE_VERSION -rfv
fi

mkdir -p $BUILD_DIR/$RELEASE_VERSION

echo "${red}Build directory = $BUILD_DIR/$RELEASE_VERSION${reset}"


# build navit

echo "${red}Building navit${reset}"
if [ "$VERBOSE" = true ]; then
    try gbs build -A i586 --spec navit_qt5.spec --include-all --keep-packs
else
    try gbs build -A i586 --spec navit_qt5.spec --include-all --keep-packs > /dev/null 2>&1
fi

cp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-*.rpm $BUILD_DIR/$RELEASE_VERSION

# build nxe
echo "${red}Building nxe${reset}"
if [ "$VERBOSE" = true ]; then
    try gbs build -A i586 --spec nxe.spec --include-all --keep-packs
else
    try gbs build -A i586 --spec nxe.spec --include-all --keep-packs > /dev/null 2>&1
fi

cp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/nxe-*.rpm $BUILD_DIR/$RELEASE_VERSION

# copy rest
cp Navigation/packages/gpsd-3.11\~dev-0.i686.rpm $BUILD_DIR/$RELEASE_VERSION
cp Navigation/packages/libdbus-c++-1.0.0-1.i686.rpm $BUILD_DIR/$RELEASE_VERSION
cp Navigation/packages/qt5-qtwayland-5.4.0-0.i686.rpm $BUILD_DIR/$RELEASE_VERSION
