
// PmbClockDlg.h : header file
//

#pragma once

#include "seven_segment.h"


#define WM_TASKBAR	(WM_USER + 1)

#define PMB_WINDOW_MINIM_SIZEX	60
#define PMB_WINDOW_MINIM_SIZEY	30


// CPmbClockDlg dialog
class CPmbClockDlg : public CDialogEx
{
	enum tpFormat : char
	{
		tpfrmtMillisecond,
		tpfrmtSecond,
		tpfrmtMinute
	};

// Construction
public:
	CPmbClockDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PMBCLOCK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	void adjust_layout(int cx, int cy, bool bFirstset);
	void update_taskbar_icon(CDC* pDC, const tm& now);

	void draw_separator(CDC* pDC, int s, int pw, COLORREF color) const;

	void write_icon();
	void remove_icon();

	UINT _mseconds() const;

// Implementation
protected:
	tpFormat m_format;
	COLORREF m_bkColor;
	COLORREF m_color;
	seven_segment m_s[9];

	HICON m_hIcon;
	HICON m_hIconClock;
	HBITMAP m_hBitmap;

	bool m_transparent;
	bool m_bdate;
	CFont m_dfont;
	COLORREF m_dColor;

	int m_yday;
	int m_sec;

	bool m_begindrag;
	CPoint m_point;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnTaskbar(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnConfigMilliseconds();
	afx_msg void OnConfigSeconds();
	afx_msg void OnConfigMinutes();
	afx_msg void OnConfigTransparent();
	afx_msg void OnConfigBackcolor();
	afx_msg void OnConfigColor();
	afx_msg void OnConfigDate();
	afx_msg void OnConfigDatefont();
};
