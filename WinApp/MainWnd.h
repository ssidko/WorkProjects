#ifndef _MAINWND
#define _MAINWND

#include "main.h"
#include "Button.h"
#include "Static.h"
#include "ListBox.h"
#include "stdio.h"

#include "main.h"

using namespace Gdiplus;

#define RGB_COLOR_BACKGROUND_1			RGB(245,244,239)
#define RGB_COLOR_BACKGROUND_2			RGB(233,233,238)
#define RGB_COLOR_BACKGROUND_3			RGB(236,233,216)

class Pane : public W32Lib::Wnd
{
private:
	DWORD erase_counter;
	DWORD paint_counter;

public:
	Pane (Wnd *parent, RECT *rect);
	~Pane(void);

	virtual LRESULT OnEraseBackground(HDC hdc);
	virtual void OnPaint(HDC hdc);
	virtual LRESULT OnNcHitTest(WORD x, WORD y);
};

class MainWnd : public W32Lib::Wnd
{
	enum CtrlID
	{
		kNull,
		kControlsGBox,
		kPaintingGBox,
		kButton,
		kStatic,
		kListBox
	};

private:
	WFont def_font;
	PButton button;
	GroupBox controls_gbox;
	GroupBox painting_gbox;
	WStatic static1;
	Pane *pane;

public:
	MainWnd(void);
	~MainWnd(void);

	void Create(LPCTSTR lpWindowName);
	void ArrangeControls(void);

	virtual LRESULT OnCreate(LPCREATESTRUCT pcs);
	virtual LRESULT OnEraseBackground(HDC hdc);
	virtual void OnPaint(HDC hdc);
	virtual void OnClose();
	virtual void OnDestroy();
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(UINT nType, int width, int height);
	void OnTstButtonClick(void);
};

#endif //_MAINWND