#!/bin/sh

TIZEN_IP="192.168.41.64"
NXE_VERSION=0.1.1
NAVIT_VERSION=
GBS_ROOT="$HOME/GBS-ROOT"

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

unalias gbs > /dev/null 2>&1

# remove all rpms from target
echo "${red}Cleaning previous installation${reset}"
ssh root@$TIZEN_IP rm /root/navit* -rf
ssh root@$TIZEN_IP rm /root/nxe* -rf

# remove nxe and navit
ssh root@$TIZEN_IP zypper -n -q rm nxe > /dev/null 2>&1
ssh root@$TIZEN_IP zypper -n -q rm navit > /dev/null 2>&1

# build navit
echo "${red}Building navit${reset}"
try gbs build -A i586 --spec navit_qt5.spec --include-all > /dev/null 2>&1
try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-0.5.0.6011svn-1.i686.rpm root@192.168.41.64:/root
try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-debuginfo-0.5.0.6011svn-1.i686.rpm root@192.168.41.64:/root
try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/navit-debugsource-0.5.0.6011svn-1.i686.rpm root@192.168.41.64:/root

# build nxe
echo "${red}Building nxe${reset}"
try gbs build -A i586 --spec nxe.spec --include-all > /dev/null 2>&1
try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/nxe-0.1-1.i686.rpm root@192.168.41.64:/root
try scp $GBS_ROOT/local/BUILD-ROOTS/scratch.i586.0/home/abuild/rpmbuild/RPMS/i686/nxe-debugsource-0.1-1.i686.rpm root@192.168.41.64:/root

# install both
try ssh root@$TIZEN_IP rpm -ivh /root/navit*
try ssh root@$TIZEN_IP rpm -ivh /root/nxe*

# example
cd Navigation/NXE/exampleapp
try ./make_xpk.sh app ~/mykey.pem
try scp app.xpk root@$TIZEN_IP:/home/app
try ssh root@$TIZEN_IP chown app /home/app/app.xpk
