
# Installing nxe crosswalk plugin and application

# Copy all needed rpms to /root
cd /root
rpm -ivh gpsd-3.11~dev-0.i686.rpm
rpm -ivh navit-0.5.0.6011svn-1.i686.rpm
zypper in libboost_filesystem_1_57_0
rpm -ivh libdbus-c++-1.0.0-1.i686.rpm
rpm -ivh nxe-0.1-1.i686.rpm

# now install app as user `app`
su - app
cd /home/app
pkgcmd -i -t xpk -p app.xpk -q
# lauch application
xwalk-launcher `xwalkctl | grep simple_extension_app | awk '{print $1}'`
