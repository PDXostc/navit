%define nxe_dir Navigation/NXE

Name:           nxe
Version:        1.4
Release:        1
Summary:        Crosswalk extension to control Navit

Group:          System/Libraries
License:        GPL-3.0
URL:            http://navit.sourceforge.net/
Source0:        %{name}-%{version}.tar.gz
Source1:        nxe.manifest

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: 	boost-devel
BuildRequires: 	tts-devel
BuildRequires:  cmake
BuildRequires:  dbus-glib-devel
BuildRequires:  libdbus-c++-devel
BuildRequires:  gpsd-devel
BuildRequires:  curl-devel
BuildRequires:  qt5-qtwayland
BuildRequires:  qt5-qtwayland-devel
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
BuildRequires:  pkgconfig(Qt5Compositor)
BuildRequires:  pkgconfig(egl)
BuildRequires:  pkgconfig(glesv2)

Requires:	dbus
Requires:	dbus-glib
Requires:	crosswalk
Requires:	navit

%description 
A simple desc

%prep
%setup -q

%build
cmake %{nxe_dir} -DDISABLE_TESTS=ON -DCMAKE_INSTALL_PREFIX=%{_prefix} -DNXE_TARGET_OS=Tizen
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%files 
%defattr(-,root,root,-)
%manifest nxe.manifest
%{_prefix}/lib/libnxe.so
%{_prefix}/share/nxe/nxe.conf
%{_prefix}/share/nxe/osm_maps.xml
%{_prefix}/lib/libfruit.so
%{_prefix}/bin/mapdownloader
%{_prefix}/bin/nxe-app
%{_prefix}/share/dbus-1/services/org.nxe.mapdownloader.service
%{_prefix}/share/applications/org.nxe.navit.desktop
%{_prefix}/bin/nxe-tizen-test
