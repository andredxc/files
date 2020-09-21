/*
  continuous.c: Reads a pontenciometer, a light sensor, a sound sensor and a
  temperature sensor on Galileo Gen2 ADC_AD0, ADC_AD1, ADC_AD2 e ADC_AD3 in
  continuous mode.

  Copyright (c) 2016 Walter Fetter Lages <w.fetter@ieee.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    You can also obtain a copy of the GNU General Public License
    at <http://www.gnu.org/licenses>.

*/

#include <byteswap.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libgalileo2.h"

#define DATA_POINTS 1000
#define SAMPLING_PERIOD 1e-3

/* Assumes the data format for Galileo Gen2 */
struct sensors
{
        uint16_t pot;		/* be:u12/16>>0 */
        uint16_t light;		/* be:u12/16>>0 */
        uint16_t sound;		/* be:u12/16>>0 */
        uint16_t temp;		/* be:u12/16>>0 */
        int64_t timestamp;	/* le:s64/64>>0 */
};

int main(int argc,char * argv[])
{
        char data_str[80];
        double scale[4];
        int fd;
        static struct sensors data[DATA_POINTS];
        int i;
        int samples;
        FILE *file;
        char path_str[80];

        if(argc != 2)
        {
                puts("Usage: continuous <output_file>\n");
                return -1;
        }

        pputs("/sys/bus/iio/devices/iio:device0/buffer/enable","0");
        for(i=0;i < 4;i++)
        {
                snprintf(path_str,sizeof path_str,"/sys/bus/iio/devices/iio:device0/in_voltage%d_scale",i);
                pgets(data_str,sizeof data_str,path_str);
                scale[i]=atof(data_str)/1000.0;

                snprintf(path_str,sizeof path_str,"/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage%d_en",i);
                pputs(path_str,"1");
        }
        pputs("/sys/bus/iio/devices/iio:device0/scan_elements/in_timestamp_en","1");

        snprintf(data_str,sizeof data_str,"%d",DATA_POINTS);
        pputs("/sys/bus/iio/devices/iio:device0/buffer/length",data_str);

#ifdef TRIG_SYSFS
        pgets(data_str,sizeof data_str,"/sys/bus/iio/devices/trigger0/name");
        pputs("/sys/bus/iio/devices/iio:device0/trigger/current_trigger",data_str);
#else
        pgets(data_str,sizeof data_str,"/sys/bus/iio/devices/trigger1/name");
        pputs("/sys/bus/iio/devices/iio:device0/trigger/current_trigger",data_str);

        snprintf(data_str,sizeof data_str,"%d",(int)round(1.0/SAMPLING_PERIOD));
        pputs("/sys/bus/iio/devices/trigger1/frequency",data_str);
#endif

        pputs("/sys/bus/iio/devices/iio:device0/buffer/enable","1");

#ifdef TRIG_SYSFS
        for(i=0; i < DATA_POINTS;i++)
        {
                pputs("/sys/bus/iio/devices/trigger0/trigger_now","1");
                usleep(ceil(SAMPLING_PERIOD*1e6));
        }
#else
        sleep(ceil(DATA_POINTS*SAMPLING_PERIOD));
#endif

        pputs("/sys/bus/iio/devices/iio:device0/buffer/enable","0");

        pputs("/sys/bus/iio/devices/iio:device0/trigger/current_trigger","\n");

        if((fd=open("/dev/iio:device0",O_RDONLY)) < 0)
        {
                perror("Opening /dev/iio:device0:");
                return -1;
        }

        samples=read(fd,data,sizeof data)/sizeof(struct sensors);
        close(fd);

        pputs("/sys/bus/iio/devices/iio:device0/buffer/length","2");

        for(i=0;i < 4;i++)
        {
                snprintf(path_str,sizeof path_str,"/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage%d_en",i);
                pputs(path_str,"0");
        }
        pputs("/sys/bus/iio/devices/iio:device0/scan_elements/in_timestamp_en","0");

        if((file=fopen(argv[1],"w")) == NULL)
        {
                perror("Opening output file:");
                return -1;
        }

        for(i=0;i < samples;i++)
        {
                data[i].pot=bswap_16(data[i].pot);
                data[i].light=bswap_16(data[i].light);
                data[i].sound=bswap_16(data[i].sound);
                data[i].temp=bswap_16(data[i].temp);

                fprintf(file,"%f\t%f\t%f\t%f\t%f\n",
                        data[i].pot*scale[0],
                        data[i].light*scale[1],
                        data[i].sound*scale[2],
                        data[i].temp*scale[3],
                        (data[i].timestamp-data[0].timestamp)*1e-9);
        }
        fclose(file);

        return 0;
}