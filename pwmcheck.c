#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include "ping.c"
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

int red, green, blue = 0;

int i, y, u = 0;

time_t now;
struct tm *now_tm;
int hour, minute;

void updateLED(int speed, int redTarget, int greenTarget, int blueTarget) {

	printf("%sredTarget: %d\t red: %d\t%sgreenTarget: %d \tgreen: %d\t%sblueTarget: %d\tblue: %d\t\n", KRED, redTarget, red, KGRN, greenTarget, green, KBLU, blueTarget, blue);

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

// loooooooop
	for (;;) {


		now = time(NULL);
		now_tm = localtime(&now);
		hour = now_tm->tm_hour;
		minute = now_tm->tm_min;

		printf("%sTime: %d:%d\t", KNRM, hour, minute);

		if (!ping("mathsterk.net")) {
			printf("%sWhoops is OK\t", KGRN);
			if (hour > 7 && hour < 21) {
				updateLED(10, 0, 100, 0);
			} else {
				updateLED(10, 0, 10, 0);
			}
			delay(1000);
		} else {
			printf("%sWhoops is NOT OK\t", KRED);
			while (ping("mathsterk.net")) {
				if (hour > 7 && hour < 21) {
					for (i = 0; i < 5; i++) {
						updateLED(10, 100, 0, 0);
						delay(100);
						updateLED(10, 0, 0, 0);
					}
				} else {
					for (i = 0; i < 5; i++) {
						updateLED(10, 20, 0, 0);
						delay(100);
						updateLED(10, 0, 0, 0);
					}
				}
			}
		}
	}

// stuff that should be done when finished
	updateLED(100, 0, 0, 0);
	return 1;
}