#include "dshowtest.h"


int dshow_test(void)
{
	int x = 0;

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		// Add error-handling code here. (Omitted for clarity.)
	}

	IGraphBuilder *pGraph;
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

	IMediaControl *pControl;
	IMediaEvent   *pEvent;
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

	hr = pGraph->RenderFile(L"d:\\Video\\jcuuElizium.avi", NULL);
	hr = pControl->Run();

	long evCode = 0;
	pEvent->WaitForCompletion(INFINITE, &evCode);

	pControl->Release();
	pEvent->Release();
	pGraph->Release();
	CoUninitialize();

	return 0;	
}
