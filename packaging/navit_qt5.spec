%define svnver 6011
%define relver 0.5.0

%define navit_dir Navigation/navit_qt5/navit

Name:           navit
Version:        %{relver}.%{svnver}svn
Release:        1%{?dist}
Summary:        Car navigation system with routing engine

Group:          Applications/Productivity
License:        GPL
URL:            http://navit.sourceforge.net/
Source0:        navit-%{version}.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gconf-2.0)
BuildRequires:  pkgconfig(fontconfig)
BuildRequires:  pkgconfig(freetype2)
BuildRequires: 	qt5-plugin-bearer-connman
BuildRequires: 	qt5-plugin-bearer-generic
BuildRequires: 	qt5-plugin-bearer-nm
BuildRequires: 	qt5-plugin-generic-evdev
BuildRequires: 	qt5-plugin-imageformat-gif
BuildRequires: 	qt5-plugin-imageformat-ico
BuildRequires: 	qt5-plugin-imageformat-jpeg
BuildRequires: 	qt5-plugin-platform-eglfs
BuildRequires: 	qt5-plugin-platforminputcontext-ibus
BuildRequires: 	qt5-plugin-platform-linuxfb
BuildRequires: 	qt5-plugin-platform-minimal
BuildRequires: 	qt5-plugin-platform-minimalegl
BuildRequires: 	qt5-plugin-platform-offscreen
BuildRequires: 	qt5-qtdeclarative
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Xml)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5OpenGL)
BuildRequires:  pkgconfig(egl)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:	desktop-file-utils
BuildRequires:  gettext-devel
BuildRequires:  libjpeg-devel
BuildRequires:  python-devel
BuildRequires:  libxslt
BuildRequires:  cmake
BuildRequires:  gpsd-devel

Requires:	dbus
Requires:	dbus-glib

%description
Navit is  modular design is capable of using vector maps of various formats 
for routing and rendering of the displayed map. It's even possible to use 
multiple maps at a time.
The GTK+ or SDL user interfaces are designed to work well with touch screen 
displays. Points of Interest of various formats are displayed on the map.
The current vehicle position is either read from gpsd or directly from NMEA 
GPS sensors.
The routing engine not only calculates an optimal route to your destination,
but also generates directions and even speaks to you using speechd.

%prep
%setup -q -n %{name}-%{version}/navit

# get rid of moc files
find . -name '*.moc' -delete

%build
cmake -DSAMPLE_MAP=n -DCMAKE_INSTALL_PREFIX=%{_prefix} -C %{navit_dir}
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
find %{buildroot} -name '*.la' -exec rm -f {} ';'
%find_lang navit

# installed scaled png files
install -m 644 %{navit_dir}/navit/xpm/*.png $RPM_BUILD_ROOT/%{_datadir}/navit/xpm/
install -m 644 %{navit_dir}/../custom/img/*.png $RPM_BUILD_ROOT/%{_datadir}/navit/xpm/

# install customized ui layout and settings
install -m 644 %{navit_dir}/../custom/navit.xml $RPM_BUILD_ROOT/%{_datadir}/navit/

#install example map
mkdir $RPM_BUILD_ROOT/%{_datadir}/navit/maps
install -m 644 %{navit_dir}/../custom/maps/*.bin $RPM_BUILD_ROOT/%{_datadir}/navit/maps/

%clean
rm -rf %{buildroot}

%files -f navit.lang
%defattr(-,root,root)
%{_bindir}/navit
%{_bindir}/maptool
%dir %{_libdir}/navit
%{_libdir}/navit/*
%{_datadir}/applications/navit.desktop
%{_datadir}/icons/hicolor/22x22/apps/navit.png
%{_datadir}/icons/hicolor/128x128/apps/navit.png
%dir %{_datadir}/navit
%{_datadir}/navit/*
%{_mandir}/man1/*
%{_datadir}/dbus-1/services/*.service
%dir %{_datadir}/locale/uk
%{_datadir}/locale/uk/*

