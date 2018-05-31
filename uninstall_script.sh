#!/bin/bash

echo "Simple Clean Piboxy Driver..."

sudo rm /bin/remoteswitch 
sudo sed -e '/remoteswitch/d' /etc/rc.local

echo "Simple Clean Piboxy Driver Done!"
