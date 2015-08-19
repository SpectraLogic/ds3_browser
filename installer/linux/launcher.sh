#!/bin/sh
pwd=`pwd`
export LD_LIBRARY_PATH=$pwd/lib:$LD_LIBRARY_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH=$pwd/lib/plugins
./app/Deep\ Storage\ Browser
