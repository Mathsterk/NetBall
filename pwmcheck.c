#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include "ping.c"

boolean whoops, google, optiplex, router = false;



int i, y, u = 0;
int main (void)
{
	// "setup"
	wiringPiSetup () ;
	//  pinMode (0, pwm) ;
	softPwmCreate (0, 255, 255) ;
	// end of "setup"

	// if (ping("www.google.com"))
	// 	printf("Ping is not OK. \n");
	// else
	// 	printf("Ping is OK. \n");




	unsigned int rgbColour[3];

	// Start off with red.
	rgbColour[0] = 255;
	rgbColour[1] = 0;
	rgbColour[2] = 0;

	// Choose the colours to increment and decrement.
	for (int decColour = 0; decColour < 3; decColour += 1) {
		int incColour = decColour == 2 ? 0 : decColour + 1;

		// cross-fade the two colours.
		for (int i = 0; i < 255; i += 1) {
			rgbColour[decColour] -= 1;
			rgbColour[incColour] += 1;

			updateLED(rgbColour[0], rgbColour[1], rgbColour[2]);
			delay(5);
		}
	}


	
}

void updateLED(int redLED, int greenLED, int blueLED) {
	softPwmWrite(0, 255 - redLED);
	softPwmWrite(2, 255 - greenLED);
	softPwmWrite(3, 255 - blueLED);
}

// return 0 or 1