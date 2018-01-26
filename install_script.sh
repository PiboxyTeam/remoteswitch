#!/bin/bash
#

hostname=$(hostname)
if [[ "$hostname" == "BATOCERA" ]];then

echo "Install BATOCERA Piboxy Driver..."

gpio readall > /dev/null 2>&1

if [ $? -eq 0 ]; then 

mkdir -p /recalbox/share/system/bin/
curl http://piboxy.com/download/remoteswitch > /recalbox/share/system/bin/remoteswitch
chmod a+x /recalbox/share/system/bin/remoteswitch
echo "/recalbox/share/system/bin/remoteswitch -r 17 -s 18 -n 4 &" > /recalbox/share/system/custom.sh
chmod a+x /recalbox/share/system/custom.sh

mount -o remount -rw /boot

if grep -q "^dtparam=i2c_arm=off" /boot/config.txt; then
	sed -i "s|^dtparam=i2c_arm=off|dtparam=i2c_arm=on|" /boot/config.txt &> /dev/null
elif grep -q "^#dtparam=i2c_arm=on" /boot/config.txt; then
	sed -i "s|^#dtparam=i2c_arm=on|dtparam=i2c_arm=on|" /boot/config.txt &> /dev/null
else
	echo "dtparam=i2c1=on" | tee -a /boot/config.txt &> /dev/null
	echo "dtparam=i2c_arm=on" | tee -a /boot/config.txt &> /dev/null
fi


if grep -q "^vc_i2c_override=0" /boot/cmdline.txt; then
	sed -i "s|^vc_i2c_override=0|vc_i2c_override=1|" /boot/cmdline.txt &> /dev/null
else
	echo -n "bcm2708.vc_i2c_override=1" >> /boot/cmdline.txt
fi
sync
mount -o remount -r /boot

if ! [ -e /etc/modprobe.d/raspi-blacklist.conf ]; then
touch /etc/modprobe.d/raspi-blacklist.conf
fi
sed /etc/modules.conf -i -e "s/^#[[:space:]]*\(i2c[-_]dev\)/\1/"
if ! grep -q "^i2c[-_]dev" /etc/modules.conf ; then
printf "i2c-dev\n" >> /etc/modules.conf
printf "i2c_bcm2708\n" >> /etc/modules.conf
fi

/recalbox/scripts/recalbox-save-overlay.sh

modprobe i2c-dev
modprobe i2c_bcm2708
sync
/recalbox/share/system/bin/remoteswitch -r 17 -s 18 -n 4 &

echo "It will be reboot later to finish install."
sync
sleep 2
reboot
else 

echo "Your batocera wiringpi can not be detects your Raspberry Pi model,If you think it is a BUG, please contact us.";
echo "=================================================="
gpio readall
echo "=================================================="
exit

fi

else

echo "Install Piboxy Driver..."

sudo apt-get update
sudo apt-get install -qq wiringpi 
sudo sh -c " curl http://piboxy.com/download/remoteswitch > /bin/remoteswitch "
sudo chmod a+x /bin/remoteswitch
sudo sed -i '$i sudo remoteswitch -r 17 -s 18 -n 4 &' /etc/rc.local
sudo sync

#Enable I2C

if grep -q "^dtparam=i2c_arm=off" /boot/config.txt; then
	sudo sed -i "s|^dtparam=i2c_arm=off|dtparam=i2c_arm=on|" /boot/config.txt &> /dev/null
elif grep -q "^#dtparam=i2c_arm=on" /boot/config.txt; then
	sudo sed -i "s|^#dtparam=i2c_arm=on|dtparam=i2c_arm=on|" /boot/config.txt &> /dev/null
else
	echo "dtparam=i2c_arm=on" | sudo tee -a /boot/config.txt &> /dev/null
fi

if ! [ -e /etc/modprobe.d/raspi-blacklist.conf ]; then
sudo touch /etc/modprobe.d/raspi-blacklist.conf
fi
sudo sed /etc/modprobe.d/raspi-blacklist.conf -i -e "s/^\(blacklist[[:space:]]*i2c[-_]bcm2708\)/#\1/"
sudo sed /etc/modules -i -e "s/^#[[:space:]]*\(i2c[-_]dev\)/\1/"
if ! grep -q "^i2c[-_]dev" /etc/modules; then
sudo printf "i2c-dev\n" >> /etc/modules
fi
sudo dtparam i2c_arm=on
sudo modprobe i2c-dev

sudo sync
sudo remoteswitch -r 17 -s 18 -n 4 &

fi
