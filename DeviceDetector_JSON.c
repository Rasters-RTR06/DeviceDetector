#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cJSON.h"

#define JSON_FILE "devices.json"

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
            sscanf_s(line, "- Category: %49s", category, cat_size);
        else if (strstr(line, "- Name:") != NULL)
            sscanf_s(line, "- Name: %49s", name, name_size);
        else if (strstr(line, "- Configuration:") != NULL)
            sscanf_s(line, "- Configuration: %99[^\n]", config, config_size);  

        line = strtok_s(NULL, "\n", &context);
    }
}

/*int main() {
    readDevices();
    addDevice("USB Device", "Pendrive", "32GB");
    addDevice("USB Device", "Charging Device", "Fast Charger");
    addDevice("Monitor", "LG", "1080p, 60Hz");
    addDevice("Monitor", "Samsung", "4K, 144Hz");
    addDevice("Bluetooth Device", "Mouse", "Wireless");
    addDevice("Audio Device", "Headphone", "Stereo, Noise Cancelling");
    
   
    readDevices();

    int deviceID;

    printf("Enter Device ID to retrieve: ");
    scanf_s("%d", &deviceID);

    char* deviceInfo = getDeviceByID(deviceID);
   // printf("%s\n", deviceInfo);

    char category[50], name[50], config[100];

    parseDeviceString(deviceInfo, category, (unsigned int)sizeof(category),
        name, (unsigned int)sizeof(name),
        config, (unsigned int)sizeof(config));

    printf("Category: %s\n", category);
    printf("Name: %s\n", name);
    printf("Configuration: %s\n", config);


    free(deviceInfo);
    return 0;
}*/
