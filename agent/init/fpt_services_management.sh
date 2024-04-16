#!/bin/sh
fw_setenv mfg_mode

source /etc/profile

cp -r /app/default/TZ /var/conf/

udhcpc -s /app/bin/udhcpc.sh -i eth0 -p /var/run/eth0.pid &
start_ntpdate > /dev/null 2>&1 &

sleep 10

amixer -Dhw:0 cset name='Rear Amic Capture Volume' 75
amixer -Dhw:0 cset name='Master Playback Volume' 120

echo 5000 > /proc/sys/vm/min_free_kbytes

APP_FCAM="/app/bin/fcam"

echo Firmware is Release ------------------
if [ -e $APP_FCAM ]; then
    # LD_LIBRARY_PATH=/app/lib/ $APP_FCAM > /dev/null 2>&1
    LD_LIBRARY_PATH=/app/lib/ $APP_FCAM & echo $$! > /var/run/fcam.pid
else
    echo "file $APP_FCAM not exist"
fi
