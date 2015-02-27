
%define nxe_dir Navigation/NXE

Name:           nxe
Version:        0.1
Release:        1
Summary:        Crosswalk extension to control Navit

Group:          System/Libraries
License:        GPL-3.0
URL:            http://navit.sourceforge.net/
Source0:        %{name}-%{version}.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: 	boost-devel
BuildRequires:  cmake
BuildRequires:  dbus-glib-devel
BuildRequires:  libdbus-c++-devel

Requires:	dbus
Requires:	dbus-glib
Requires:	crosswalk
Requires:	navit

%description 
A simple desc

%prep
%setup -q

%build
cmake %{nxe_dir} -DDISABLE_TESTS=ON -DCMAKE_INSTALL_PREFIX=%{_prefix} 
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%files 
%{_prefix}/lib/tizen-extensions-crosswalk/libnxe.so
%{_prefix}/share/nxe/nxe.conf
