all:
	gcc -o remoteswitch rpi_pwr.c -lwiringPi -O3 -lpthread

clean:
	rm remoteswitch

run:
	sudo ./remoteswitch -r 17 -s 18 -n 4
