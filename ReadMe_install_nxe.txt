
# Installing for end users

There's a script install.sh in root directory of this repository. 
In order to install latest release, do the following: 

0.) You have to know you Tizen IP. To find this out, one can use /sbin/ifconfig from Tizen target device

1.) Download all rpm files to a folder, let's use ~/tmp/navit_binaries
2.) Necessary RPMs are: 
* gpsd*.rpm
* libdbus-c++*.rpm
* qt5-qtwayland*.rpm
* navit*.rpm
* nxe*.rpm

3.) Once RPM are downloaded invoke the script:
$ ./install.sh --tizen-ip 192.168.41.59 --package-dir ~/tmp/navit_binaries

Where 192.168.41.59 is a Tizen IP 

There's an additional --launch option for install, which will automatically launch NavIt on target device. So command will look like: 

$ ./install.sh --tizen-ip 192.168.41.59 --package-dir ~/tmp/navit_binaries --launch

However in order to use --launch option _app_ user on Tizen device has to have password set up (via passwd command)


# Installing for developers ( GBS is installed)

For developers there's a script flash.sh in root directory of this repo. In order to use this I recommend going through the file, it's easy bash script that's essentially invoking gbs, scp and ssh.  
