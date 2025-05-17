#!/bin/sh

export THIS_DIR=$(pwd)

export LD_LIBRARY_PATH="$THIS_DIR:/customer/lib:/lib:/lib:/config/lib:/mnt/SDCARD/miyoo/lib:/mnt/SDCARD/.tmp_update/lib:/mnt/SDCARD/.tmp_update/lib/parasyte"
cd $THIS_DIR
PID=$(pidof MainUI)
echo $PID
kill -STOP $PID
./ZAG2000
kill -CONT $PID