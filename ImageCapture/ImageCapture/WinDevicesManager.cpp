#include "WinDevicesManager.h"
#include <windows.h>
#include <Cfgmgr32.h>
#include <Setupapi.h>

#pragma comment(lib, "Setupapi.lib")

DEFINE_GUID(GUID_DEVCLASS_PORTS, 0x4d36e978L, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);

class DeviceInfoSet
{
private:
	HDEVINFO handle;
	DWORD index;
	DWORD error;
public:
	DeviceInfoSet();
	~DeviceInfoSet();
	BOOL Open(GUID *class_guid, DWORD flags);
	void Close();
};

DeviceInfoSet::DeviceInfoSet() : handle(NULL), index(0), error(0)
{
}

DeviceInfoSet::~DeviceInfoSet()
{
	Close();
}

BOOL DeviceInfoSet::Open(GUID *class_guid, DWORD flags)
{
	Close();

	handle = ::SetupDiGetClassDevs(class_guid, NULL, NULL, flags);
	if (handle == INVALID_HANDLE_VALUE) {
		handle = NULL;
		error = ::GetLastError();
		return FALSE;
	}
	return TRUE;	
}

void DeviceInfoSet::Close()
{
	if (handle) {
		::SetupDiDestroyDeviceInfoList(handle);
		handle = NULL;
		error = 0;
	}
}


WinDevicesManager::WinDevicesManager(void)
{
}

WinDevicesManager::~WinDevicesManager(void)
{
}

void WinDevicesManager::Test(void)
{
	BOOL result = false;
	DWORD err = 0;
	ULONG class_index = 0;
	GUID class_guid;
	DWORD sz = 0;
	TCHAR class_name[MAX_CLASS_NAME_LEN] = {0};
	BYTE buff[2048] = {0};
	DWORD buff_size = 2048;
	TCHAR *str = (TCHAR *)buff;

	HDEVINFO dev_info_set = SetupDiGetClassDevs(&/*GUID_DEVCLASS_PORTS*/GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (dev_info_set != INVALID_HANDLE_VALUE) {
		DWORD dev_index = 0;
		SP_DEVINFO_DATA info;
		memset(&info, 0x00, sizeof(info));
		info.cbSize = sizeof(info);

		SP_DEVICE_INTERFACE_DATA interface_data;
		memset(&interface_data, 0x00, sizeof(interface_data));
		interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		while (::SetupDiEnumDeviceInfo(dev_info_set, dev_index, &info)) {

			memset(&buff, 0x00, sizeof(buff));
			if (!SetupDiGetDeviceRegistryProperty(dev_info_set, &info, SPDRP_DEVICEDESC, NULL, buff, 2048, NULL)) {
				err = ::GetLastError();
			}

			memset(&buff, 0x00, sizeof(buff));
			if (!SetupDiGetDeviceRegistryProperty(dev_info_set, &info, SPDRP_FRIENDLYNAME, NULL, buff, 2048, NULL)) {
				err = ::GetLastError();
			}

			memset(&buff, 0x00, sizeof(buff));
			if (!SetupDiGetDeviceRegistryProperty(dev_info_set, &info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, buff, 2048, NULL)) {
				err = ::GetLastError();
			}

			result = SetupDiEnumDeviceInterfaces(dev_info_set, &info, &GUID_DEVINTERFACE_COMPORT, dev_index, &interface_data);
			if (!result) {
				err = ::GetLastError();
			}

			//PSP_DEVICE_INTERFACE_DETAIL_DATA interface_datail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buff;
			//memset(&buff, 0x00, sizeof(buff));
			//interface_datail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			//result = SetupDiGetDeviceInterfaceDetail(dev_info_set, &interface_data, interface_datail_data, 2048, NULL, NULL);
			//if (!result) {
			//	err = ::GetLastError();
			//}

			PSP_DEVICE_INTERFACE_DETAIL_DATA_A interface_datail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)buff;
			memset(&buff, 0x00, sizeof(buff));
			interface_datail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
			result = SetupDiGetDeviceInterfaceDetailA(dev_info_set, &interface_data, interface_datail_data, 2048, NULL, NULL);
			if (!result) {
				err = ::GetLastError();
			}

			const HKEY key = ::SetupDiOpenDevRegKey(dev_info_set, &info, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
			if (key == INVALID_HANDLE_VALUE) {
				continue;
			}

			DWORD data_tape;

			memset(&buff, 0x00, sizeof(buff));
			LONG ret = ::RegQueryValueExA(key, ("PortName"), NULL, &data_tape, reinterpret_cast<unsigned char *>(buff), &buff_size);
			memset(&buff, 0x00, sizeof(buff));
			ret = ::RegQueryValueExA(key, ("PortNumber"), NULL, &data_tape, reinterpret_cast<unsigned char *>(buff), &buff_size);

			dev_index++;
		}
		
		SetupDiDestroyDeviceInfoList(dev_info_set);
	}
	return;
}
