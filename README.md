DS3 Explorer
============

DS3 Explorer is a C++/Qt graphical desktop application for interacting with the
DS3 Deep Storage REST interface.  Windows and Mac OSX are currently supported
although it should work on any Qt compatible platform.

Requirements
------------

* Qt 5.3
* C++ compiler (Windows MSVC, Apple XCode, g++ etc)

Build Instructions
------------------

Building a Qt application is generally the same on all platforms.  It generally
involves using `qmake` to generate Makefiles/project files/etc and then
using your systems native build tools to build the application.  It is
recommended to build within a separate `build` directory to keep the source
directory clean.  This also makes it easy to clean your workspace.

Windows Builds
--------------

The application is currently setup to use Microsoft Visual Studio (Express).
In the Microsoft Visual Studio Developer Command Prompt:

    cd <ds3_explorer directory>
    mkdir build 
    cd build
    qmake ../ds3_explorer.pro
    nmake

Debug and release executables will be located under build/debug and
build/release.

Microsoft Visual Studio can also be used to build/develop the application.
To generate a Microsoft Visual Studio project file:

    cd <ds3_explorer directory>
    mkdir build
    cd build
    qmake -tp vc

Mac OSX Builds
--------------

Install XCode if you have not already done so.  In Terminal:

    cd <ds3_explorer directory>
    mkdir build
    cd build
    qmake ../ds3_explorer.pro
    make

Debug and release app bundles will be located under build/debug and
build/release.

XCode can also be used to build/develop the application.  To generate an
XCode project file:

    cd <ds3_explorer directory>
    mkdir build
    cd build
    qmake -spec macx-xcode ../ds3_explorer.pro

Known Bugs
----------

* qmake does not appear to generate XCode project files correctly when there
  is a space in the .pro's TARGET definition.
