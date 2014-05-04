#include "DirectShow.h"
#include "Dshow.h"

DirectShow::DirectShow(void)
{
}

DirectShow::~DirectShow(void)
{
}

void DirectShow::Test()
{
	DWORD err;
	IGraphBuilder *pGraph = NULL;
	IMediaControl *pControl = NULL;
	IMediaEvent   *pEvent = NULL;

	// Initialize the COM library.
	HRESULT result = CoInitialize(NULL);
	if (FAILED(result)) {
		err = ::GetLastError();
	}

	// Create the filter graph manager and query for interfaces.
	result = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (FAILED(result)) {
		err = ::GetLastError();
	}

	result = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	result = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

	QList<QString> dev_list;
	AvailableDevices(dev_list);

	pControl->Release();
	pEvent->Release();
	pGraph->Release();
	CoUninitialize();
}

void DirectShow::AvailableDevices(QList<QString> &list)
{
	DWORD err = 0;
	HRESULT result = 0;
	QString str = "";

	list.clear();

	result = CoInitialize(NULL);
	if (FAILED(result)) {
		err = ::GetLastError();
	}

	ICreateDevEnum *sys_dev_enum = NULL;
	result = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&sys_dev_enum);
	if (FAILED(result)) {
		err = ::GetLastError();
		return;
	}

	IEnumMoniker *enum_moniker = NULL;
	result = sys_dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enum_moniker, 0);
	if (result == S_OK) {
		IMoniker *moniker = NULL;
		while (S_OK == enum_moniker->Next(1, &moniker, NULL)) {
			IPropertyBag *prop_bag;
			result = moniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&prop_bag);
			if (SUCCEEDED(result)) {
				VARIANT name;
				VariantInit(&name);
				result = prop_bag->Read(L"FriendlyName", &name, 0);
				if (SUCCEEDED(result))
				{
					str = QString::fromUtf16((wchar_t *)name.bstrVal, -1);
					list.push_back(str);
				}
				VariantClear(&name);
				prop_bag->Release();
			}
			moniker->Release();
		}
	} else {
		err = ::GetLastError();
	}


	if (enum_moniker) enum_moniker->Release();
	if (sys_dev_enum) sys_dev_enum->Release();
	CoUninitialize();
}
