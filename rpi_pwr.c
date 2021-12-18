/* gcc rpi_pwr.c -lwiringPi -O3  -lpthread */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <sys/time.h>
#include <limits.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <syslog.h>

#define IGNORE_CHANGE_BELOW_USEC 10000

struct timeval last_change;
static volatile int reboot_state;
static volatile int shutdown_state;

static int Board_Version = 0;

int g_i2c_fd = -1;

unsigned int temp_limit_high = 40000;
unsigned int temp_limit_low = 15000;
unsigned int warning_voltage_threshold = 4850;

int I2C_ReadBoardVersion(void)
{
    int fd = -1;
    int val[2];

    if((fd = open ("/dev/i2c-0", O_RDWR)) < 0)
    {
        if((fd = open ("/dev/i2c-1", O_RDWR)) < 0)
        {
            printf("Your I2C dev can not access!\n");
            return -1;/* i2c not active */
        }
    }

    close(fd);

    if ((g_i2c_fd = wiringPiI2CSetup (0x52)) < 0)
    {
        //printf("Your I2C connect can not reliable!\n");

        close(g_i2c_fd);
        return -2; /* not device */
    }

    sleep(1);

    if( (val[0] = wiringPiI2CReadReg8 (g_i2c_fd, 0x08)) != 0xA5)
    {
        //printf("Verify your device signature not good!\n");
        close(g_i2c_fd);
        return -3; /* conn bad */
    }

    if( (val[0] = wiringPiI2CReadReg8 (g_i2c_fd, 0x09)) != 0xF0)
    {
        //printf("Verify your device signature not good!\n");

        close(g_i2c_fd);
        return -3; /* conn bad */
    }

    usleep(100 * 1000);

    val[0] = wiringPiI2CReadReg8 (g_i2c_fd, 0x06);
    val[1] = wiringPiI2CReadReg8 (g_i2c_fd, 0x07);

    if(val[0] == -1 || val[1] == -1)
    {
        //printf("Read out Board Version fail,maybe you can decrease your I2C speed!\n");
        close(g_i2c_fd);
        return -4;
    }

    usleep(100 * 1000);

    if(wiringPiI2CWriteReg8 (g_i2c_fd, 0x05, 0x01) < 0)  /* Pi  insert notice */
    {
        //printf("I2C ACK sync fail!\n");
        close(g_i2c_fd);
        return -5;
    }


    printf("Your Extension Board Version:%d \n", (val[0] << 8) | val[1]);
    fflush(stdout);

    return (val[0] << 8) | val[1];
}

void Fan_Speed_Detect(void)
{
    int cpu_temp_fd = -1;
    int cpu_temp = 0;
    //int cpu_temp_origin = 0;
    char temp_buf[128] = {0x00};
    int pwm_value;
    int err = 0;
	static int err_stop = 0;
	
	if(err_stop == 1){
		return;
	}

    cpu_temp_fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
    lseek(cpu_temp_fd, 0, SEEK_SET);
    read(cpu_temp_fd, temp_buf, sizeof(temp_buf));
    cpu_temp = atoi(temp_buf);
    //cpu_temp_origin = cpu_temp;

    if(cpu_temp > temp_limit_high)  /* 40 degC */
    {
        cpu_temp = temp_limit_high;
    }
    else if(cpu_temp < temp_limit_low)   /* 15 degC */
    {
        cpu_temp = temp_limit_low;
    }
    pwm_value = ((float)65535.0 / (float)temp_limit_high) * (float)cpu_temp;

    err += wiringPiI2CWriteReg8 (g_i2c_fd, 0x00, ((pwm_value >> 8) & 0xFF)) ;		/* write pwm */
    usleep(100 * 1000);
    err += wiringPiI2CWriteReg8 (g_i2c_fd, 0x01, pwm_value & 0xFF) ;

    if(err != 0)
    {
		err_stop = 1;
        printf("Fan Speed Modify Error:( %d - due to raspberrypi thermal manager),Reboot your pi and piboxy please.\n", err);
    }

    fflush(stdout);

    close(cpu_temp_fd);
}

void VDD_Detect(void)
{
    int err = 0;
    unsigned int val[2];
    unsigned int voltage = 0;
    static int first_detect = 0;

    usleep(100 * 1000);
    err + wiringPiI2CWriteReg8 (g_i2c_fd, 0x05, 0x02);  /* Pi ADC start conv */
    sleep(1);

    usleep(100 * 1000);
    val[0] = wiringPiI2CReadReg8 (g_i2c_fd, 0x0A);
    usleep(100 * 1000);
    val[1] = wiringPiI2CReadReg8 (g_i2c_fd, 0x0B);
    usleep(100 * 1000);

    if(val[0] != -1 && val[1] != -1)
    {
        voltage = (val[0] << 8) | val[1];

        if(first_detect == 0)
        {
            printf("Power Supply WatchDog Enable. Current Voltage = %d mV\n", voltage);
			first_detect = 1;
        }else if(voltage <= warning_voltage_threshold)
        {
            printf("Your power supply voltage too low,Voltage = %d mV\n", voltage);
        }
    }


    if(err != 0)
    {
        printf("Power Supply Voltage Read Error:%d \n", err);
    }
}

void reboot_handle(void)
{
    struct timeval now;
    unsigned long diff;

    gettimeofday(&now, NULL);

    // Time difference in usec
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000  + last_change.tv_usec);
    // Filter jitter
    if (diff > IGNORE_CHANGE_BELOW_USEC)
    {
        if (reboot_state)
        {
            /* - */
        }
        else
        {
            system("sudo sync");
            system("sudo reboot");
        }

        reboot_state = !reboot_state;
    }

    last_change = now;

}

void shutdown_handle(void)
{

    struct timeval now;
    unsigned long diff;

    gettimeofday(&now, NULL);

    // Time difference in usec
    diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);

	
    // Filter jitter
    if (diff > IGNORE_CHANGE_BELOW_USEC)
    {
        if (shutdown_state)
        {
            /* - */
        }
        else
        {
            system("sudo sync");
            system("sudo shutdown -h now");
        }

        shutdown_state = !shutdown_state;
    }

    last_change = now;


}

int verify_gpio(int gpio)
{
    if(gpio < 0 || gpio > 52)
    {
        return -1;
    }
    else
    {
        return gpio;
    }
}

int main(int argc, char **argv)
{
    int ch;

    int shutdown_gpio = -1;
    int reboot_gpio = -1;
    int notice_gpio = -1;

    int i = 0;
	
    while((ch = getopt(argc, argv, "r:s:n:")) != -1)
        switch(ch)
        {
        case 'r':
            reboot_gpio = verify_gpio(atoi(optarg));
            break;
        case 's':
            shutdown_gpio = verify_gpio(atoi(optarg));
            break;
        case 'n':
            notice_gpio = verify_gpio(atoi(optarg));
            break;
        }

    if(reboot_gpio == -1 || shutdown_gpio == -1 || reboot_gpio == shutdown_gpio)
    {
        printf("Please use:remoteswitch -r [bcm_gpio_num] -s [bcm_gpio_num] -n [bcm_gpio_num]\n")                                                                                                                                                             ;
        printf("-r : Reboot GPIO\n");
        printf("-s : Shutdown GPIO\n");
        printf("-n : Notice GPIO\n");	
        printf("\n Shutdown GPIO must be not the same as Reboot GPIO.Notice GPIO not necessary.\n");

        exit(-1);
    }

    gettimeofday(&last_change, NULL);

    wiringPiSetupGpio();

    if(notice_gpio == -1)
    {
        printf("Notice GPIO not found! \n");
        exit(-2);
    }
    else
    {
        pinMode(notice_gpio, OUTPUT);
        digitalWrite(notice_gpio, 0);
    }

    pinMode(reboot_gpio, OUTPUT);
    pinMode(shutdown_gpio, OUTPUT);

    wiringPiISR(reboot_gpio, INT_EDGE_BOTH, &reboot_handle);
    wiringPiISR(shutdown_gpio, INT_EDGE_BOTH, &shutdown_handle);

    reboot_state = digitalRead(reboot_gpio);
    if(reboot_state != 0)
    {
        printf("Please Check Your reboot_gpio Connect. \n");
        exit(-2);
    }

    shutdown_state = digitalRead(shutdown_gpio);
    if(shutdown_state != 0)
    {
        printf("Please Check Your shutdown_gpio Connect. \n");
        exit(-2);
    }

    for(i = 0; i < 30; i++) /* retry limit */
    {
        printf("Remote Key I2C Synchronizing...(%d/30)\n", i + 1);
        fflush(stdout);
        Board_Version = I2C_ReadBoardVersion();
        if(Board_Version >= 0) break;
        sleep(1);
    }

    if((Board_Version) < 0)
    {
        printf("Working in default mode.Code = %d\n", Board_Version);
        fflush(stdout);
        for(;;)
        {
            sleep(1000);
        }

    }
    else
    {
        printf("Synchronization completed.\n");
        fflush(stdout);
		sleep(1);
        if((Board_Version) == 0) /* rev0 */
        {
            for(;;)
            {
                Fan_Speed_Detect();
                sleep(10);
            }
        }

        if((Board_Version) == 1) /* rev0 + vdd wdt. */
        {
            for(;;)
            {
                VDD_Detect();
                Fan_Speed_Detect();
                sleep(10);
            }
        }
    }


}

