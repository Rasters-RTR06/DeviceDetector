# DeviceDetector

## JSON READER WRITER

### WHY JSON FILE ?
OUR MAIN REASON IS : 

Configuration and Data Storage:
Configuration Files: JSON is often used for storing configuration settings for UIs, such as user preferences, application settings, and layout configurations.   
Data Storage: While not a full-fledged database, JSON can be used for simple data storage within applications or for storing data in NoSQL databases that support JSON documents.

Flexibility and Structure:
Hierarchical Data: JSON can represent complex, hierarchical data structures using nested objects and arrays. This allows UIs to handle data with varying levels of complexity.   
Dynamic Data: JSON's flexibility makes it suitable for handling dynamic data that may change frequently.

### OUR Structure
```json
  {
	"USB Device":	[{
			"name":	"Pendrive",
			"config":	"32GB"
		}, {
			"name":	"Charging Device",
			"config":	"Fast Charger"
		}],
	"Monitor":	[{
			"name":	"LG",
			"config":	"1080p, 60Hz"
		}, {
			"name":	"Samsung",
			"config":	"4K, 144Hz"
		}],
	"Bluetooth Device":	[{
			"name":	"Mouse",
			"config":	"Wireless"
		}],
	"Audio Device":	[{
			"name":	"Headphone",
			"config":	"Stereo, Noise Cancelling"
		}]
}
  ```


### Inspiration
https://github.com/DaveGamble/cJSON

### How to use readDevices() & addDevice() 
readDevicecs will list down everything in json file.
Here we can make use of getDeviceByID() which take and ID as input and return deviceInfo.
With deviceInfo once can make use of parseDeviceString() to parse deviceInfo char*,
char category[50], name[50], config[100];

Now we can show this output value on UI just by reading this.

### Future Plan
We can add delete device function which will delete the entry of device from json file.
