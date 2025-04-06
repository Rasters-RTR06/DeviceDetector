#include "DeviceDetector.h"

#define MAX_DEVICE_PATH 512
#define BLOCKED_LIST_FILE "blocked_devices.json"

typedef struct
{
    char deviceID[MAX_DEVICE_PATH];
    char description[MAX_DEVICE_PATH];
} USBDevice;

USBDevice *blockedDevices = NULL;
int blockedCount = 0;

// Updated JSON handling
void LoadBlockedList(USBDevice **blockedDevices, int *blockedCount)
{
    FILE *fp = fopen(BLOCKED_LIST_FILE, "r");
    if (!fp)
        return;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = (char *)malloc(size + 1);
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    cJSON *json = cJSON_Parse(buffer);
    if (json)
    {
        *blockedCount = cJSON_GetArraySize(json);
        *blockedDevices = (USBDevice *)malloc(*blockedCount * sizeof(USBDevice));

        for (int i = 0; i < *blockedCount; i++)
        {
            cJSON *item = cJSON_GetArrayItem(json, i);
            cJSON *id = cJSON_GetObjectItem(item, "deviceID");
            cJSON *desc = cJSON_GetObjectItem(item, "description");
            if (id && desc)
            {
                strcpy_s((*blockedDevices)[i].deviceID, MAX_DEVICE_PATH, cJSON_GetStringValue(id));
                strcpy_s((*blockedDevices)[i].description, MAX_DEVICE_PATH, cJSON_GetStringValue(desc));
            }
        }
        cJSON_Delete(json);
    }
    free(buffer);
}

void SaveBlockedList(USBDevice *blockedDevices, int blockedCount)
{
    cJSON *json = cJSON_CreateArray();
    for (int i = 0; i < blockedCount; i++)
    {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "deviceID", blockedDevices[i].deviceID);
        cJSON_AddStringToObject(item, "description", blockedDevices[i].description);
        cJSON_AddItemToArray(json, item);
    }

    FILE *fp = fopen(BLOCKED_LIST_FILE, "w");
    if (fp)
    {
        char *str = cJSON_Print(json);
        fprintf(fp, "%s", str);
        free(str);
        fclose(fp);
    }
    cJSON_Delete(json);
}

// Updated Add/Remove functions
void AddToBlockedList(USBDevice **blockedDevices, int *blockedCount, USBDevice device)
{
    *blockedDevices = (USBDevice *)realloc(*blockedDevices, (*blockedCount + 1) * sizeof(USBDevice));
    (*blockedDevices)[*blockedCount] = device;
    (*blockedCount)++;
    SaveBlockedList(*blockedDevices, *blockedCount);
}

void RemoveFromBlockedList(USBDevice **blockedDevices, int *blockedCount, const char *deviceID)
{
    for (int i = 0; i < *blockedCount; i++)
    {
        if (strcmp((*blockedDevices)[i].deviceID, deviceID) == 0)
        {
            for (int j = i; j < *blockedCount - 1; j++)
            {
                (*blockedDevices)[j] = (*blockedDevices)[j + 1];
            }
            (*blockedCount)--;
            *blockedDevices = (USBDevice *)realloc(*blockedDevices, *blockedCount * sizeof(USBDevice));
            SaveBlockedList(*blockedDevices, *blockedCount);
            break;
        }
    }
}

void testAddingBlockedDevice()
{
    USBDevice device;
    char description[] = "New usb device - pendrive 32GiB";
    char deviceID[] = "usb-device-1";
    int i = 0;
    while (description[i] != '\0')
    {
        device.description[i] = description[i];
        i++;
    }
    device.description[i] = '\0';
    i = 0;
    while (deviceID[i] != '\0')
    {
        device.deviceID[i] = deviceID[i];
        i++;
    }
    device.deviceID[i] = '\0';

    AddToBlockedList(&blockedDevices, &blockedCount, device);
}
