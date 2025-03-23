#ifdef __cplusplus

extern "C"
{

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



}
#endif


