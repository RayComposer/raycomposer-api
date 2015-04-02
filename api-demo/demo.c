/* Demo code for using the RayComposer Device API
 * (C) 2015 RayComposer - Robin Adams
 */

#include <stdio.h>
#include <math.h>

#include "rcdev.h"

int main(void){
  int i; /* loop counter */
  int ret; /* return value */
  int count; /* device count */
  char deviceId[256]; /* device id string */
  int handle; /* device handle */
  int frameCounter = 1; /* counter for frames */


  /* Initialise the Library. API Version is returned. */
  ret = RCInit();
  if(ret < 0){
    printf("Error initialising Library! Exit.\n"); return -1;
  }
  if(ret < RCAPI_VERSION){
    printf("API Version too old. Please use a newer dll/lib.\n"); return -2;
  }

  printf("API Version %d.%02d\n", ret >> 8, ret & 0xFF);


  /* Detect & enumerate the connected devices. */
  count = RCEnumerateDevices();
  if(count < 0){
    printf("Error enumerating devices! Exit.\n"); return -3;
  }
  if(count == 0){
    printf("No devices found. Exit.\n"); return 0;
  }

  /* List the devices found. */
  printf("Found %d device(s): \n", count);
  for(i = 0; i < count; i++){

    ret = RCDeviceID(i, deviceId, sizeof(deviceId));
    if(ret < 0){
      printf("Error reading device id! Exit.\n"); return -4;
    }
    printf(" %d: %s\n", i, deviceId);
  }
  printf("\n");


  /* Demo laser output */
  /* Select first device in list. */
  ret = RCDeviceID(0, deviceId, sizeof(deviceId));
  if(ret < 0){
    printf("Error reading device id! Exit.\n"); return -5;
  }

  printf("Opening device: %s\n", deviceId);
  handle = RCOpenDevice(deviceId);
  if(handle < 0){
    printf("Error opening device: %d! Exit.\n", handle); return -6;
  }

  printf("Starting laser.\n");
  ret = RCStartOutput(handle);
  if(ret < RCOk){
    printf("Error starting laser output: %d! Exit.\n", ret); return -7;
  }

  do{
    unsigned int speed = 20000; /* sampling rate */

    /* Your TODO: create fancy laser graphics here */
    struct RCPoint points[200];
    unsigned int pointCount = sizeof(points) / sizeof(struct RCPoint);
    unsigned int i;
    for(i=0; i < pointCount; i++){
      double phi = (double)i * M_PI * 2.0 / count;
      points[i].x = sin(phi) * 32767.5 - 0.5;
      points[i].y = cos(phi) * 32767.5 - 0.5;
      points[i].red = 65535;
      points[i].green = 65535;
      points[i].blue = 65535;
      points[i].intensity = 65535;
      points[i].user1 = 65535;
      points[i].user2 = 65535;
    }

    /* wait for free buffer; second parameter is timeout
     *   0 = poll number of free buffers only, return immediately
     * < 0 = wait forever until buffer becomes free
     * > 0 = wait the number of miliseconds or until a buffer becomes free */
    ret = RCWaitForReady(handle, -1);
    if(ret < RCOk){
      printf("\nError waiting for free buffer: %d! Exit.\n", ret); return -8;
    }
    printf("Showing frame %d.\r", frameCounter);
    ret = RCWriteFrame(handle, points, pointCount, speed, 0);
    if(ret < RCOk){
      printf("\nError writing frame to device: %d! Exit.\n", ret); return -9;
    }
  } while(frameCounter++ < 1000);

  printf("\nStoping laser.\n");
  ret = RCStopOutput(handle);
  if(ret < RCOk){
    printf("Error stoping laser output: %d! Exit.\n", ret); return -10;
  }

  printf("Closing device.\n");
  ret = RCCloseDevice(handle);
  if(ret < RCOk){
    printf("Error closing device: %d! Exit.\n", ret); return -11;
  }


  ret = RCExit();
  if(ret < RCOk){
    printf("Error closing Library: %d! Exit.\n", ret); return -12;
  }

  return 0;
}

