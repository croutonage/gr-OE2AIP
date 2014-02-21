#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/pa185059/Desktop/Gnuradio/gr-OE2AIP/lib
export PATH=/home/pa185059/Desktop/Gnuradio/gr-OE2AIP/build/lib:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-OE2AIP 
