#include <stdio.h>
#include "ni845x.h"
#include <windows.h>   // Include file for Win32 time functions

/* the NI 845x handles */
NiHandle DeviceHandle;
NiHandle I2CHandle;

/*  error Function for NI 845x */
#ifndef errChk
#define errChk(fCall) if (Error = (fCall), Error < 0) {goto Error;} else
#endif

int main() {

    int    Error            = 0;

    char   FirstDevice[260];       // 260 characters for 845x resource name
    uInt16 ClockRate        = 400;  // clock rate in KHz (1000)
    uInt32 NumberOfDevices  = 0;
    uInt8  PortNumber       = 0;    // port number (0)
    uInt16 ACKTimeout       = 50; // ACK timeout in ms (50)
    uInt16 Address          = 0x60; // I2C address 7bit (0x60)

    uInt32 WriteSize       = 0;
    uInt8  SendData[515];          // array for total bytes to write
    uInt32 ReadSize        = 0;
    uInt8  ReadData[35];           // read array for bytes to read

    uInt8  Delay           = 1;    // delay time between pages
    uInt8  i;

    char   ErrorMsg[1024];


    printf("\n\nSearching for Devices\n\n");
    /* find first device */
    errChk (ni845xFindDevice (FirstDevice, NULL , &NumberOfDevices));
    printf("Found %d device(s) on the USB. \n", NumberOfDevices);

    /* open device handle */
    errChk (ni845xOpen (FirstDevice, &DeviceHandle));
    printf("Found following device to initialize: %s \n", FirstDevice);

    /* Set the I/O Voltage Level */
    errChk (ni845xSetIoVoltageLevel (DeviceHandle, kNi845x33Volts));

    /* create configuration reference */
    errChk (ni845xI2cConfigurationOpen (&I2CHandle));
   
    /* configure configuration properties */
    errChk (ni845xI2cConfigurationSetPort (I2CHandle, PortNumber));
    errChk (ni845xI2cConfigurationSetAckPollTimeout (I2CHandle, ACKTimeout));
    errChk (ni845xI2cConfigurationSetAddress (I2CHandle, Address));
    errChk (ni845xI2cConfigurationSetAddressSize (I2CHandle, kNi845xI2cAddress7Bit));
    errChk (ni845xI2cConfigurationSetClockRate (I2CHandle,ClockRate));

    printf (" ....initialized successfully\n\n");

    WriteSize = 2;
    SendData[0] = 0x00;
    SendData[1] = 0x01;
    errChk (ni845xI2cWrite (DeviceHandle, I2CHandle, WriteSize, SendData));
    printf("Write %d bytes: ", WriteSize);
    for (i = 0; i < WriteSize; i++)    {
        printf("0x%02X ", SendData[i]);
    }

    WriteSize = 1;
    SendData[0] = 0x00;
    errChk (ni845xI2cWriteRead (DeviceHandle, I2CHandle, WriteSize,
        SendData, 2, &ReadSize, ReadData));
    printf("Read %d bytes: ", ReadSize);
    for (i = 0; i < ReadSize; i++)    {
        printf("0x%02X ", ReadData[i]);
    }
    getchar();

Error:
   if (Error < 0)
   {
      printf ("\n-- Error Handring --\n");
      ni845xStatusToString(Error, 1024, ErrorMsg);
      printf ("\nError %d %s \n", Error, ErrorMsg);
      ni845xI2cConfigurationClose (I2CHandle);
      ni845xClose (DeviceHandle);
      exit(1);
   }
    return 0;
}