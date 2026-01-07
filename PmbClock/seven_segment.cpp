#include "pch.h"
#include "seven_segment.h"







seven_segment::rcalc::rcalc(const CRect& r)
	: CRect(r)
{
}










seven_segment::calc::calc()
	: border(10), m(5)
{
}


void seven_segment::calc::set(const CRect& r)
{
	xb = r.Width() / border;

	xD = r.left + xb;
	yD = r.bottom - xb;

	xm = ((r.right - xb) - xD) / m;
	ym = yD - (r.top + xb);

	pw = 2 * (ym < 60 ? 1 : ym / 30);
}


void seven_segment::calc::set(rcalc& r)
{
	xb = r.Width() / border;

	r.left += xb;
	r.right -= xb;
	r.top += xb;
	r.bottom -= xb;
	xm = r.Width() / m;
	ym = r.Height();

	pw = 2 * (ym < 60 ? 1 : ym / 30);

	xD = r.left;
	yD = r.bottom;
	r.xA = xD + xm;
	r.yA = r.top;
	r.xB = r.right;
	r.yG = r.yA + ym / 2;
	r.xG = xD + (yD - r.yG) * xm / ym;
}


int seven_segment::calc::operator()(int y) const
{
	return xD + (yD - y) * xm / ym;
}


















seven_segment::seven_segment()
{
}


seven_segment::seven_segment(const CRect& r, COLORREF bkcolor, COLORREF color)
	: CRect(r), m_bkColor(bkcolor), m_color(color)
{
}



void seven_segment::set(const CRect& r)
{
	*static_cast<CRect*>(this) = r;
	NormalizeRect();
}


void seven_segment::set(const CRect& r, COLORREF bkcolor, COLORREF color, bool bdot)
{
	*static_cast<CRect*>(this) = r;
	NormalizeRect();

	m_dot = bdot;
	m_bkColor = bkcolor;
	m_color = color;
}


void seven_segment::set(COLORREF bkcolor, COLORREF color)
{
	m_bkColor = bkcolor;
	m_color = color;
}


bool seven_segment::set(char nv, CDC* pDC)
{
	bool br = nv != m_val;
	if (br)
	{
		m_val = nv;
		if (pDC)
			draw(pDC);
	}
	return br;
}


void seven_segment::draw(CDC* pDC) const
{
	if (!Width() || !Height())
		return;

	bool sA = m_val == '0'                 || m_val == '2' || m_val == '3'                 || m_val == '5' || m_val == '6' || m_val == '7' || m_val == '8' || m_val == '9',
		 sB = m_val == '0' || m_val == '1' || m_val == '2' || m_val == '3' || m_val == '4'                                 || m_val == '7' || m_val == '8' || m_val == '9',
		 sC = m_val == '0' || m_val == '1'                 || m_val == '3' || m_val == '4' || m_val == '5' || m_val == '6' || m_val == '7' || m_val == '8' || m_val == '9',
		 sD = m_val == '0'                 || m_val == '2' || m_val == '3'                 || m_val == '5' || m_val == '6'                 || m_val == '8' || m_val == '9',
		 sE = m_val == '0'                 || m_val == '2'                                                 || m_val == '6'                 || m_val == '8'                ,
		 sF = m_val == '0'                                                 || m_val == '4' || m_val == '5' || m_val == '6'                 || m_val == '8' || m_val == '9',
		 sG =                                 m_val == '2' || m_val == '3' || m_val == '4' || m_val == '5' || m_val == '6'                 || m_val == '8' || m_val == '9';


	pDC->FillSolidRect(this, m_bkColor);

	rcalc r(*this);
	calc c;

	c.set(r);
	CPen pen;
	pen.CreatePen(PS_SOLID, c.pw, m_color);
	CPen* oldPen = pDC->SelectObject(&pen);

	if (sA)
	{
		pDC->MoveTo(r.xA + c.pw, r.yA);
		pDC->LineTo(r.xB - c.pw, r.yA);
	}
	if (sB)
	{
		pDC->MoveTo(r.xB, r.yA + c.pw);
		pDC->LineTo(r.xG + r.Width() - c.xm, r.yG - c.pw);
	}
	if (sC)
	{
		pDC->MoveTo(r.xG + r.Width() - c.xm, r.yG + c.pw);
		pDC->LineTo(c.xD + r.Width() - c.xm, c.yD - c.pw);
	}
	if (sD)
	{
		pDC->MoveTo(c.xD + c.pw, c.yD);
		pDC->LineTo(c.xD + r.Width() - c.xm - c.pw, c.yD);
	}
	if (sE)
	{
		pDC->MoveTo(r.xG, r.yG + c.pw);
		pDC->LineTo(c.xD, c.yD - c.pw);
	}
	if (sF)
	{
		pDC->MoveTo(r.xA, r.yA + c.pw);
		pDC->LineTo(r.xG, r.yG - c.pw);
	}
	if (sG)
	{
		pDC->MoveTo(r.xG + c.pw, r.yG);
		pDC->LineTo(r.xG + r.Width() - c.xm - c.pw, r.yG);
	}
	if (m_dot)
	{
		if (c.pw < 2)
		{
			pDC->MoveTo(r.right + 1, r.bottom - 1);
			pDC->LineTo(r.right - 1, r.bottom);
		}
		else
		{
			CBrush brush;
			brush.CreateSolidBrush(m_color);
			CBrush* oldBrush = pDC->SelectObject(&brush);
			pDC->Ellipse(r.right - c.pw / 2, r.bottom - c.pw / 2, r.right + c.pw / 2, r.bottom + c.pw / 2);
			pDC->SelectObject(oldBrush);
		}
	}
	pDC->SelectObject(oldPen);
}


int seven_segment::calc_x(int y) const
{
	calc c;
	c.set(this);

	return c(y);
}


int seven_segment::calc_pw() const
{
	calc c;
	c.set(this);

	return c.pw;
}



