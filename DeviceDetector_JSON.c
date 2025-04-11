#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include "DeviceDetector.h"
#include "Headers/DeviceDetector_JSON.h"
#include "cjson/cJSON.h"

#define JSON_FILE "devices.json"

FILE* gpConfigFile = NULL;
BOOL bConfigFileOpen = FALSE;
char* gConfigData = NULL;
int gIsWriteNeeded = 0;

//Example 
/********************************************************************
    readDevices();    // This is for read whole file

**************************************************************** 
    This is for How to add Device  

    addDevice("USB Device", "Pendrive", "32GB");
    addDevice("Monitor", "LG", "1080p, 60Hz");
    addDevice("Monitor", "Samsung", "4K, 144Hz");
    addDevice("Bluetooth Device", "Mouse", "Wireless");
    addDevice("Audio Device", "Headphone", "Stereo, Noise Cancelling");
    
   
/****************************************************************
    char category[50], name[50], config[100];
    char* deviceInfo = getDeviceByID(DEVICE_LG_MONITOR);  // Here we are using ENUM Value
    parseDeviceString(deviceInfo, category, (unsigned int)sizeof(category),
                     name, (unsigned int)sizeof(name),
                     config, (unsigned int)sizeof(config));
*****************************************************************
    printf("Category: %s\n", category);
    printf("Name: %s\n", name);
    printf("Configuration: %s\n", config);
*****************************************************************/


void readDevices() 
{
    FILE* file;
    errno_t err = fopen_s(&file, JSON_FILE, "r");
    if (err != 0 || file == NULL) 
    {
        printf("No existing JSON file found. Creating a new one.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length == 0) 
    {
        printf("JSON file is empty. No devices found.\n");
        fclose(file);
        return;
    }

    char* json_data = (char*)malloc(length + 1);
    fread(json_data, 1, length, file);
    fclose(file);
    json_data[length] = '\0';

    cJSON* root = cJSON_Parse(json_data);
    free(json_data);

    if (!root) 
    {
        printf("Error: JSON file is corrupted or invalid.\n");
        return;
    }

    char* formatted_json = cJSON_Print(root);
    printf("\nCurrent Devices:\n%s\n", formatted_json);

    free(formatted_json);
    cJSON_Delete(root);
}


void addDevice(const char* category, const char* deviceName, const char* config) 
{
    FILE* file;
    cJSON* root = NULL;

    errno_t err = fopen_s(&file, JSON_FILE, "r");
    if (err == 0 && file != NULL) 
    {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (length > 0) 
        {
            char* json_data = (char*)malloc(length + 1);
            fread(json_data, 1, length, file);
            json_data[length] = '\0';
            root = cJSON_Parse(json_data);
            free(json_data);
        }
        fclose(file);
    }

    if (!root) 
    {
        root = cJSON_CreateObject();
    }


    cJSON* categoryArray = cJSON_GetObjectItemCaseSensitive(root, category);
    if (!cJSON_IsArray(categoryArray)) 
    {

        categoryArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root, category, categoryArray);
    }


    cJSON* newDevice = cJSON_CreateObject();
    cJSON_AddStringToObject(newDevice, "name", deviceName);
    cJSON_AddStringToObject(newDevice, "config", config);


    cJSON_AddItemToArray(categoryArray, newDevice);


    err = fopen_s(&file, JSON_FILE, "w");
    if (err != 0 || file == NULL) 
    {
        printf("Error: Unable to write to JSON file.\n");
        cJSON_Delete(root);
        return;
    }

    char* json_str = cJSON_Print(root);
    fputs(json_str, file);
    fclose(file);
    free(json_str);
    cJSON_Delete(root);

    printf("Device added successfully: %s - %s\n", deviceName, config);
}
char* getDeviceByID(int deviceID) 
{
    FILE* file;
    errno_t err = fopen_s(&file, JSON_FILE, "r");
    if (err != 0 || file == NULL) 
    {
        return _strdup("Error: JSON file not found.");
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length == 0) 
    {
        fclose(file);
        return _strdup("JSON file is empty. No devices found.");
    }

    char* json_data = (char*)malloc(length + 1);
    fread(json_data, 1, length, file);
    fclose(file);
    json_data[length] = '\0';

    cJSON* root = cJSON_Parse(json_data);
    free(json_data);

    if (!root) 
    {
        return _strdup("Error: JSON file is corrupted or invalid.");
    }

    int currentID = 0;
    cJSON* category = NULL;
    cJSON_ArrayForEach(category, root) 
    {
        cJSON* device = NULL;
        cJSON_ArrayForEach(device, category) 
        {
            if (currentID == deviceID) 
            {
                cJSON* name = cJSON_GetObjectItemCaseSensitive(device, "name");
                cJSON* config = cJSON_GetObjectItemCaseSensitive(device, "config");
                if (cJSON_IsString(name) && cJSON_IsString(config)) 
                {
                    char* result = (char*)malloc(256);
                    snprintf(result, 256, "Device Found (ID: %d)\n- Category: %s\n- Name: %s\n- Configuration: %s",
                        deviceID, category->string, name->valuestring, config->valuestring);
                    cJSON_Delete(root);
                    return result;
                }
            }
            currentID++;
        }
    }

    cJSON_Delete(root);
    return _strdup("Error: No device found with the given ID.");
}

void parseDeviceString(const char* input, char* category, size_t cat_size, char* name, size_t name_size, char* config, size_t config_size) 
{
    char* context = NULL;
    char temp[256];  
    strcpy_s(temp, sizeof(temp), input);

    char* line = strtok_s(temp, "\n", &context);  
    while (line != NULL) 
    {
        if (strstr(line, "- Category:") != NULL)
            sscanf_s(line, "- Category: %49s", category, (unsigned int)cat_size);
        else if (strstr(line, "- Name:") != NULL)
            sscanf_s(line, "- Name: %49s", name, (unsigned int)name_size);
        else if (strstr(line, "- Configuration:") != NULL)
            sscanf_s(line, "- Configuration: %99[^\n]", config, (unsigned int)config_size);  

        line = strtok_s(NULL, "\n", &context);
    }
}

#pragma region ConfigFile OpenClose

void OpenConfigFile(char *readmode)
{
    errno_t err = fopen_s(&gpConfigFile, JSON_FILE, readmode);
    if (err != 0 || gpConfigFile == NULL)
    {
        printf("No existing JSON file found. Creating a new one.\n");
        return;
    }

    if (strcmp(readmode, "a+") == 0)
    {
        gIsWriteNeeded = 1;
    }
    else
    {
        gIsWriteNeeded = 0;
    }

    DecryptConfigFile();
    bConfigFileOpen = TRUE;
}

void DecryptConfigFile()
{
    fseek(gpConfigFile, 0, SEEK_END);
    long length = ftell(gpConfigFile);
    fseek(gpConfigFile, 0, SEEK_SET);

    
    gConfigData = (char*)malloc(length + 1);
    fread(gConfigData, 1, length, gpConfigFile);
    gConfigData[length] = '\0';
    // decrypt config file here
}

void EncryptConfigFile(int writeToFile)
{
    // encrypt config file here
    if (writeToFile == 1)
    {
        fputs(gConfigData, gpConfigFile);
    }
    free(gConfigData);
    gConfigData = NULL;
}

void CloseConfigFile()
{
    if (bConfigFileOpen == TRUE)
    {
        EncryptConfigFile(gIsWriteNeeded);
        fclose(gpConfigFile);
        gpConfigFile = NULL;
    }
}

#pragma endregion

#pragma region UserData

void* ReadAdminDetails()
{
    PAdminInfo aInfo = NULL;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        aInfo = (PAdminInfo)malloc(sizeof(AdminInfo));
        memset(aInfo, 0, sizeof(AdminInfo));
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjAdmin = cJSON_GetObjectItemCaseSensitive(cObjRoot, "Admin");

        if (cJSON_IsObject(cObjAdmin) == 0) 
        {
            cJSON_Delete(cObjRoot);
            CloseConfigFile();
            free(aInfo);
            
            aInfo = NULL;
            return NULL;
        }
        else
        {
            cJSON* fName = cJSON_GetObjectItem(cObjAdmin, "firstName");
            cJSON* lName = cJSON_GetObjectItem(cObjAdmin, "lastName");
            cJSON* eMail = cJSON_GetObjectItem(cObjAdmin, "emailID");
            cJSON* uName = cJSON_GetObjectItem(cObjAdmin, "userName");
            cJSON* pWd = cJSON_GetObjectItem(cObjAdmin, "password");

            strcpy(aInfo->firstName, cJSON_GetStringValue(fName));
            strcpy(aInfo->lastName, cJSON_GetStringValue(lName));
            strcpy(aInfo->emailID, cJSON_GetStringValue(eMail));
            strcpy(aInfo->userName, cJSON_GetStringValue(uName));
            strcpy(aInfo->password, cJSON_GetStringValue(pWd));
        }
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return (void *)aInfo;
}

int AddAdminDetails(void* adminInfo)
{
    OpenConfigFile("a+");;
    if (bConfigFileOpen == TRUE)
    {
        AdminInfo* aInfo = (AdminInfo*)adminInfo;
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        if (!cObjRoot)
        {
            cObjRoot = cJSON_CreateObject();
        }

        cJSON* newAdmin = cJSON_CreateObject();
        cJSON_AddStringToObject(newAdmin, "firstName", aInfo->firstName);
        cJSON_AddStringToObject(newAdmin, "lastName", aInfo->lastName);
        cJSON_AddStringToObject(newAdmin, "emailID", aInfo->emailID);
        cJSON_AddStringToObject(newAdmin, "userName", aInfo->userName);
        cJSON_AddStringToObject(newAdmin, "password", aInfo->password);

        cJSON_AddItemToObject(cObjRoot, "Admin", newAdmin);

        char* json_str = cJSON_Print(cObjRoot);
        
        if (json_str == NULL)
        {
            cJSON_Delete(cObjRoot);
            CloseConfigFile();
            return -1; 
        }

        free(gConfigData);
        gConfigData = NULL;

        int jsonLen = strlen(json_str);

        /*FILE* s = NULL;
        errno_t err = fopen_s(&s, "xx.json", "w");
        if (err != 0 || s == NULL)
        {
            MessageBox(NULL, TEXT("xx not Found"), TEXT("Validation Error"), MB_OK);
            return;
        }
        fputs(json_str, s);
        fclose(s);*/
        
        gConfigData = (char*)malloc(jsonLen + 1);
        strcpy(gConfigData, json_str);

        free(json_str);
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return 0;
}
#pragma endregion

#pragma region DeviceInfo

int AddDeviceToDeviceInfoList(void* deviceInfo)
{
    OpenConfigFile("a+");;
    if (bConfigFileOpen == TRUE)
    {
        DeviceInfo* dInfo = (DeviceInfo*)deviceInfo;
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjDeviceArray = cJSON_GetObjectItemCaseSensitive(cObjRoot, "DeviceList");
        if (!cJSON_IsArray(cObjDeviceArray) && cJSON_IsNull(cObjDeviceArray))
        {
            cObjDeviceArray = cJSON_CreateArray();
            cJSON_AddItemToObject(cObjRoot, "DeviceList", cObjDeviceArray);
        }

        cJSON* newDevice = cJSON_CreateObject();
        cJSON_AddStringToObject(newDevice, "comment", dInfo->comment);
        cJSON_AddStringToObject(newDevice, "totalBytes", dInfo->totalBytes);
        cJSON_AddStringToObject(newDevice, "deviceInstanceID", dInfo->deviceInstanceID);
        cJSON_AddStringToObject(newDevice, "devicePath", dInfo->devicePath);
        cJSON_AddStringToObject(newDevice, "friendlyName", dInfo->friendlyName);
        cJSON_AddStringToObject(newDevice, "manufacturer", dInfo->manufacturer);
        cJSON_AddStringToObject(newDevice, "deviceDescription", dInfo->deviceDescription);
        cJSON_AddStringToObject(newDevice, "classGuid", dInfo->classGuid);

        cJSON_AddItemToArray(cObjDeviceArray, newDevice);

        char* json_str = cJSON_Print(cObjRoot);
        
        if (json_str == NULL)
        {
            cJSON_Delete(cObjRoot);
            CloseConfigFile();
            return -1;
        }

        free(gConfigData);
        gConfigData = NULL;

        int jsonLen = strlen(json_str);

        gConfigData = (char*)malloc(jsonLen + 1);
        strcpy(gConfigData, json_str);

        free(json_str);
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return 0;
}

int DeviceInfoListLength()
{
    int deviceListLen = 0;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjDeviceList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "DeviceList");

        if (!cJSON_IsArray(cObjDeviceArray) && cJSON_IsNull(cObjDeviceArray))
        {

        }
        else
        {
            deviceListLen = cJSON_GetArraySize(cObjDeviceList);
        }
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return deviceListLen;
}

void* GetDeviceInfoItemFromIndex(int index)
{
    PDeviceInfo dInfo = NULL;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        dInfo = (PDeviceInfo)malloc(sizeof(DeviceInfo));
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjDeviceList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "DeviceList");

        cJSON* deviceInfo = cJSON_GetArrayItem(cObjDeviceList, index);

        cJSON* comment = cJSON_GetObjectItem(deviceInfo, "comment");
        cJSON* totalBytes = cJSON_GetObjectItem(deviceInfo, "totalBytes");
        cJSON* deviceInstanceID = cJSON_GetObjectItem(deviceInfo, "deviceInstanceID");
        cJSON* devicePath = cJSON_GetObjectItem(deviceInfo, "devicePath");
        cJSON* friendlyName = cJSON_GetObjectItem(deviceInfo, "friendlyName");
        cJSON* manufacturer = cJSON_GetObjectItem(deviceInfo, "manufacturer");
        cJSON* deviceDescription = cJSON_GetObjectItem(deviceInfo, "deviceDescription");
        cJSON* classGuid = cJSON_GetObjectItem(deviceInfo, "classGuid");

        strcpy(dInfo->comment, cJSON_GetStringValue(comment));
        strcpy(dInfo->totalBytes, cJSON_GetStringValue(totalBytes));
        strcpy(dInfo->deviceInstanceID, cJSON_GetStringValue(deviceInstanceID));
        strcpy(dInfo->devicePath, cJSON_GetStringValue(devicePath));
        strcpy(dInfo->friendlyName, cJSON_GetStringValue(friendlyName));
        strcpy(dInfo->manufacturer, cJSON_GetStringValue(manufacturer));
        strcpy(dInfo->deviceDescription, cJSON_GetStringValue(deviceDescription));
        strcpy(dInfo->classGuid, cJSON_GetStringValue(classGuid));

        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return (void*)dInfo;
}

int FindDeviceInDeviceList(char* deviceInstanceID)
{
    int deviceFound = 0;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        dInfo = (PDeviceInfo)malloc(sizeof(DeviceInfo));
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjDeviceList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "DeviceList");

        for (int i = 0; i < cJSON_GetArraySize(cObjDeviceList); i++)
        {
            cJSON* deviceInfo = cJSON_GetArrayItem(cObjDeviceList, i);

            cJSON* deviceInstanceID = cJSON_GetObjectItem(deviceInfo, "deviceInstanceID");

            if (strcmp(deviceInstanceID->valuestring, deviceInstanceID) == 0)
            {
                deviceFound = 1;
                break;
            }
        }

        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return deviceFound;
}

int RemoveDeviceFromDeviceList(int index)
{
    int deviceDelete = 0;
    OpenConfigFile("a+");
    if (bConfigFileOpen == TRUE)
    {
        dInfo = (PDeviceInfo)malloc(sizeof(DeviceInfo));
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjDeviceList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "DeviceList");

        cJSON_DeleteItemFromArray(cObjDeviceList, index);

        char* json_str = cJSON_Print(cObjRoot);
        if (json_str == NULL)
        {
            cJSON_Delete(cObjRoot);
            CloseConfigFile();
            return deviceDelete;
        }

        free(gConfigData);
        gConfigData = NULL;

        int jsonLen = strlen(json_str);

        gConfigData = (char*)malloc(jsonLen + 1);
        strcpy(gConfigData, json_str);

        free(json_str);
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return deviceDelete;
}

#pragma endregion

#pragma region BlackList
int AddDeviceToBlackList(void*)
{
    OpenConfigFile("a+");;
    if (bConfigFileOpen == TRUE)
    {
        DeviceInfo* dInfo = (DeviceInfo*)deviceInfo;
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjBlackListArray = cJSON_GetObjectItemCaseSensitive(cObjRoot, "BlackList");
        if (!cJSON_IsArray(cObjBlackListArray) && cJSON_IsNull(cObjBlackListArray))
        {
            cObjBlackListArray = cJSON_CreateArray();
            cJSON_AddItemToObject(cObjRoot, "BlackList", cObjBlackListArray);
        }

        cJSON* newDevice = cJSON_CreateObject();
        cJSON_AddStringToObject(newDevice, "comment", dInfo->comment);
        cJSON_AddStringToObject(newDevice, "totalBytes", dInfo->totalBytes);
        cJSON_AddStringToObject(newDevice, "deviceInstanceID", dInfo->deviceInstanceID);
        cJSON_AddStringToObject(newDevice, "devicePath", dInfo->devicePath);
        cJSON_AddStringToObject(newDevice, "friendlyName", dInfo->friendlyName);
        cJSON_AddStringToObject(newDevice, "manufacturer", dInfo->manufacturer);
        cJSON_AddStringToObject(newDevice, "deviceDescription", dInfo->deviceDescription);
        cJSON_AddStringToObject(newDevice, "classGuid", dInfo->classGuid);

        cJSON_AddItemToArray(cObjBlackListArray, newDevice);

        char* json_str = cJSON_Print(cObjRoot);
        
        if (json_str == NULL)
        {
            cJSON_Delete(cObjRoot);
            CloseConfigFile();
            return -1;
        }

        free(gConfigData);
        gConfigData = NULL;

        int jsonLen = strlen(json_str);

        gConfigData = (char*)malloc(jsonLen + 1);
        strcpy(gConfigData, json_str);

        free(json_str);
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return 0;
}

int BlackListLength()
{
    int blackListLen = 0;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjBlackList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "BlackList");

        if (!cJSON_IsArray(cObjBlackList) && cJSON_IsNull(cObjBlackList))
        {

        }
        else
        {
            blackListLen = cJSON_GetArraySize(cObjBlackList);
        }
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();

    return blackListLen;
}

void* GetBlackListItemFromIndex(int index)
{
    PDeviceInfo dInfo = NULL;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        dInfo = (PDeviceInfo)malloc(sizeof(DeviceInfo));
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjBlackList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "BlackList");

        cJSON* deviceInfo = cJSON_GetArrayItem(cObjBlackList, index);

        cJSON* comment = cJSON_GetObjectItem(deviceInfo, "comment");
        cJSON* totalBytes = cJSON_GetObjectItem(deviceInfo, "totalBytes");
        cJSON* deviceInstanceID = cJSON_GetObjectItem(deviceInfo, "deviceInstanceID");
        cJSON* devicePath = cJSON_GetObjectItem(deviceInfo, "devicePath");
        cJSON* friendlyName = cJSON_GetObjectItem(deviceInfo, "friendlyName");
        cJSON* manufacturer = cJSON_GetObjectItem(deviceInfo, "manufacturer");
        cJSON* deviceDescription = cJSON_GetObjectItem(deviceInfo, "deviceDescription");
        cJSON* classGuid = cJSON_GetObjectItem(deviceInfo, "classGuid");

        strcpy(dInfo->comment, cJSON_GetStringValue(comment));
        strcpy(dInfo->totalBytes, cJSON_GetStringValue(totalBytes));
        strcpy(dInfo->deviceInstanceID, cJSON_GetStringValue(deviceInstanceID));
        strcpy(dInfo->devicePath, cJSON_GetStringValue(devicePath));
        strcpy(dInfo->friendlyName, cJSON_GetStringValue(friendlyName));
        strcpy(dInfo->manufacturer, cJSON_GetStringValue(manufacturer));
        strcpy(dInfo->deviceDescription, cJSON_GetStringValue(deviceDescription));
        strcpy(dInfo->classGuid, cJSON_GetStringValue(classGuid));

        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return (void*)dInfo;
}

int FindDeviceInBlackList(char*)
{
    int deviceFound = 0;
    OpenConfigFile("r");
    if (bConfigFileOpen == TRUE)
    {
        dInfo = (PDeviceInfo)malloc(sizeof(DeviceInfo));
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjBlackList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "BlackList");

        for (int i = 0; i < cJSON_GetArraySize(cObjBlackList); i++)
        {
            cJSON* deviceInfo = cJSON_GetArrayItem(cObjBlackList, i);

            cJSON* deviceInstanceID = cJSON_GetObjectItem(deviceInfo, "deviceInstanceID");

            if (strcmp(deviceInstanceID->valuestring, deviceInstanceID) == 0)
            {
                deviceFound = 1;
                break;
            }
        }

        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return deviceFound;
}

int RemoveDeviceFromBlackList(int index)
{
    int deviceDelete = 0;
    OpenConfigFile("a+");
    if (bConfigFileOpen == TRUE)
    {
        cJSON* cObjRoot = cJSON_Parse(gConfigData);

        cJSON* cObjBlackList = cJSON_GetObjectItemCaseSensitive(cObjRoot, "BlackList");

        cJSON_DeleteItemFromArray(cObjBlackList, index);

        char* json_str = cJSON_Print(cObjRoot);
        if (json_str == NULL)
        {
            cJSON_Delete(cObjRoot);
            CloseConfigFile();
            return deviceDelete;
        }

        free(gConfigData);
        gConfigData = NULL;

        int jsonLen = strlen(json_str);

        gConfigData = (char*)malloc(jsonLen + 1);
        strcpy(gConfigData, json_str);

        free(json_str);
        cJSON_Delete(cObjRoot);
    }

    CloseConfigFile();
    return deviceDelete;
}

#pragma endregion


 


