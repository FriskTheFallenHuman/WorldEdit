<img align="center" src="https://github.com/FriskTheFallenHuman/WorldEdit/blob/main/install/bitmaps/repository_logo.png?raw=true" alt="WorldEdit Logo">

# WorldEdit

WorldEdit is a level (map) editor for **Doom 3**. Its primary use is idTech4-based games like Doom 3, Quake 4 and Prey.

## Download

Get the latest WorldEdit binaries from the [releases page](https://github.com/FriskTheFallenHuman/WorldEdit/releases). We have binaries for Windows, plus [compilation instructions from the DarkMod wiki](https://wiki.thedarkmod.com/index.php?title=DarkRadiant_-_Compiling_in_Linux) for various Linux distributions.

# Getting started

WorldEdit requires game resources to work with, these resources are not installed by this editor. You'll need to point WorldEdit to one of these games (Doom 3, Quake 4, etc.) before you can start to work on your map.

# Compiling on Windows

## Prerequisites

WorldEdit is built on Windows using *Microsoft Visual Studio*, the free Community Edition can be obtained here:

*VC++ 2022:* https://visualstudio.microsoft.com/downloads/ (Choose Visual Studio Community)

When installing Studio please make sure to enable the "Desktop Development with C++" workload.

## Build

The main Visual C++ solution file is located in the root folder of this repository:

`WorldEdit.sln`

Open this file with Visual Studio and start a build by right-clicking on the top-level 
"Solution 'WorldEdit'" item and choosing Build Solution. The `WorldEdit.exe` file will be placed in the `install/` folder.

### Windows Build Dependencies

Since WorldEdit requires a couple of open-source libraries that are not available on Windows by default, it will try to download and install the dependencies when the build starts. If it fails for some reason, you can try to run this script:

 `tools/scripts/download_windeps.ps1`

or extract the tools manually, downloading the 7-Zip package containing the necessary files from the URL below ([Get 7-zip here](https://www.7-zip.org/)):

https://github.com/FriskTheFallenHuman/WorldEdit_WinDeps/releases/  

The dependencies packages need to be extracted into the main WorldEdit source directory, i.e. alongside the `include/` and `radiant/` directories.
Just drop the windeps.7z in the WorldEdit folder and use 7-zip's "Extract to here"

# Compiling on Linux

## Prerequisites

To compile WorldEdit a number of libraries (with development headers) and a standards-compliant C++17 compiler are required. On an Ubuntu system the requirements may include any or all of the following packages:

* zlib1g-dev 
* libjpeg-dev 
* libwxgtk3.0-dev 
* libxml2-dev 
* libsigc++-2.0-dev 
* libpng-dev 
* libftgl-dev 
* libglew-dev 
* libalut-dev 
* libvorbis-dev
* libgtest-dev
* libeigen3-dev
* libgit2-dev (optional)

To generate the local offline HTML user guide, the `asciidoctor` command must be in your
PATH. This is an optional dependency: if the command is not found, the CMake build will
proceed without building the user guide.

This does not include core development tools such as g++ or the git client to download the
sources (use sudo apt-get install git for that). One possible set of packages might be:

`sudo apt-get install git cmake g++ gettext pkg-config`

More required package lists for various Linux distributions are [listed in the DarkMod Wiki Article](https://wiki.thedarkmod.com/index.php?title=DarkRadiant_-_Compiling_in_Linux).

## Build

To build WorldEdit the standard CMake build process is used:

```
cmake .
make
sudo make install
```

To install somewhere other than the default of `/usr/local`, use the `CMAKE_INSTALL_PREFIX` variable.

```
cmake -DCMAKE_INSTALL_PREFIX=/opt/worldedit
make
sudo make install
```

Other useful variables are `CMAKE_BUILD_TYPE` to choose Debug or Release builds, and `ENABLE_RELOCATION` to control whether WorldEdit uses hard-coded absolute paths like `/usr/lib` or paths relative to the binary (useful for certain package formats like Snappy or FlatPak).

# More Build Information

A more detailed compilation guide can be found on The Dark Mod's wiki:

https://wiki.thedarkmod.com/index.php?title=DarkRadiant_-_Compilation_Guide

# License

Just like DarkRadiant, WorldEdit source code is published under the [GNU General Public License 2.0 (GPLv2)](http://www.gnu.org/licenses/gpl-2.0.html
), except for a few libraries which are using the BSD or MIT licenses, see the [LICENSE](https://raw.githubusercontent.com/codereader/DarkRadiant/master/LICENSE) file for specifics.
