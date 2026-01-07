#pragma once
#include <atltypes.h>






struct seven_segment : CRect
{
protected:
	struct rcalc : CRect
	{
		rcalc(const CRect& r);

		int	xA,
			yA,
			xB,
			yG,
			xG;
	};


	struct calc
	{
		calc();

		void set(const CRect& r);
		void set(rcalc& r);

		int operator()(int y) const;

		const int border;
		const int m;

		int xb;
		int xD;
		int yD;

		int xm;
		int ym;

		int pw;
	};


public:


	seven_segment();
	seven_segment(const CRect& r, COLORREF bkcolor, COLORREF color);

	void set(const CRect& r);
	void set(const CRect& r, COLORREF bkcolor, COLORREF color, bool bdot);
	void set(COLORREF bkcolor, COLORREF color);

	bool set(char nv, CDC* pDC);
	void draw(CDC* pDC) const;

	int calc_x(int y) const;
	int calc_pw() const;

protected:
	bool m_dot;
	char m_val;
	COLORREF m_bkColor;
	COLORREF m_color;
};

