cl /c /EHsc DeviceDetector.cpp

link DeviceDetector.obj user32.lib gdi32.lib kernel32.lib /SUBSYSTEM:WINDOWS