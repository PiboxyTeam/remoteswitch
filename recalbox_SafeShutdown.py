import RPi.GPIO as GPIO
import os
import time
from multiprocessing import Process

#initialize pins
shutdownPin = 18  #BCM mode    
rebootPin = 17  
signalPin = 4  

#initialize GPIO settings
def init():
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(shutdownPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.setup(rebootPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.setup(signalPin, GPIO.OUT)
	GPIO.output(signalPin, GPIO.LOW)
	GPIO.setwarnings(False)

#waits for user to hold button up to 1 second before issuing poweroff command
def poweroff():
	while True:
		GPIO.wait_for_edge(shutdownPin, GPIO.FALLING)
		os.system("sync")
		os.system("shutdown -h now")

#resets the pi
def reset():
	while True:
		GPIO.wait_for_edge(rebootPin, GPIO.FALLING)
		os.system("sync")
		os.system("shutdown -r now")


if __name__ == "__main__":
	#initialize GPIO settings
	init()
	#create a multiprocessing.Process instance for each function to enable parallelism 
	powerProcess = Process(target = poweroff)
	powerProcess.start()
	resetProcess = Process(target = reset)
	resetProcess.start()

	powerProcess.join()
	resetProcess.join()

	GPIO.cleanup()
