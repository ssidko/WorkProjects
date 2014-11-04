#include "MainWnd.h"

Pane::Pane(Wnd *parent, RECT *rect) : erase_counter(0), paint_counter(0)
{
	Wnd::Create(WND_CLASS_NAME, _T("TestPane"), WS_CHILD|WS_DLGFRAME,
				rect->left, rect->top, (rect->right - rect->left), (rect->bottom - rect->top),
				parent, NULL,::GetModuleHandle(NULL));
}

Pane::~Pane()
{
	
}

LRESULT Pane::OnEraseBackground(HDC hdc)
{
	erase_counter++;

	TCHAR str[32] = {0};
	_stprintf(str, _T("Erase counter: %d"), erase_counter);

	//Создаем контекст рисования и устанавливаем 
	//пиксельную систему координат
	Graphics g(hdc);

	g.SetPageUnit(UnitPixel);
	RECT rc; GetClientRect(&rc);
	RectF bounds(0, 0, (REAL)(rc.right), (REAL)(rc.bottom));
	SolidBrush brush(Color(255, 220, 200, 220));
	g.FillRectangle(&brush, bounds);

	// Готовим формат и параметры шрифта
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	Font font(L"Tahoma", 8, FontStyleRegular);

	SolidBrush text_brush(Color(255, 0, 0, 0));
	PointF text_pt(5, 10);
	g.DrawString(str, -1, &font, text_pt, &text_brush);

	return 1;
}

void Pane::OnPaint(HDC hdc)
{

}

LRESULT Pane::OnNcHitTest(WORD x, WORD y)
{
	return HTCAPTION;
}

// 

MainWnd::MainWnd(void)
{
}

MainWnd::~MainWnd(void)
{
}

void MainWnd::Create(LPCTSTR lpWindowName)
{
	//Wnd::Create(WND_CLASS_NAME, lpWindowName, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN/*|WS_CLIPSIBLINGS*/, CW_USEDEFAULT, CW_USEDEFAULT,
	//			CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, ::GetModuleHandle(NULL));
	Wnd::CreateEx(WS_EX_WINDOWEDGE, WND_CLASS_NAME, lpWindowName, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN/*|WS_CLIPSIBLINGS*/, CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, ::GetModuleHandle(NULL));
}


LRESULT MainWnd::OnCreate(LPCREATESTRUCT pcs)
{
	try
	{
		SIZE btn_size;
		RECT rc;

		GetClientRect(&rc);
		def_font.CreateFont(GetDC(), _T("Tahoma"), 8);

		controls_gbox.Create(_T("Controls"), rc.left+5, rc.top/*+5*/, rc.right-2*5, (rc.bottom/2)-2*5, this, kControlsGBox);
		controls_gbox.SetFont(&def_font);

		//painting_gbox.Create(_T("Painting"), 0, 0, 0, 0, this, kPaintingGBox);
		//painting_gbox.SetFont(&def_font);

		button.Create(_T("Welcome, GDI+ !"), 10, 15, 0, 0, this, 1);
		button.SetFont(&def_font);
		button.GetIdealSize(&btn_size);
		button.SetWindowSize(&btn_size);

		static1.Create(_T("Test static"), SS_LEFTNOWORDWRAP, 10, 40, 100, 20, this, MainWnd::kStatic);
		static1.SetFont(&def_font);

		rc.top = 70;
		rc.left = 10;
		rc.right = 200;
		rc.bottom = 200;

		pane = new Pane(this, &rc);
		pane->Show();

		return 0;
	}
	catch(Exception &e)
	{
		e.ShowMessage();
		return  -1;
	}
}

void MainWnd::ArrangeControls(void)
{
	RECT rc;
	GetClientRect(&rc);

	controls_gbox.SetWindowPos(rc.left+5, rc.top+5, rc.right-2*5, (rc.bottom/2)-2*5);
	painting_gbox.SetWindowPos(rc.left+5, (rc.bottom/2)+5, rc.right-2*5, (rc.bottom/2)-2*5);
}

LRESULT MainWnd::OnSize(UINT nType, int width, int height)
{
	controls_gbox.SetWindowPos(5, 0, width-2*5, (height/2)/*-5*/);
	painting_gbox.SetWindowPos(5, (height/2), width-2*5, (height/2)-5);
	return 0;
}

void MainWnd::OnPaint(HDC hdc)
{
	// Все строки - в кодировке Unicode
	WCHAR welcome[]=L"Welcome, GDI+ !";

	// Создаем контекст рисования и устанавливаем 
	// пиксельную систему координат
	Graphics g(hdc);
	g.SetPageUnit(UnitPixel);
	RECT rc; GetClientRect(&rc);
	RectF bounds(0, 0, float(rc.right), float(rc.bottom));

	// Создаем кисть с градиентом на все окно и полупрозрачностью         
	//LinearGradientBrush brush
	//(bounds, Color(50, 255, 0, 0), Color(190, 0, 0, 255), 
	//LinearGradientModeBackwardDiagonal);

	SolidBrush tetx_brush(Color(255, 0,0,0));
	SolidBrush brush(Color(255, 200,200,200));

	// Готовим формат и параметры шрифта
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	Font font(L"Tahoma", 8, FontStyleRegular);

	//g.FillRectangle(&brush, bounds);

	// Выводим текст приветствия, длина -1 означает,.
	// что строка заканчивается нулем    
	g.DrawString(welcome, -1, &font, bounds, &format, &tetx_brush);
}

LRESULT MainWnd::OnEraseBackground(HDC hdc)
{
	 //Создаем контекст рисования и устанавливаем 
	 //пиксельную систему координат
	Graphics g(hdc);
	g.SetPageUnit(UnitPixel);
	RECT rc; GetClientRect(&rc);
	RectF bounds(0, 0, (REAL)(rc.right), (REAL)(rc.bottom));
	SolidBrush brush(Color(255,236,233,216));

	g.FillRectangle(&brush, bounds);
	return 1;
}

void MainWnd::OnClose()
{
	DestroyWindow();
}

void MainWnd::OnDestroy()
{

}

LRESULT MainWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (lParam)
	{
		if (LOWORD(wParam) == 1)
		{
			if (HIWORD(wParam) == BN_CLICKED)
				OnTstButtonClick();
		}
	}
	return 0;
}

void MainWnd::OnTstButtonClick()
{
	int x = 0;
}