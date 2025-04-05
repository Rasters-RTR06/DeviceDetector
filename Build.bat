cl /c /EHsc DeviceDetector.cpp

rc.exe DeviceDetector.rc

link DeviceDetector.obj DeviceDetector.res user32.lib gdi32.lib kernel32.lib SetupAPI.lib /SUBSYSTEM:WINDOWS