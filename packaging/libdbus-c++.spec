
%define dbus_cplusplus_dir Navigation/3rdParty/dbus-cplusplus

Summary:	C++ Interface for DBus
Name:		libdbus-c++
Version:	1.0.0
Release:	1
URL:		http://dev.openwengo.org/trac/openwengo/trac.fcgi/browser/wengophone-ng/branches/wengophone-dbus-api/libs/dbus/src
Source0:	%{name}-%{version}.tar.gz
License:	LGPL
Group:		Libraries
BuildRoot:	%{_tmppath}/%{name}-root
Prefix:		/usr

BuildRequires: 	pkgconfig(dbus-1)
BuildRequires: 	pkgconfig(ecore)
BuildRequires: 	pkgconfig(expat)
BuildRequires: 	dbus-devel
BuildRequires: 	dbus-glib-devel
BuildRequires:  pkgconfig(dbus-glib-1)

%description

Ability to reflect dbus methods and signals into a more natural C++ object system.



%package devel
Requires:	libdbus-c++ = %{version}
Group:		Development/Libraries
Summary:	Header files for libdbus-c++

%description devel
Header files for libdbus-c++


%prep
%setup -q 

%build
cd %{dbus_cplusplus_dir}
./autogen.sh
./configure --prefix=/usr
make -j 4

%install
cd %{dbus_cplusplus_dir}
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(755,root,root)
%{prefix}/lib/libdbus-c++*.so*



%files devel
%defattr(-,root,root)
%{prefix}/bin/dbusxx-xml2cpp
%{prefix}/bin/dbusxx-introspect
%{prefix}/lib/libdbus-c*.a
%{prefix}/lib/libdbus-c*.la
%{prefix}/include/dbus-c++-1
%{prefix}/lib/pkgconfig/*.pc

