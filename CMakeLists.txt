cmake_minimum_required(VERSION 3.15)

# Project name and version
project(WorldEdit VERSION 4.0.0)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GCC 8 and earlier require explicit linking against stdc++fs
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
	link_libraries(stdc++fs)
endif()

# Expose some build options
option(ENABLE_RELOCATION
       "Avoid hard-coded absolute paths to libraries or resources"
       ON)

# Define GNU-style directory structure by default
include(GNUInstallDirs)

# Set up core build paths
set(CORE_MODULE_LIBRARY "libcore")
set(PKGDATADIR "${CMAKE_INSTALL_FULL_DATADIR}/worldedit")
set(PKGLIBDIR "${CMAKE_INSTALL_FULL_LIBDIR}/worldedit")
if (${ENABLE_RELOCATION})
    set(RELATIVE_LIBDIR "../${CMAKE_INSTALL_LIBDIR}/worldedit")
else()
    set(HTMLDIR "${CMAKE_INSTALL_FULL_DATADIR}/doc/worldedit")
endif()

# Build shared libraries by default
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
if (${ENABLE_RELOCATION})
    set(CMAKE_INSTALL_RPATH "$ORIGIN/${RELATIVE_LIBDIR}")
else()
    set(CMAKE_INSTALL_RPATH "${PKGLIBDIR}")
endif()

# Debug or release mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_definitions(
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Locate system packages
include(FindPkgConfig)
pkg_check_modules(SIGC sigc++-2.0 REQUIRED)
pkg_check_modules(FTGL ftgl REQUIRED)
pkg_check_modules(FREETYPE freetype2 REQUIRED)
pkg_check_modules(GL gl REQUIRED)
pkg_check_modules(GLEW glew REQUIRED)
pkg_check_modules(JPEG libjpeg REQUIRED)
pkg_check_modules(PNG libpng REQUIRED)
pkg_check_modules(AL openal REQUIRED)
pkg_check_modules(OGG ogg REQUIRED)
pkg_check_modules(VORBIS vorbisfile REQUIRED)
pkg_check_modules(X11 x11 REQUIRED)
pkg_check_modules(ZLIB zlib REQUIRED)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
pkg_check_modules(EIGEN eigen3 REQUIRED)

# Locate a packaged pugixml, if installed if not, fall back to embedded code copy.
find_package(pugixml QUIET)
if (TARGET pugixml::pugixml)
    message(NOTICE "Distribution pugixml found and used.")
else()
    include_directories(libs/pugixml)
endif()

# Locate wxWidgets
find_package(wxWidgets REQUIRED
             COMPONENTS base core stc adv gl xrc aui)
include(${wxWidgets_USE_FILE})

# Locate Python
find_package(Python REQUIRED COMPONENTS Development)

# Global includes and flags
include_directories(libs include)
add_compile_options(${SIGC_CFLAGS})
add_compile_definitions(POSIX
                        WXINTL_NO_GETTEXT_MACRO
                        FMT_HEADER_ONLY
                        HAVE_STD_FILESYSTEM)
add_link_options(LINKER:-z,defs)

# Generate config.h
configure_file(config.h.in config.h)
add_compile_definitions(HAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Supporting libraries
add_subdirectory(libs/math)
add_subdirectory(libs/xmlutil)
add_subdirectory(libs/scene)
add_subdirectory(libs/wxutil)
add_subdirectory(libs/module)

# Mandatory modules
add_subdirectory(plugins/script)
add_subdirectory(plugins/sound)

pkg_check_modules(LIBGIT libgit2)
if (${LIBGIT_FOUND})
    add_subdirectory(plugins/vcs)
endif()

# Main radiant components
add_subdirectory(radiantcore)
add_subdirectory(radiant)

# Tests
pkg_check_modules(GTEST gtest)
pkg_check_modules(GTEST_MAIN gtest_main)
if (${GTEST_FOUND} AND ${GTEST_MAIN_FOUND})
    enable_testing()
    add_subdirectory(test)
endif()

# Documentation
find_program(ASCIIDOCTOR asciidoctor)
if (ASCIIDOCTOR)
    add_subdirectory(doc)
else()
    message(WARNING "asciidoctor not found; documentation will not be generated")
endif()

# Install main targets
install(TARGETS worldedit math xmlutil scene wxutil
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${PKGLIBDIR})
install(TARGETS radiantcore script sound
        LIBRARY DESTINATION ${PKGLIBDIR}/modules)

if (${LIBGIT_FOUND})
    install(TARGETS vcs
            LIBRARY DESTINATION ${PKGLIBDIR}/plugins)
endif()

# Generate and install the .desktop file
configure_file(install/worldedit.desktop.in install/worldedit.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/install/worldedit.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

# Install resources
file(GLOB XML_FILES "${PROJECT_SOURCE_DIR}/install/*.xml")
install(FILES ${XML_FILES} DESTINATION ${PKGDATADIR})

install(DIRECTORY install/games DESTINATION ${PKGDATADIR})
install(DIRECTORY install/bitmaps DESTINATION ${PKGDATADIR})
install(DIRECTORY install/gl DESTINATION ${PKGDATADIR})
install(DIRECTORY install/ui DESTINATION ${PKGDATADIR}
        FILES_MATCHING PATTERN "*.ttf" PATTERN "*.xrc")
install(DIRECTORY install/resources DESTINATION ${PKGDATADIR})

install(FILES ${PROJECT_SOURCE_DIR}/install/bitmaps/worldedit_icon_64x64.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps
        RENAME net.worldedit.WorldEdit.png)
install(FILES ${PROJECT_SOURCE_DIR}/install/bitmaps/worldedit_icon_128x128.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps
        RENAME net.worldedit.WorldEdit.png)
install(FILES ${PROJECT_SOURCE_DIR}/install/net.worldedit.WorldEdit.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

# Install locale data
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14")
# CMake 3.14 and above support TYPE LOCALE, they deduct DESTINATION themselves
install(DIRECTORY install/i18n/de TYPE LOCALE
        FILES_MATCHING PATTERN "*.mo")
else()
install(DIRECTORY install/i18n/de DESTINATION ${CMAKE_INSTALL_LOCALEDIR}
        FILES_MATCHING PATTERN "*.mo")
endif()

# Install scripts
install(DIRECTORY install/scripts DESTINATION ${PKGDATADIR}
        FILES_MATCHING PATTERN "*.py")
