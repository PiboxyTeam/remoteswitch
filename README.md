## One-Step Automated Install
Those who want to get started quickly and conveniently, may install Piboxy Driver using the following command for ***Raspberry Pi***:

#### `curl -sSL piboxy.com/install | bash`

If you want to use the ***Tinker Board***, driver is provided form Maker, we tested only by 20170417-tinker-board-linaro-stretch-alip-v1.8 with v1.0(version 0) hardware, and you need to install the software before connecting the hardware.

#### `curl -skSL https://git.io/vNugv | bash`


## Compatibility

Platform|Version 0|Version 1 Beta|Notes
------------------ | :----------: | :----------: | :---------
Raspberry Pi 3 Model B|√|√|
Raspberry Pi 2 Model B |√|-|
Raspberry Pi Model B+|√|-|
Tinker Board |√||Only Power On/Off Reboot Function

√ = Fully Test
- = Partially Test
* = Fully Test but not have all function
× = Not compatible

## Version Table

 - Version 0 : Basic Function
 - Version 1 Beta : Power Supply Watch Dog + Fan Controler + Version 0
