## One-Step Automated Install
For those who want to get started quickly and conveniently, you may install Piboxy Driver using the following command for your ***Raspberry Pi***:

#### `curl -sSL piboxy.com/install | bash`

If you want to use the ***Tinker Board***, the driver is provided from Maker.  We have only tested the 20170417-tinker-board-linaro-stretch-alip-v1.8 with v1.0(version 0) hardware and you need to install the software before connecting the hardware.

#### `curl -skSL https://git.io/vNugv | bash`


## Compatibility

Platform|Version 0(HW V1.0)|Version 1 Beta(HW V1.1)|Notes
------------------ | :----------: | :----------: | :---------
Raspberry Pi 3 Model B|√|√|
Raspberry Pi 2 Model B |√|wip|
Raspberry Pi Model B+|√|wip|
Tinker Board |√|×|Only Power On/Off Reboot Function

## Version Table

 - Version 0 : Basic Functions
 - Version 1 Beta : Power Supply Watch Dog + Fan Controler + Version 0
