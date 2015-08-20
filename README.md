Deep Storage Browser
====================

Deep Storage Browser is a C++/Qt graphical desktop application for interacting
with the Deep Storage REST interface.  Windows and Mac OSX are currently
supported although it should work on any Qt compatible platform.

BlackPearl Compatibility
------------------------

This version of the Deep Storage Browser is only compatible with BlackPearl
version 1.2.

Build Requirements
------------------

* [Qt 5.3](http://qt-project.org)
* [DS3 C SDK](https://github.com/SpectraLogic/ds3_c_sdk)
* C++ compiler (Microsoft Visual Studio, Apple XCode, GCC, etc)

Build Instructions
------------------

Building a Qt application is generally the same on all platforms.  It generally
involves using `qmake` to generate Makefiles/project files/etc and then
using your system's native build tools to build the application.  It is
recommended to build within a separate `build` directory to keep the source
directory clean.  This also makes it easy to clean your workspace.

Windows Builds
--------------

The application is currently setup to use Microsoft Visual Studio (Express).
Build the [DS3 C SDK](https://github.com/SpectraLogic/ds3_c_sdk).
In the Microsoft Visual Studio Developer Command Prompt:

    cd <deep_storage_browser directory>
    mkdir build 
    cd build
    qmake ../deep_storage_browser.pro
    copy <ds3_c_sdk directory>\src\ds3.h .\
    copy <ds3_c_sdk directory>\win32\output\lib\*.lib .\
    copy <ds3_c_sdk directory>\win32\output\bin\*.dll .\release
    mkdir curl
    copy <ds3_c_sdk directory>\win32\deps\install\curl .\curl
    copy <ds3_c_sdk directory>\win32\deps\install\include\zconf.h .\
    copy <ds3_c_sdk directory>\win32\deps\install\include\zlib.h .\
    copy <ds3_c_sdk directory>\win32\deps\install\lib\zlib_a.lib .\
    nmake release

A release executable will be located under build/release.  Running `nmake`
without any targets specified will build the debug version to build/debug.

Microsoft Visual Studio can also be used to build/develop the application.
To generate a Microsoft Visual Studio project file:

    cd <deep_storage_browser directory>
    mkdir build
    cd build
    qmake -tp vc ../deep_storage_browser.pro

Mac OSX Builds
--------------

Install XCode if you have not already done so.  Build and install the
[DS3 C SDK](https://github.com/SpectraLogic/ds3_c_sdk) and it's dependencies.
Then, in Terminal:

    cd <deep_storage_browser directory>
    mkdir build
    cd build
    qmake ../deep_storage_browser.pro
    make release

A release app bundle will be located under build/release.  Running `make`
without any targets specified will build the debug version to build/debug.

XCode can also be used to build/develop the application.  To generate an
XCode project file:

    cd <deep_storage_browser directory>
    mkdir build
    cd build
    qmake -spec macx-xcode ../deep_storage_browser.pro

Packaging and Deploying
-----------------------

The easiest way to package up and deploy the application is to build the
application and deploy all of its dependencies (Qt libraries, C SDK, etc) along
with the app.  Qt provides `macdeloyqt`, `windeployqt`, etc tools to make this
easy.  They are located in your Qt's bin directory (where `qmake` is located).
Just run the appropriate `deployqt` tool, for your host platform, and point it
to where your built executable is; on OSX, point it to the .app bundle.
On OSX, it copies all the dependencies to the .app bundle.  On Windows, it
copies all the dependencies to the directory where the .exe file is.

Session Persistence
-------------------

The application is currently setup to automatically save the session data
if the user selects that option.  Where the data is saved depends on the
platform.  On OSX, it is stored in
~/Library/Preferences/com.spectralogic.Deep Storage Browser.plist.

OSX Note - OSX Yosemite, and possibly earlier versions, cache the plist files.
Thus, deleting the plist file is not enough to clear the setting.  You must
then either restart, logout/login, or execute `killall -u yourusername cfprefsd`

TODO - Update for Windows

Running Tests
-------------

Qt's unit test framework is used for testing.  A separate `test` console
application is provided under the `test/` directory that will run all the
tests.  To run the tests, build and run the test application like any
other Qt application.

    cd <deep_storage_browser directory>/test
    mkdir build
    cd build
    qmake ../test.pro
    make
    ./test
