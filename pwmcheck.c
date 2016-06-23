// include everything, EVERYTHING
#include <wiringPi.h>
#include <softPwm.h>
#include <pthread.h>
#include "ping.c"
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jansson.h>


// COLORS :D
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// http://192.168.1.6:8013/render?target=alias(scaleToSeconds(nonNegativeDerivative(scale(nav.devices.Tomaten.ports.eth0.ifInOctets,8)),1),%22In%22)&format=json&from=-30min
#define URL_FORMAT   "http://%s:%s/render?target=alias(scaleToSeconds(nonNegativeDerivative(scale(nav.devices.%s.ports.%s.ifInOctets,8)),1),\%22In\s%22)&format=json&from=-%s"
// host, port, hostname, port, time

// #define URL_FORMAT   "https://api.github.com/repos/%s/%s/commits"
#define URL_SIZE     256


// ints for rgb values
int red, green, blue = 0;

// ints for for for for for for looooooops (yes, stoopid names)
int i = 0;

// time stuff
time_t now;
struct tm *now_tm;
int hour, minute;

/* Return the offset of the first newline in text or the length of
   text if there's no newline */
static int newline_offset(const char *text)
{
	const char *newline = strchr(text, '\n');
	if (!newline)
		return strlen(text);
	else
		return (int)(newline - text);
}

// updateLED(speed of change, red, green, blue)
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

		softPwmWrite(0, 100 - red);
		softPwmWrite(2, 100 - green);
		softPwmWrite(3, 100 - blue);

		delay(speed);
	}
}

// MURDER!
void sigint(int a)
{
	updateLED(2, 0, 0, 0);
	abort();
}

int speed(/* FIGURE OUT SHIT */) {
	size_t y;
	char *text;
	char url[URL_SIZE];

	json_t *root;
	json_error_t error;

	if (argc != 3)
	{
		fprintf(stderr, "usage: %s USER REPOSITORY\n\n", argv[0]);
		fprintf(stderr, "List commits at USER's REPOSITORY.\n\n");
		return 2;
	}

	snprintf(url, URL_SIZE, URL_FORMAT, argv[1], argv[2]);

	text = request(url);
	if (!text)
		return 1;

	root = json_loads(text, 0, &error);
	free(text);

	if (!root)
	{
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
		return 1;
	}

	if (!json_is_array(root))
	{
		fprintf(stderr, "error: root is not an array\n");
		json_decref(root);
		return 1;
	}
/*
	[
    {
        "sha": "<the commit ID>",
        "commit": {
            "message": "<the commit message>",
            <more fields, not important to this tutorial...>
        },
        <more fields...>
    },
    {
        "sha": "<the commit ID>",
        "commit": {
            "message": "<the commit message>",
            <more fields...>
        },
        <more fields...>
    },
    <more commits...>
]
*/


/*
[
	{"target": "In",
	 "datapoints": [
	 	[null, 1466634000],
	 	[11667056.293333335, 1466634300],
	 	[15363236.853333334, 1466634600],
	 	[11810406.4, 1466634900],
	 	[10556079.253333334, 1466635200],
	 	[null, 1466635500]
	 	]
	}
]
*/


	    for (i = 0; i < json_array_size(root); i++)
	{
		json_t *data, *sha, *commit, *message;
		const char *message_text;

		data = json_array_get(root, i);
		if (!json_is_object(data))
		{
			fprintf(stderr, "error: commit data %d is not an object\n", i + 1);
			json_decref(root);
			return 1;
		}

		sha = json_object_get(data, "sha");
		if (!json_is_string(sha))
		{
			fprintf(stderr, "error: commit %d: sha is not a string\n", i + 1);
			json_decref(root);
			return 1;
		}

		commit = json_object_get(data, "commit");
		if (!json_is_object(commit))
		{
			fprintf(stderr, "error: commit %d: commit is not an object\n", i + 1);
			json_decref(root);
			return 1;
		}

		message = json_object_get(commit, "message");
		if (!json_is_string(message))
		{
			fprintf(stderr, "error: commit %d: message is not a string\n", i + 1);
			json_decref(root);
			return 1;
		}

		message_text = json_string_value(message);
		printf("%.8s %.*s\n",
		       json_string_value(id),
		       newline_offset(message_text),
		       message_text);
	}
}

int main (void) {
	// "setup"
	wiringPiSetup();
	softPwmCreate(0, 100, 100);
	softPwmCreate(2, 100, 100);
	softPwmCreate(3, 100, 100);
	signal(SIGINT, sigint);
	// end of "setup"


	// loooooooop
	for (;;) {

		// time lord stuff
		now = time(NULL);
		now_tm = localtime(&now);
		hour = now_tm->tm_hour;
		minute = now_tm->tm_min;

		printf("%sTime: %d:%d\t", KNRM, hour, minute);

		if (!ping("mathsterk.net")) {
			printf("%sWhoops is OK\t", KGRN);
			if (hour > 7 && hour < 21) {
				updateLED(100, 0, 100, 0);
			} else {
				updateLED(100, 0, 7, 0);
			}
			delay(10000);
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
	return 0;
}