#include "Experemental.h"

#include "WinFile.h"

void EnumerateDevicesInterfaces(const GUID* device_interface_guid)
{
	BOOL result = false;
	DWORD last_error = 0;
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
				last_error = ::GetLastError();

				dev_iface_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new BYTE[required_size];
				memset(dev_iface_detail, 0x00, required_size);
				dev_iface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				result = SetupDiGetDeviceInterfaceDetail(info_set, &dev_iface_data, dev_iface_detail, required_size, NULL, NULL);
				last_error = ::GetLastError();

				DWORD reg_data_type = 0;
				required_size = 0;
				result = ::SetupDiGetDeviceRegistryProperty(info_set, &dev_info, SPDRP_FRIENDLYNAME/*SPDRP_DEVICEDESC*/, &reg_data_type, NULL, 0, &required_size);
				last_error = ::GetLastError();

				BYTE *property_buff = (BYTE *) new BYTE[required_size];
				memset(property_buff, 0x00, required_size);

				result = ::SetupDiGetDeviceRegistryProperty(info_set, &dev_info, SPDRP_FRIENDLYNAME/*SPDRP_DEVICEDESC*/, &reg_data_type, property_buff, required_size, NULL);
				last_error = ::GetLastError();


				TCHAR *str = (TCHAR *)property_buff;

				std::cout << "Frendly name: " << (TCHAR *)property_buff << std::endl;
				std::cout << "Device path: " << dev_iface_detail->DevicePath << std::endl;

				MyLib::WinError error;
				MyLib::WinFile drive(dev_iface_detail->DevicePath);
				if (drive.Open(MyLib::FileMode::kReadOnly)) {

					BOOL result = FALSE;
					DWORD bytes_ret = 0;

					STORAGE_DEVICE_NUMBER dev_number;
					memset(&dev_number, 0x00, sizeof(dev_number));

					result = ::DeviceIoControl(drive.Handle(),
						IOCTL_STORAGE_GET_DEVICE_NUMBER,
						NULL, 0,
						&dev_number, sizeof(dev_number),
						&bytes_ret, NULL);
					if (!result) {
						error.Update();
					}

					std::cout << "Device number: " << dev_number.DeviceNumber << std::endl;
					std::cout << "Device type: " << dev_number.DeviceType << std::endl;

					FILE_DEVICE_BATTERY;

					STORAGE_PROPERTY_QUERY property_query;
					memset(&property_query, 0x00, sizeof(property_query));
					property_query.PropertyId = StorageDeviceProperty;
					property_query.QueryType = PropertyStandardQuery;

					STORAGE_DESCRIPTOR_HEADER descriptor_header;
					memset(&descriptor_header, 0x00, sizeof(descriptor_header));

					result = ::DeviceIoControl(drive.Handle(),
						IOCTL_STORAGE_QUERY_PROPERTY,
						&property_query, sizeof(property_query),
						&descriptor_header, sizeof(descriptor_header),
						&bytes_ret, NULL);
					if (!result) {
						error.Update();
					}

					PSTORAGE_DEVICE_DESCRIPTOR device_descriptor = (PSTORAGE_DEVICE_DESCRIPTOR) new BYTE[descriptor_header.Size];
					memset(device_descriptor, 0x00, descriptor_header.Size);
					device_descriptor->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

					result = ::DeviceIoControl(drive.Handle(),
						IOCTL_STORAGE_QUERY_PROPERTY,
						&property_query, sizeof(property_query),
						device_descriptor, descriptor_header.Size,
						&bytes_ret, NULL);
					if (!result) {
						error.Update();
					}

					if (device_descriptor->Version == sizeof(STORAGE_DEVICE_DESCRIPTOR)) {
						MyLib::WinFile file(_T("device_descriptor.bin"));
						if (file.Create(MyLib::FileMode::kReadWrite)) {
							file.Write(device_descriptor, descriptor_header.Size);
						}

						if (device_descriptor->VendorIdOffset) {
							std::cout << "Vendor ID: " << (TCHAR *)&((BYTE *)device_descriptor)[device_descriptor->VendorIdOffset] << std::endl;
						}
						if (device_descriptor->ProductIdOffset) {
							std::cout << "Product ID: " << (TCHAR *)&((BYTE *)device_descriptor)[device_descriptor->ProductIdOffset] << std::endl;
						}
						if (device_descriptor->ProductRevisionOffset) {
							std::cout << "Product revision: " << (TCHAR *)&((BYTE *)device_descriptor)[device_descriptor->ProductRevisionOffset] << std::endl;
						}
						if (device_descriptor->SerialNumberOffset) {
							std::cout << "Serial Number: " << (TCHAR *)&((BYTE *)device_descriptor)[device_descriptor->SerialNumberOffset] << std::endl;
						}
						std::cout << std::endl;					
					}
					delete[] device_descriptor;
				}

				++iface_index;
				delete[] dev_iface_detail;
				delete[] property_buff;
			}

			++dev_index;
		}

		if (last_error = ::GetLastError()) {
			int x = 0;
		}

		result = ::SetupDiDestroyDeviceInfoList(info_set);
	}
}

void Testing(void)
{

}
