
// PmbClockDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "PmbClock.h"
#include "PmbClockDlg.h"
#include "afxdialogex.h"

#include <sys/timeb.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma warning(push)
#pragma warning(disable : 4244)
//#pragma warning(pop)

#define PMB_CLOCK_TIMER_	12345

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPmbClockDlg dialog



CPmbClockDlg::CPmbClockDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PMBCLOCK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CPmbClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CPmbClockDlg::adjust_layout(int cx, int cy, bool bFirstset)
{
	const int dx = m_format == tpfrmtMillisecond ? 10 : m_format == tpfrmtSecond ? 8 : 5;

	CRect cr;
	if (cx / dx < cy / 2)
	{
		cr.left = cx / dx;
		cr.right = cr.left + cx / dx;
		cr.top = cy / 2 - (3 * cx / dx / 2) / 2;
		cr.bottom = cr.top + 3 * cx / dx / 2;
	}
	else
	{
		int wi = 2 * cy / 3;
		cr.left = cx / 2 - wi * dx / 2;
		cr.right = cr.left + wi;
		cr.top = cy / 4;
		cr.bottom = cr.top + cy / 2;
	}

	{
		short nBig = m_format == tpfrmtMillisecond || m_format == tpfrmtSecond ? 6 : 4;
		short nSmall = m_format == tpfrmtMillisecond ? 3 : 0;
		short nSpace = m_format == tpfrmtMillisecond || m_format == tpfrmtSecond ? 2 : 1;

		int width = cr.Width();
		int w = nBig * width + nSmall * 3 * width / 4 + 2 * width / 4;
		cr.left = cx / 2 - w / 2;
		cr.right = cr.left + width;
	}

	int nDig = m_format == tpfrmtMillisecond ? 9 : m_format == tpfrmtSecond ? 6 : 4;
	for (int i = 0; i < nDig; ++i)
	{
		if (6 == i)
		{
			cr.top += cr.Height() / 4;
			cr.right = cr.left + 3 * cr.Width() / 4;
		}
		if (bFirstset)
			m_s[i].set(cr, m_bkColor, m_color, i == 5 && m_format == tpfrmtMillisecond);
		else
			m_s[i].set(cr);
		int w = cr.Width();
		if (i < 5 && i % 2 == 1)
			cr.left = cr.right + w / 4;
		else
			cr.left = cr.right;
		cr.right = cr.left + w;
	}
}




void CPmbClockDlg::update_taskbar_icon(CDC* pDC, const tm& now)
{
	if ((m_format == tpfrmtMillisecond || m_format == tpfrmtSecond || IsIconic()) && m_sec == now.tm_sec || m_format == tpfrmtMinute && m_sec == now.tm_min)
		return;
	if (m_format == tpfrmtMillisecond || m_format == tpfrmtSecond || IsIconic())
		m_sec = now.tm_sec;
	else
		m_sec = now.tm_min;

	CDC memDC;
	memDC.CreateCompatibleDC(nullptr);

	if (!m_hIconClock)
	{
		ICONINFO info;
		GetIconInfo(m_hIcon, &info);
		BITMAP bmp;
		GetObject(info.hbmColor, sizeof(bmp), &bmp);

		m_hBitmap = (HBITMAP)CopyImage(info.hbmColor, IMAGE_BITMAP, 0, 0, 0);
	}
	HBITMAP hOldBmp = (HBITMAP)memDC.SelectObject(m_hBitmap);
	
	memDC.FillSolidRect(0, 0, 32, 32, RGB(0x00, 0x00, 0x00));

	CBrush brush;
	brush.CreateSolidBrush(RGB(0xC0, 0xC0, 0xC0));
	CBrush* oldBrush = memDC.SelectObject(&brush);
	CPen penCircl, penDiv;
	penCircl.CreatePen(PS_SOLID, 1, RGB(0x80, 0x80, 0x80));
	penDiv.CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
	CPen* oldPen = memDC.SelectObject(&penCircl);
	memDC.Ellipse(0, 0, 32, 32);

	for (int d = 0; d < 60; d += 5)
	{
		int ri = d % 10 ? 16 - 3 : 16 - 5;
		memDC.SelectObject(d % 10 ? &penCircl : &penDiv);
		memDC.MoveTo(16 + ri * sin(d * 360 / 60 * 3.14159 / 180), 16 - ri * cos(d * 360 / 60 * 3.141592 / 180));
		memDC.LineTo(16 + 16 * sin(d * 360 / 60 * 3.14159 / 180), 16 - 16 * cos(d * 360 / 60 * 3.141592 / 180));
	}

	CPen penSec, penMin, penHour;
	penSec.CreatePen(PS_SOLID, 1, RGB(0xFF, 0x00, 0x00));
	penMin.CreatePen(PS_SOLID, 3, RGB(0x00, 0x00, 0xFF));
	penHour.CreatePen(PS_SOLID, 5, RGB(0x00, 0x00, 0x80));

	memDC.SelectObject(&penHour);
	memDC.MoveTo(16, 16);
	memDC.LineTo(16 + 9 * sin(now.tm_hour * 360 / 12 * 3.14159 / 180), 16 - 9 * cos(now.tm_hour * 360 / 12 * 3.141592 / 180));

	memDC.SelectObject(&penMin);
	memDC.MoveTo(16, 16);
	memDC.LineTo(16 + 12 * sin(now.tm_min * 360 / 60 * 3.14159 / 180), 16 - 12 * cos(now.tm_min * 360 / 60 * 3.141592 / 180));

	if (m_format == tpfrmtMillisecond || m_format == tpfrmtSecond || IsIconic())
	{
		memDC.SelectObject(&penSec);
		memDC.MoveTo(16, 16);
		memDC.LineTo(16 + 16 * sin(now.tm_sec * 360 / 60 * 3.14159 / 180), 16 - 16 * cos(now.tm_sec * 360 / 60 * 3.141592 / 180));
	}

	BYTE XORmaskIcon[32 * 32 / 8 * 1 * 32];
	BYTE ANDmaskIcon[32 * 32 / 8 * 1];
	LONG count = GetBitmapBits(m_hBitmap, 32 * 32 / 8 * 1 * 32, XORmaskIcon);

	memDC.SelectObject(hOldBmp);
	memDC.SelectObject(oldPen);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&memDC, 32, 32);
	CBitmap* oldBmp = memDC.SelectObject(&memBmp);

	memDC.FillSolidRect(0, 0, 32, 32, RGB(0xFF, 0xFF, 0xFF));
	CBrush brushInv;
	brushInv.CreateSolidBrush(RGB(0x00, 0x00, 0x00));
	memDC.SelectObject(&brushInv);
	memDC.Ellipse(0, 0, 32, 32);
	DWORD count2 = memBmp.GetBitmapBits(128, ANDmaskIcon);

	memDC.SelectObject(oldBrush);
	memDC.SelectObject(oldBmp);
	brush.DeleteObject();
	memBmp.DeleteObject();
	memDC.DeleteDC();

	if (m_hIconClock)
		DestroyIcon(m_hIconClock);

	m_hIconClock = CreateIcon(AfxGetInstanceHandle(), 32, 32, 1, 32, ANDmaskIcon, XORmaskIcon);
	SetIcon(m_hIconClock, FALSE);
	SetIcon(m_hIconClock, TRUE);
}


BEGIN_MESSAGE_MAP(CPmbClockDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_TASKBAR, &CPmbClockDlg::OnTaskbar)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CONFIG_MILLISECONDS, &CPmbClockDlg::OnConfigMilliseconds)
	ON_COMMAND(ID_CONFIG_SECONDS, &CPmbClockDlg::OnConfigSeconds)
	ON_COMMAND(ID_CONFIG_MINUTES, &CPmbClockDlg::OnConfigMinutes)
	ON_COMMAND(ID_CONFIG_TRANSPARENT, &CPmbClockDlg::OnConfigTransparent)
	ON_COMMAND(ID_CONFIG_BACKCOLOR, &CPmbClockDlg::OnConfigBackcolor)
	ON_COMMAND(ID_CONFIG_COLOR, &CPmbClockDlg::OnConfigColor)
	ON_COMMAND(ID_CONFIG_DATE, &CPmbClockDlg::OnConfigDate)
	ON_COMMAND(ID_CONFIG_DATEFONT, &CPmbClockDlg::OnConfigDatefont)
	ON_COMMAND(ID_CMD_ABOUT, &CPmbClockDlg::OnCmdAbout)
	ON_COMMAND(ID_CMD_SHOW_HIDE, &CPmbClockDlg::OnCmdShowHide)
	ON_COMMAND(ID_CMD_EXIT_APP, &CPmbClockDlg::OnCmdExitApp)
	ON_COMMAND(ID_CONFIG_START_ON_BOOT, &CPmbClockDlg::OnConfigStartOnBoot)
END_MESSAGE_MAP()


// CPmbClockDlg message handlers

BOOL CPmbClockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_hIconClock = nullptr;
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	initUI();

	BOOL bOk;
	UINT size;
	LPBYTE pbyte;
	if (!(bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"format", &pbyte, &size)))
		m_format = tpfrmtMillisecond;
	else
	{
		if (size == sizeof(m_format))
			m_format = *(tpFormat*)pbyte;
		else
			m_format = tpfrmtMillisecond;
		free(pbyte);
	}

	if (bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"wr", &pbyte, &size))
	{
		if (size == sizeof(CRect))
		{
			CRect wr = *(CRect*)pbyte;
			if (wr.Width() < PMB_WINDOW_MINIM_SIZEX)
				wr.right = wr.left + PMB_WINDOW_MINIM_SIZEX;
			if (wr.Height() < PMB_WINDOW_MINIM_SIZEY)
				wr.bottom = wr.top + PMB_WINDOW_MINIM_SIZEY;
			if (wr.bottom <= PMB_WINDOW_MINIM_SIZEY / 2)
			{
				int h = wr.Height();
				wr.bottom = PMB_WINDOW_MINIM_SIZEY / 2;
				wr.top = wr.bottom - h;
			}
			if (GetSystemMetrics(SM_CXSCREEN) - PMB_WINDOW_MINIM_SIZEX / 2 <= wr.left)
			{
				int w = wr.Width();
				wr.left = GetSystemMetrics(SM_CXSCREEN) - PMB_WINDOW_MINIM_SIZEX / 2;
				wr.right = wr.left + w;
			}
			if (GetSystemMetrics(SM_CYSCREEN) - PMB_WINDOW_MINIM_SIZEY / 2 <= wr.top)
			{
				int h = wr.Height();
				wr.top = GetSystemMetrics(SM_CYSCREEN) - PMB_WINDOW_MINIM_SIZEY / 2;
				wr.bottom = wr.top + h;
			}
			if (wr.right <= PMB_WINDOW_MINIM_SIZEX / 2)
			{
				int w = wr.Width();
				wr.right = PMB_WINDOW_MINIM_SIZEX / 2;
				wr.left = wr.right - w;
			}
			SetWindowPos(nullptr, wr.left, wr.top, wr.Width(), wr.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
		}
		free(pbyte);
	}

	if (!(bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"bkcolor", &pbyte, &size)))
		m_bkColor = RGB(0x00, 0x00, 0x00);
	else
	{
		if (size == sizeof(m_bkColor))
			m_bkColor = *(COLORREF*)pbyte;
		else
			m_bkColor = RGB(0x00, 0x00, 0x00);
		free(pbyte);
	}
	if (!(bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"color", &pbyte, &size)))
		m_color = RGB(0x00, 0x00, 0xFF);
	else
	{
		if (size == sizeof(m_color))
			m_color = *(COLORREF*)pbyte;
		else
			m_color = RGB(0x00, 0x00, 0xFF);
		free(pbyte);
	}


	if (bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"transparent", &pbyte, &size))
	{
		if (m_transparent = (size == sizeof(bool) && *pbyte))
		{
			LONG ExtendedStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
			SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle | WS_EX_LAYERED);
			::SetLayeredWindowAttributes(GetSafeHwnd(), m_bkColor, 200, LWA_COLORKEY);
		}
		free(pbyte);
	}
	else
		m_transparent = false;

	if (!(bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"bdate", &pbyte, &size)))
		m_bdate = true;
	else
	{
		if (size == sizeof(m_bdate))
			m_bdate = *(bool*)pbyte;
		else
			m_bdate = true;
		free(pbyte);
	}
	if (bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"dfont", &pbyte, &size))
	{
		if (size == sizeof(LOGFONT))
			m_dfont.CreateFontIndirect((LOGFONT*)pbyte);
		free(pbyte);
	}
	if (!(bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"dcolor", &pbyte, &size)))
		m_dColor = RGB(0xFF, 0xFF, 0xFF);
	else
	{
		if (size == sizeof(m_dColor))
			m_dColor = *(COLORREF*)pbyte;
		else
			m_dColor = RGB(0xFF, 0xFF, 0xFF);
		free(pbyte);
	}

	m_begindrag = false;
	m_yday = -1;
	m_sec = -1;

	CRect cr;
	GetClientRect(cr);
	adjust_layout(cr.Width(), cr.Height(), true);

	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	UINT mstime = _mseconds();
	SetTimer(PMB_CLOCK_TIMER_, mstime, nullptr);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CPmbClockDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}




void CPmbClockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		tm now;
		_timeb stnow;
		_ftime_s(&stnow);
		localtime_s(&now, &stnow.time);

		CPen pen;
		pen.CreatePen(PS_SOLID, 1, m_color);
		CPen* oldPen = dc.SelectObject(&pen);
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		dc.FillSolidRect(0, 0, cxIcon, cyIcon, m_bkColor);
		dc.MoveTo(x + cxIcon / 2, y + cyIcon / 2);
		dc.LineTo(x + cxIcon + cxIcon * cos(now.tm_sec * 60 / 360), y + cyIcon / 2 + cyIcon * sin(now.tm_sec * 60 / 360));
		dc.SelectObject(oldPen);
	}
	else
	{
		CPaintDC dc(this); // device context for painting

		int nDig = m_format == tpfrmtMillisecond ? 9 : m_format == tpfrmtSecond ? 6 : 4;
		for (int i = 0; i < nDig; ++i)
			m_s[i].draw(&dc);

		CBrush brush, *oldBrush = nullptr;
		int pw = m_s[1].calc_pw();
		if (1 < pw)
		{
			brush.CreateSolidBrush(m_color);
			oldBrush = dc.SelectObject(&brush);
		}

		int nSpace = m_format == tpfrmtMillisecond || m_format == tpfrmtSecond ? 2 : 1;
		for (int i = 0; i < nSpace; ++i)
			draw_separator(&dc, i, pw, m_color);

		if (oldBrush)
			dc.SelectObject(oldBrush);
	}
}


void CPmbClockDlg::draw_separator(CDC* pDC, int s, int pw, COLORREF color) const
{
	int j = (2 * s) + 1;

	int dx = 0 * (m_s[j + 1].left - m_s[j].right) / 2;
	int y = m_s[j].top + m_s[j].Height() / 3;
	int x = m_s[j].calc_x(y) - m_s[j].left + m_s[j].right + dx;
	if (pw < 2)
		pDC->SetPixel(x, y, color);
	else
	{
		CGdiObject* oldObj = pDC->SelectStockObject(NULL_PEN);
		pDC->Ellipse(x - pw, y - pw, x + pw, y + pw);
		pDC->SelectObject(oldObj);
	}

	y = m_s[j].bottom - m_s[j].Height() / 3;
	x = m_s[j].calc_x(y) - m_s[j].left + m_s[j].right + dx;
	if (pw < 2)
		pDC->SetPixel(x, y, color);
	else
	{
		CGdiObject* oldObj = pDC->SelectStockObject(NULL_PEN);
		pDC->Ellipse(x - pw, y - pw, x + pw, y + pw);
		pDC->SelectObject(oldObj);
	}
}





// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPmbClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPmbClockDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == PMB_CLOCK_TIMER_)
	{
		tm now;
		_timeb stnow;
		_ftime_s(&stnow);
		localtime_s(&now, &stnow.time);
		stnow.millitm;

		CDC* pDC = GetDC();
		if (IsIconic())
		{
			update_taskbar_icon(pDC, now);
			ReleaseDC(pDC);
			return;
		}

		CStringA s;
		s.Format("%02d", now.tm_hour);
		m_s[0].set(s[0], pDC);
		m_s[1].set(s[1], pDC);

		s.Format("%02d", now.tm_min);
		m_s[2].set(s[0], pDC);
		m_s[3].set(s[1], pDC);

		if (m_format == tpfrmtMillisecond || m_format == tpfrmtSecond)
		{
			s.Format("%02d", now.tm_sec);
			m_s[4].set(s[0], pDC);
			if (m_s[5].set(s[1], pDC))
				update_taskbar_icon(pDC, now);

			if (m_format == tpfrmtMillisecond)
			{
				s.Format("%03d", stnow.millitm);
				m_s[6].set(s[0], pDC);
				m_s[7].set(s[1], pDC);
				m_s[8].set(s[2], pDC);
			}
		}
		else
		{
			COLORREF color = now.tm_sec % 2 ? m_color : m_bkColor;

			CBrush brush, * oldBrush = nullptr;
			int pw = m_s[1].calc_pw();
			if (1 < pw)
			{
				brush.CreateSolidBrush(color);
				oldBrush = pDC->SelectObject(&brush);
			}
			draw_separator(pDC, 0, pw, color);

			if (oldBrush)
				pDC->SelectObject(oldBrush);

			update_taskbar_icon(pDC, now);
		}

		if (!IsIconic() && m_bdate && m_yday != now.tm_yday)
		{
			CRect cr;
			GetClientRect(cr);

			m_yday = now.tm_yday;
		
			CTime t = CTime::GetCurrentTime();
			SYSTEMTIME sysTime;
			t.GetAsSystemTime(sysTime);
			WCHAR tstr[_MAX_PATH], sdtfrmt[_MAX_PATH];
			GetLocaleInfo(LOCALE_NAME_USER_DEFAULT, LOCALE_SLONGDATE, sdtfrmt, _MAX_PATH);
			GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, NULL, &sysTime, sdtfrmt, tstr, _MAX_PATH, nullptr);
			CString date = tstr;

			if (!m_dfont.m_hObject)
				m_dfont.CreatePointFont(180, L"Monotype Corsiva", pDC);
			CFont* oldFont = pDC->SelectObject(&m_dfont);
			CSize dr = pDC->GetTextExtent(date);
			if (m_s[1].bottom + dr.cy <= cr.Height())
			{
				cr.top = m_s[0].bottom + (cr.bottom - m_s[0].bottom) / 2 - dr.cy / 2;
				cr.bottom = cr.top + dr.cy;
				cr.left = (cr.Width() - dr.cx) / 2;
				cr.right = cr.left + dr.cx;
				COLORREF bkColor = pDC->SetBkColor(m_bkColor),
					color = pDC->SetTextColor(m_dColor);
				pDC->DrawText(date, cr, DT_SINGLELINE | DT_LEFT);
				pDC->SetBkColor(bkColor);
				pDC->SetTextColor(color);
			}
			pDC->SelectObject(oldFont);
		}
		ReleaseDC(pDC);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CPmbClockDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	adjust_layout(cx, cy, false);
	RedrawWindow();
}


BOOL CPmbClockDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect cr;
	GetClientRect(cr);
	pDC->FillSolidRect(cr, m_bkColor);
	m_yday = -1;

	return true;// CDialogEx::OnEraseBkgnd(pDC);
}




void CPmbClockDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	m_begindrag = true;
	m_point = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPmbClockDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_begindrag)
	{
		if (m_point != point)
		{
			CRect cr;
			GetClientRect(cr);
			bool bError = false;
			if (cr.Width() - 15 < m_point.x && m_point.x < cr.Width() + 15 && cr.Height() - 15 < m_point.y && m_point.y < cr.Height() + 15)
			{
				if (!(bError = cr.Width() + point.x - m_point.x < PMB_WINDOW_MINIM_SIZEX || cr.Height() + point.y - m_point.y < PMB_WINDOW_MINIM_SIZEY))
				{
					SetWindowPos(&CWnd::wndTopMost, 0, 0, cr.Width() + point.x - m_point.x, cr.Height() + point.y - m_point.y, SWP_NOMOVE);
					m_point = CPoint(cr.Width() + point.x - m_point.x, cr.Height() + point.y - m_point.y);
					m_yday = -1;
				}
			}
			else if (cr.Height() - 10 < m_point.y && m_point.y < cr.Height() + 10)
			{
				if (!(bError = cr.Height() + point.y - m_point.y < PMB_WINDOW_MINIM_SIZEY))
				{
					SetWindowPos(&CWnd::wndTopMost, 0, 0, cr.Width(), cr.Height() + point.y - m_point.y, SWP_NOMOVE);
					m_point.y = cr.Height() + point.y - m_point.y;
					m_yday = -1;
				}
			}
			else if (cr.Width() - 10 < m_point.x && m_point.x < cr.Width() + 10)
			{
				if (!(bError = cr.Width() + point.x - m_point.x < PMB_WINDOW_MINIM_SIZEX))
				{
					SetWindowPos(&CWnd::wndTopMost, 0, 0, cr.Width() + point.x - m_point.x, cr.Height(), SWP_NOMOVE);
					m_point.x = cr.Width() + point.x - m_point.x;
					m_yday = -1;
				}
			}
			else
			{
				ClientToScreen(&point);
				if (!(bError = GetSystemMetrics(SM_CXSCREEN) - PMB_WINDOW_MINIM_SIZEX / 2 < point.x - m_point.x || GetSystemMetrics(SM_CYSCREEN) - PMB_WINDOW_MINIM_SIZEY / 2 < point.y - m_point.y
						|| point.x - m_point.x + cr.Width() < PMB_WINDOW_MINIM_SIZEX / 2 || point.y - m_point.y + cr.Height() < PMB_WINDOW_MINIM_SIZEY / 2))
					SetWindowPos(&CWnd::wndTopMost, point.x - m_point.x, point.y - m_point.y, 0, 0, SWP_NOSIZE);
			}

			if (!bError)
			{
				GetWindowRect(cr);
				theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"wr", (LPBYTE)& cr, sizeof(cr));
			}
			return;
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}



void CPmbClockDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_begindrag)
	{
		m_begindrag = false;
		ReleaseCapture();
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CPmbClockDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_begindrag)
	{
		m_begindrag = false;
		ReleaseCapture();
	}
	ShowWindow(SW_MINIMIZE);
	m_yday = -1;
	CDialogEx::OnLButtonDblClk(nFlags, point);
}





void CPmbClockDlg::write_icon()
{
	// handle to icon
	HICON hIcon;
	// text for tool tip
	WCHAR lpszTip[] = L"Mouse is on the Icon !!";

	HINSTANCE hInst =
		AfxFindResourceHandle(MAKEINTRESOURCE(IDR_MAINFRAME), RT_GROUP_ICON);
	hIcon = (HICON)LoadImage(hInst,
		MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON,
		16,
		16,
		LR_DEFAULTCOLOR);

	// set NOTIFYCONDATA structure

	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = m_hWnd;
	tnid.uID = IDR_MAINFRAME;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = WM_TASKBAR; // my user message
	tnid.hIcon = hIcon;
	m_yday = -1;
	if (lpszTip)
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
	else
		tnid.szTip[0] = '\0';

	// call to Shell_NotifyIcon with NIM_ADD parameter

	BOOL vret = Shell_NotifyIcon(NIM_ADD, &tnid);

	// free icon 

	if (hIcon)
		DestroyIcon(hIcon);
}



void CPmbClockDlg::remove_icon()
{

	// for remove, only provide cbSize, hWnd and uID!

	NOTIFYICONDATA tnid;

	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = m_hWnd;
	tnid.uID = IDR_MAINFRAME;

	// call to Shell_NotifyIcon with NIM_DEL parameter

	Shell_NotifyIcon(NIM_DELETE, &tnid);
}


UINT CPmbClockDlg::_mseconds() const
{
	return m_format == tpfrmtMillisecond ? 125 : m_format == tpfrmtSecond ? 500 : 1000;
}


afx_msg LRESULT CPmbClockDlg::OnTaskbar(WPARAM wParam, LPARAM lParam)
{
	UINT uMouseMsg = (UINT)lParam;

	switch (uMouseMsg)
	{
	case WM_LBUTTONDOWN:
		AfxMessageBox(L"Mouse click on the Icon !");
		break;
	default:
		break;
	}
	return 0;
}




void CPmbClockDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_CONFIG);
	if (menu.m_hMenu)
	{
		CMenu* pSubMenu = menu.GetSubMenu(0);
		if (m_format == tpfrmtMillisecond)
			pSubMenu->CheckMenuItem(ID_CONFIG_MILLISECONDS, MF_CHECKED | MF_BYCOMMAND);
		else if (m_format == tpfrmtSecond)
			pSubMenu->CheckMenuItem(ID_CONFIG_SECONDS, MF_CHECKED | MF_BYCOMMAND);
		else
			pSubMenu->CheckMenuItem(ID_CONFIG_MINUTES, MF_CHECKED | MF_BYCOMMAND);
		if (m_bdate)
			pSubMenu->CheckMenuItem(ID_CONFIG_DATE, MF_CHECKED | MF_BYCOMMAND);
		if (m_transparent)
			pSubMenu->CheckMenuItem(ID_CONFIG_TRANSPARENT, MF_CHECKED | MF_BYCOMMAND);
		if (m_startupBoot)
			pSubMenu->CheckMenuItem(ID_CONFIG_START_ON_BOOT, MF_CHECKED | MF_BYCOMMAND);
		if (m_showHide)
			pSubMenu->CheckMenuItem(ID_CMD_SHOW_HIDE, MF_CHECKED | MF_BYCOMMAND);
		ClientToScreen(&point);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CPmbClockDlg::OnConfigMilliseconds()
{
	if (m_format != tpfrmtMillisecond)
	{
		KillTimer(PMB_CLOCK_TIMER_);
		m_format = tpfrmtMillisecond;
		CRect cr;
		GetClientRect(cr);

		adjust_layout(cr.Width(), cr.Height(), true);
		SetTimer(PMB_CLOCK_TIMER_, _mseconds(), nullptr);
		RedrawWindow();
		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"format", (LPBYTE)&m_format, sizeof(m_format));
	}
}


void CPmbClockDlg::OnConfigSeconds()
{
	if (m_format != tpfrmtSecond)
	{
		KillTimer(PMB_CLOCK_TIMER_);
		m_format = tpfrmtSecond;
		CRect cr;
		GetClientRect(cr);

		adjust_layout(cr.Width(), cr.Height(), true);
		SetTimer(PMB_CLOCK_TIMER_, _mseconds(), nullptr);
		RedrawWindow();
		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"format", (LPBYTE)&m_format, sizeof(m_format));
	}
}


void CPmbClockDlg::OnConfigMinutes()
{
	if (m_format != tpfrmtMinute)
	{
		KillTimer(PMB_CLOCK_TIMER_);
		m_format = tpfrmtMinute;
		CRect cr;
		GetClientRect(cr);

		adjust_layout(cr.Width(), cr.Height(), true);
		SetTimer(PMB_CLOCK_TIMER_, _mseconds(), nullptr);
		RedrawWindow();
		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"format", (LPBYTE)&m_format, sizeof(m_format));
	}
}



void CPmbClockDlg::OnConfigTransparent()
{
	m_transparent = !m_transparent;
	if (m_transparent)
	{
		LONG ExtendedStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
		SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(GetSafeHwnd(), m_bkColor, 200, LWA_COLORKEY);
	}
	else
	{
		LONG ExtendedStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
		SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle & ~WS_EX_LAYERED);
	}
	theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"transparent", (LPBYTE)&m_transparent, sizeof(m_transparent));
}



void CPmbClockDlg::OnConfigBackcolor()
{
	CMFCColorDialog dlg;
	dlg.SetCurrentColor(m_bkColor);
	if (dlg.DoModal() == IDOK)
	{
		m_bkColor = dlg.GetColor();
		for (int i = 0; i < 9; ++i)
			m_s[i].set(m_bkColor, m_color);
		RedrawWindow();

		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"bkcolor", (LPBYTE)&m_bkColor, sizeof(m_bkColor));
	}
}



void CPmbClockDlg::OnConfigColor()
{
	CMFCColorDialog dlg;
	dlg.SetCurrentColor(m_color);
	if (dlg.DoModal() == IDOK)
	{
		m_color = dlg.GetColor();
		for (int i = 0; i < 9; ++i)
			m_s[i].set(m_bkColor, m_color);
		RedrawWindow();

		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"color", (LPBYTE)&m_color, sizeof(m_color));
	}
}


void CPmbClockDlg::OnConfigDate()
{
	m_bdate = !m_bdate;
	m_yday = -1;
	RedrawWindow();

	theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"bdate", (LPBYTE)&m_bdate, sizeof(m_bdate));
}



void CPmbClockDlg::OnConfigDatefont()
{
	LOGFONT lf;
	m_dfont.GetLogFont(&lf);
	CFontDialog dlg(&lf);
	if (dlg.DoModal() == IDOK)
	{
		if (m_dfont.m_hObject)
			m_dfont.DeleteObject();
		LOGFONT lf;
		dlg.GetCurrentFont(&lf);
		m_dfont.CreateFontIndirect(&lf);

		m_dColor = dlg.GetColor();
		m_yday = -1;
		RedrawWindow();

		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"dcolor", (LPBYTE)&m_dColor, sizeof(m_dColor));
		theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"dfont", (LPBYTE)&lf, sizeof(lf));
	}
}


void CPmbClockDlg::initUI()
{
	// (1)load config
	BOOL bOk;
	UINT size;
	LPBYTE pbyte;


	if (bOk = theApp.GetProfileBinary(_T(PROFILE_REGISTRY), L"startupBoot", &pbyte, &size))
	{
		m_startupBoot = true;
		free(pbyte);
	}
	else
		m_startupBoot = false;

	m_showHide = true;

	// (2)hide taskbar on windows
	LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	exStyle &= ~WS_EX_APPWINDOW;   // 移除任务栏标志
	exStyle |= WS_EX_TOOLWINDOW;   // 工具窗口
	SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle);

  // (3)show tray icon and menu
  AddTrayIcon();
}

void CPmbClockDlg::OnCmdAbout()
{
  CString aboutMsg;
  aboutMsg.Format(L"PMB Clock 0.0.2\r\n\r\nBuild date: %S\r\nBuild time: %S\r\n\r\n(c) 2026 Some Software", __DATE__, __TIME__);
  AfxMessageBox(aboutMsg, MB_OK | MB_ICONINFORMATION);
}

void CPmbClockDlg::OnCmdShowHide()
{
	m_showHide = !m_showHide;
  // show or hide main window
  if (m_showHide)
    ShowWindow(SW_SHOW);
  else
    ShowWindow(SW_HIDE);
}

void CPmbClockDlg::OnCmdExitApp()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void CPmbClockDlg::OnConfigStartOnBoot()
{
	m_startupBoot = !m_startupBoot;
	theApp.WriteProfileBinary(_T(PROFILE_REGISTRY), L"startupBoot", (LPBYTE)&m_startupBoot, sizeof(m_startupBoot));
}

void CPmbClockDlg::AddTrayIcon()
{
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = m_hWnd;
    nid.uID = IDR_MAINFRAME;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    _tcscpy_s(nid.szTip, _T("PmbClock"));

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void CPmbClockDlg::RemoveTrayIcon()
{
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = m_hWnd;
    nid.uID = IDR_MAINFRAME;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}
