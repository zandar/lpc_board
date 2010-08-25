#!/bin/sh

BINORTEMANAGER=ortemanager
BINJOYD=joyd
BINJOYSTICK=joystick
ORTEMANAGER=$(ps -e | grep $BINORTEMANAGER | grep -v grep)
JOYD=$(ps -e | grep $BINJOYD | grep -v grep)

start_ortemanager()
{
    if [ -z "$ORTEMANAGER" ]; then
        echo "Ortemanager is not running. I'm starting it now!"
        $BINORTEMANAGER -p 10.1.1.1 > /dev/null & 
    else
        echo "ORTEManager is already running."
    fi
}

start_joyd()
{
    if [ -z "$JOYD" ]; then
        echo "Starting joyd!"
        $BINJOYD > /dev/null &
    else
        echo "Joyd is running. Restarting it now!"
        killall $BINJOYD
        $BINJOYD > /dev/null &
    fi
}

case "$1" in
    stop)
	killall $BINJOYD
	;;
    stopall)
	killall $BINORTEMANAGER
	killall $BINJOYD
	;;
    start)
	start_ortemanager
	start_joyd
	;;
    *)
	start_ortemanager
	start_joyd
esac


