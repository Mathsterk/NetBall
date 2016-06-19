#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include "ping.c"
#include <signal.h>
#include <stdio.h>

int red, green, blue = 0;

int i, y, u = 0;


void updateLED(int speed, int redTarget, int greenTarget, int blueTarget) {

	printf("redTarget: %d\tred: %d\tgreenTarget: %d\tgreen: %d\tblueTarget: %d\tblue: %d\t\n", redTarget, red, greenTarget, green, blueTarget, blue);

	while (red != redTarget || green != greenTarget || blue != blueTarget) {
		if (red > redTarget) {
			red--;
		} else if (red < redTarget) {
			red++;
		}
		if (green > greenTarget) {
			green--;
		} else if (green < greenTarget) {
			green++;
		}

		if (blue > blueTarget) {
			blue--;
		} else if (blue < blueTarget) {
			blue++;
		}

		// printf("red: %d\tgreen: %d\tblue: %d\n", red, green, blue);
		softPwmWrite(0, 100 - red);
		softPwmWrite(2, 100 - green);
		softPwmWrite(3, 100 - blue);

		delay(speed);
	}
}


void sigint(int a)
{
	updateLED(2, 0, 0, 0);
	abort();
}

int main (void) {
	// "setup"
	wiringPiSetup();
	//  pinMode (0, pwm) ;
	softPwmCreate(0, 100, 100);
	softPwmCreate(2, 100, 100);
	softPwmCreate(3, 100, 100);
	signal(SIGINT, sigint);
	// end of "setup"

	// if (ping("www.google.com"))
	// 	printf("Ping is not OK. \n");
	// else
	// 	printf("Ping is OK. \n");

// loooooooop
	for (;;) {
		if (!ping("google.com")) {
			printf("Google is OK\n");
			updateLED(10, 0, 100, 0);
			delay(60000);
		} else {
			printf("Google is NOT OK\n");
			while (ping("google.com")) {
				for (i = 0; i < 5; i++) {
					updateLED(10, 100, 0, 0);
					delay(100);
					updateLED(10, 0, 0, 0);
				}
			}

		}

	}
// stuff that should be done when finished
	updateLED(100, 0, 0, 0);
	return 1;
}