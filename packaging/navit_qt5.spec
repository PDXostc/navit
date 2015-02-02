%define svnver 6011
%define relver 0.0.1

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
BuildRequires:  pkgconfig(egl)
BuildRequires:  pkgconfig(glesv2)
BuildRequires:  pkgconfig(xpm)
BuildRequires:	desktop-file-utils
BuildRequires:  gettext-devel
BuildRequires:  libjpeg-devel
BuildRequires:  python-devel
BuildRequires:  libxslt
BuildRequires:  cmake

Requires:	dbus
Requires:	dbus-glib
Requires:	gpsd

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

%clean
rm -rf %{buildroot}

%files -f navit.lang
#%files
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

