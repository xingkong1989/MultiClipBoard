#pragma once
/*
	brief:	使用静态窗口显示每个单独的数据，
		既可以显示图片，也可以显示文字
*/
#include "ClipboardData.h"

typedef CArray<ClipboardData> ClipboardArray;

// ClipboardWnd

class ClipboardWnd : public CWnd
{
	DECLARE_DYNAMIC(ClipboardWnd)

public:
	ClipboardWnd();
	virtual ~ClipboardWnd();

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	/**
	@function:	在窗口上绘制ClipboardList中的数据

	@param clipboardList	剪切板数据链表
	@param pDc				为绘制目标句柄dc句柄
	@param rect				矩形框大小，传入矩形框的大小，并且返回实际所用的大小
	同DrawClipboardData 的 第三个参数

	@return		返回成功或者失败
	*/
	BOOL DrawClipboardWnd(CDC* pDc, CRect &rect);


	/**
	@function:	绘制一个ClipboardData中的数据

	@param clipboardData	数据内容
	@param pDc				为绘制的目标dc句柄，可以为空
	@param rect				矩形框大小，传入矩形框的大小，并且返回实际所用的大小

	@return		返回成功或者失败
	*/
	BOOL DrawClipboardData(const ClipboardData& clipboardData, CDC* pDc, _Inout_  CRect &rect);

	// 重载== 和 =
	BOOL operator == (ClipboardWnd data);
	ClipboardWnd& operator= (const ClipboardWnd& other);

	int				m_closeWndSize;
	RECT			m_closeWnd;
	TEXTMETRIC		m_textMetric;
	DWORD			m_id;
	ClipboardArray	m_clipboardArray;

private:
	CString m_sInvalidChar;
	CString m_wndClassName;

	COLORREF m_textColor;						// 文字的颜色
	int m_textWidth;							// 文字的字体宽度
	int m_textSize;								// 文字的大小

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
};


/**
	@function:	过滤无效字符串

	@param_1: wzSrc			源字符串
	@param_2: cSrc				源字符串长度
	@param_3: wzDest			目的字符串
	@param_4: cDest			目的字符串长度
	@param_5: invalidChar		无效的字符，以CString形式传入

@return		过滤后有效的字符串长度
*/
int FilterInValidCharW(const wchar_t* wzSrc, int cSrc, wchar_t* wzDest, int cDest, CString invalidChar);

/**
	@function:	在矩形框上绘制文字

	@param_1: wzStr			需要绘制的文字，编码unicode
	@param_2: pDc				CDC 指针
	@param_3: rt				矩形框范围
	@param_4: invalidChar		无效的字符，以CString形式传入

	@return		返回成功或者失败
*/
bool DrawClipboardText(const wchar_t* wzStr, CDC* pDc, RECT *rt, CString invalidChar);

/**
	@functino:	在矩形框中绘制位图，根据矩形框的高度选择适当的宽度

	@param_1: pDC				CDC指针
	@param_2: hBitmap			位图句柄
	@param_3: rt				矩形框范围，并范围实际使用的范围

	@return		返回成功或者失败
*/
BOOL DrawBitmap(CDC* pDC, const HBITMAP hBitmap, RECT *rt);
