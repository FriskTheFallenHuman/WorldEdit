Name:		worldedit
Version:	0.9.12
Release:	2%{?dist}
Summary:	Level editor for Doom 3 
Group:		Applications/Editors
License:	GPLv2 and LGPLv2 and BSD
URL:		https://github.com/FriskTheFallenHuman/WorldEdit
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:	automake, autoconf, libtool, desktop-file-utils

%description
 World is a 3D level editor for Doom 3, based on the GPL
 release of DarkRadiant.

%prep
%setup -q

%build
%configure --enable-debug --prefix=/usr
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
desktop-file-install					\
  --dir=${RPM_BUILD_ROOT}%{_datadir}/applications	\
  ${RPM_BUILD_ROOT}%{_datadir}/applications/worldedit.desktop

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc README
%{_bindir}/*
%{_libdir}/worldedit/lib*
%{_libdir}/worldedit/modules
%{_libdir}/worldedit/scripts
%{_datadir}/*

%changelog
* Tue Mar 26 2009 ibix <ibixian@gmail.com> - 0.9.12-2
- patches upstream. Removed here.

* Tue Mar 24 2009 ibix <ibixian@gmail.com> - 0.9.12-1
- spec file.
- patch for sound detection on fedora.
- patch for valid desktop entry.

