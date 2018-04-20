## One-Step Automated Install
For those who want to get started quickly and conveniently, you may install Piboxy Driver using the following command for your ***Raspberry Pi***:

#### `curl -sSL piboxy.com/install | bash`

If you use ***recalbox*** as your working system, use the following script, because recalbox uses different GPIO lib, you can also choose batocera instead of recalbox.

#### `curl -sSL piboxy.com/install/recalbox | bash`

If you want to use the ***Tinker Board***, the driver is provided from Maker.  We have only tested the 20170417-tinker-board-linaro-stretch-alip-v1.8 with v1.0(version 0) hardware and you need to install the software before connecting the hardware.

#### `curl -skSL https://git.io/vNugv | bash`

## Beta Images(for Raspberry 3B Plus)
http://piboxy.com/os-images/2018-03-13-raspbian-stretch-with-wiringpi-2.46.zip
http://piboxy.com/os-images/retropie-4.3.15-rpi2_rpi3-with-wiringpi-2.46.zip
## Beta Images(for Recalbox with native I2C + WiringPi Support,Does not support Raspberry 3B Plus)
http://piboxy.com/os-images/recalbox-180420-with-native-wiringpi.zip
## Compatibility

Platform|Version 0(HW V1.0)|Version 1(HW V1.1 - V2.0)|Notes
------------------ | :----------: | :----------: | :---------
Raspberry Pi 3 Model B Plus|x|Part. Tested by Beta Images|Depends on the wiringPi library to work properly.
Raspberry Pi 3 Model B|√|√|
Raspberry Pi 2 Model B |√|√|
Raspberry Pi Model B+|√|x|
Tinker Board |√|×|Only Power On/Off Reboot Function

## Version Table

 - Version 0 : Basic Functions
 - Version 1 Beta : Power Supply Watch Dog + Fan Controler + Version 0
 
 ## Source Code

 - The source code is for reference only, please refer to the manual for more features (released in the future)
