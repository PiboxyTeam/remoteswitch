#!/bin/bash
#Step1) remount boot and root partition make writable.
mount -o remount, rw /boot
mount -o remount, rw /

#Step 3) Download Python script
mkdir -p  /opt/PiBoxy
sleep 2s

script=/opt/PiBoxy/recalbox_SafeShutdown.py

if [ -ne  $script ];
        then
        wget --no-check-certificate -O  $script "https://raw.githubusercontent.com/PiboxyTeam/remoteswitch/recalbox/recalbox_SafeShutdown.py"
fi
#-----------------------------------------------------------

#Step 4) Enable Python script to run on start up------------
DIR=/etc/init.d/S99PiBoxy

if grep -q "python $script &" "S99PiBoxy";
        then
                if [ -x $DIR];
                        then 
                                echo "Executable S99PiBoxy already configured. Do nothing."
                        else
                                chmod +x $DIR
                fi
        else
                echo "python $script &" >> $DIR
                chmod +x $DIR
                echo "Executable S99PiBoxy configured."
fi
#-----------------------------------------------------------

#Step 5) Reboot to apply changes----------------------------
echo "PiBoxy Remote Power Switch Driver's installation is accomplished . Will now reboot after  5 seconds."
for  i in `seq 0 5 |sort -rn `;
 do 
    echo  "$i  Sec"
    sleep  1
done  
shutdown -r now

