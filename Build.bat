cl /c /EHsc DeviceDetector.cpp DeviceDetector_JSON.c cJSON.c

rc.exe DeviceDetector.rc

link DeviceDetector.obj DeviceDetector_JSON.obj cJSON.obj DeviceDetector.res user32.lib gdi32.lib kernel32.lib /SUBSYSTEM:WINDOWS

DeviceDetector.exe