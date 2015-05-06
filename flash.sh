#!/bin/bash

VERBOSE=false
# it can be also a name of the host
TIZEN_IP="192.168.41.64"
GBS_ROOT="$HOME/GBS-ROOT"
BUILD_NAVIT=true
BUILD_NXE=true

# getopt section
TEMP=`getopt -o v,h,i:,g:,n,x --long verbose,help,tizen-ip:,gbs-root:,no-navit,no-nxe -n 'flash.sh' -- "$@"`
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
redirect() {
    if [ "$VERBOSE" = true ]; then
        "$@"
    else
        "$@" > /dev/null 2>&1
    fi
}

unalias gbs > /dev/null 2>&1

# remove all rpms from target
echo "${red}Cleaning previous installation${reset}"

if [ "$BUILD_NAVIT" = true ]; then
    # clean navi
    redirect ssh root@$TIZEN_IP zypper -n -q rm navit 
    redirect ssh root@$TIZEN_IP zypper -n -q rm navit-debugsource 
    redirect ssh root@$TIZEN_IP zypper -n -q rm navit-debuginfo
    redirect ssh root@$TIZEN_IP rm /root/navit* -rf

    # build navit

    echo "${red}Building navit${reset}"
    redirect try gbs build -A i586 --spec navit_qt5.spec --include-all --keep-packs

    redirect try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-0.5.0.6011svn-1.i686.rpm root@$TIZEN_IP:/root
    redirect try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-debuginfo-0.5.0.6011svn-1.i686.rpm root@$TIZEN_IP:/root
    redirect scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-debugsource-0.5.0.6011svn-1.i686.rpm root@$TIZEN_IP:/root
    redirect try ssh root@$TIZEN_IP rpm -ivh /root/navit*
fi

if [ "$BUILD_NXE" = true ]; then

    # clean
    redirect try ssh root@$TIZEN_IP rm /root/nxe* -rf
    redirect ssh root@$TIZEN_IP zypper -n -q rm nxe 
    redirect ssh root@$TIZEN_IP zypper -n -q rm nxe-debugsource 
    redirect ssh root@$TIZEN_IP zypper -n -q rm nxe-debuginfo 

    # build nxe
    echo "${red}Building nxe${reset}"
    redirect try gbs build -A i586 --spec nxe.spec --include-all --keep-packs

    redirect try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/nxe-*.rpm root@$TIZEN_IP:/root
    redirect try ssh root@$TIZEN_IP rpm -ivh /root/nxe*
fi
