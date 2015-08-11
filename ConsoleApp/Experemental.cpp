#include "Experemental.h"


void EnumerateDeviceInterface(const GUID* device_interface_guid)
{
	BOOL result = false;
	DWORD error = 0;
	DWORD dev_index = 0;
	DWORD iface_index = 0;
	HDEVINFO info_set = NULL;

	SP_DEVINFO_DATA dev_info = { 0 };
	dev_info.cbSize = sizeof(SP_DEVINFO_DATA);

	info_set = ::SetupDiGetClassDevs(device_interface_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (info_set != INVALID_HANDLE_VALUE) {
		while (::SetupDiEnumDeviceInfo(info_set, dev_index, &dev_info)) {

			SP_DEVICE_INTERFACE_DATA dev_iface_data = { 0 };
			dev_iface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			iface_index = 0;
			while (::SetupDiEnumDeviceInterfaces(info_set, &dev_info, device_interface_guid, iface_index, &dev_iface_data)) {

				DWORD required_size = 0;
				PSP_DEVICE_INTERFACE_DETAIL_DATA dev_iface_detail = NULL;

				result = SetupDiGetDeviceInterfaceDetail(info_set, &dev_iface_data, NULL, 0, &required_size, NULL);
				error = ::GetLastError();

				dev_iface_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new BYTE[required_size];
				memset(dev_iface_detail, 0x00, required_size);
				dev_iface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				result = SetupDiGetDeviceInterfaceDetail(info_set, &dev_iface_data, dev_iface_detail, required_size, NULL, NULL);
				error = ::GetLastError();

				DWORD reg_data_type = 0;
				required_size = 0;
				result = ::SetupDiGetDeviceRegistryProperty(info_set, &dev_info, SPDRP_FRIENDLYNAME/*SPDRP_DEVICEDESC*/, &reg_data_type, NULL, 0, &required_size);
				error = ::GetLastError();

				BYTE *property_buff = (BYTE *) new BYTE[required_size];
				memset(property_buff, 0x00, required_size);

				result = ::SetupDiGetDeviceRegistryProperty(info_set, &dev_info, SPDRP_FRIENDLYNAME/*SPDRP_DEVICEDESC*/, &reg_data_type, property_buff, required_size, NULL);
				error = ::GetLastError();


				TCHAR *str = (TCHAR *)property_buff;

				std::cout << "### - " << (TCHAR *)property_buff << std::endl << " ---> " << dev_iface_detail->DevicePath << std::endl;

				++iface_index;
				delete[] dev_iface_detail;
				delete[] property_buff;
			}

			++dev_index;
		}

		if (error = ::GetLastError()) {
			int x = 0;
		}

		result = ::SetupDiDestroyDeviceInfoList(info_set);
	}
}