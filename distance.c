/*
 ***********************************************************************
 *  sonic.c - a test program for the HC-SR04 Ranging detector module
 *  connected to the GPIOs of Raspberry Pi using the pigpio-API
 *
 *  Copyright (C) 2013 Dreamshader (Dirk Schanz)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 ***********************************************************************
 */

// gcc  -o distance distance.c -I /usr/local/include -L /usr/local/lib -lpigpio -lrt -lpthread

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "pigpio.h"
#define TRIGGER 18 
#define ECHO 17

void ping()
{
	long begin, end, signaltime;
        int timeout;
        long tmval;

	printf("ping\n");
	/* send 10 uSeconds burst to start measuring */
	gpioWrite(TRIGGER, 1); /* sets gpio24 high */
	gpioDelay(10);
	gpioWrite(TRIGGER, 0); /* sets gpio24 high */
	timeout = 0;
	tmval = gpioTick();
	begin = gpioTick();

	while( !gpioRead (ECHO) )
        {
		begin = gpioTick();
		if( (gpioTick() - tmval) >= 5000000 )
                {
			printf("timeout 1!\n");
			timeout = 1;
			break;
                }
		gpioDelay(5);	/* allow CPU to do other jobs */
        }

        if( !timeout )
        {
		while( gpioRead (ECHO) )
		{
			end = gpioTick();
			if( (gpioTick() - tmval) >= 5000000 )
                	{
				printf("timeout 2!\n");
				timeout = 1;
				break;
                	}
			gpioDelay(5);	/* allow CPU to do other jobs */
		}

        	if( !timeout )
        	{
			signaltime = end - begin;
			//      343 meters/second
			// =  34300 cm/second
			// = 343000 mm/second
			// =    343 mm/msecond
			// diff is in usecond
			// distance in mm = signaltime * 343 / 1000 / 2
			printf("distance is %d mm\n", signaltime*3403/20000);
			// round trip time / 1000000.0 * 34030
		}
	}

}

//
// main loop
//

main()
{

	if( gpioInitialise() > 0 )
	{
		gpioSetMode(ECHO, PI_INPUT);		/* set gpio23 as input */
		gpioSetMode(TRIGGER, PI_OUTPUT);		/* set gpio18 as output */
		gpioSetPullUpDown(ECHO, PI_PUD_OFF);	/* clear pull-ups/downs on gpio23 */ 
		gpioSetPullUpDown(TRIGGER, PI_PUD_OFF);	/* clear pull-ups/downs on gpio24 */ 

	
		for( ;; )
		{
			ping();
			gpioSleep(PI_TIME_RELATIVE, 1, 0);

	
		}
		gpioTerminate();	/* cleanup library */
	}
}

