#pragma once

#include <Windows.h>
#include "stdio.h"
#include <stdlib.h>
#include <malloc.h>
#include "resource.h"


#define MAX_DEVICE_INFO_LEN     2048
#define MAX_STR_LEN             1024


// In DeviceDetector.h

typedef struct {
    
    char comment[MAX_STR_LEN];
    char totalBytes[MAX_STR_LEN];
    char deviceInstanceID[MAX_DEVICE_INFO_LEN];     //acts as Unique ID
    char devicePath[MAX_DEVICE_INFO_LEN];
    char friendlyName[MAX_DEVICE_INFO_LEN];
    char manufacturer[MAX_DEVICE_INFO_LEN];
    char deviceDescription[MAX_DEVICE_INFO_LEN];
    char classGuid[MAX_DEVICE_INFO_LEN];
    
    
} DeviceInfo, *PDeviceInfo ;

typedef struct {
    char firstName[MAX_STR_LEN];
    char lastName[MAX_STR_LEN];
    char emailID[MAX_STR_LEN];
    char userName[MAX_STR_LEN];
    char password[MAX_STR_LEN];
} AdminInfo, *PAdminInfo;



