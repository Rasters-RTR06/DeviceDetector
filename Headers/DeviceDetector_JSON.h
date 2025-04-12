#pragma once
//#ifdef __cplusplus
//
//extern "C"
//{
//#endif

    typedef enum {
        DEVICE_PENDRIVE = 0,
        DEVICE_CHARGING_DEVICE = 1,
        DEVICE_LG_MONITOR = 2,
        DEVICE_SAMSUNG_MONITOR = 3,
        DEVICE_MOUSE = 4,
        DEVICE_HEADPHONE = 5,
        DEVICE_UNKNOWN = -1
    } DeviceEnum;

    void readDevices();
    void addDevice(const char* category, const char* deviceName, const char* config);
    char* getDeviceByID(int deviceID);
    void parseDeviceString(const char* input, char* category, size_t cat_size, char* name, size_t name_size, char* config, size_t config_size);


    void OpenConfigFile(char *);
    void CloseConfigFile();

    void EncryptConfigFile(int );
    void DecryptConfigFile();

    void* ReadAdminDetails();
    int AddAdminDetails(void*);

    int AddDeviceToDeviceInfoList(void *);
    int DeviceInfoListLength();
    void* GetDeviceInfoItemFromIndex(int);
    int FindDeviceInDeviceList(char*);
    int RemoveDeviceFromDeviceList(int);
    int UpdateAttachedDeviceComment(int, char*);


    int AddDeviceToBlackList(void*);
    int BlackListLength();
    void* GetBlackListItemFromIndex(int);
    int FindDeviceInBlackList(char*);
    int RemoveDeviceFromBlackList(int);
    int UpdateBlackListedDeviceComment(int, char*);

//#ifdef __cplusplus
//}
//#endif


