#pragma once
/*
	brief:	使用静态窗口显示每个单独的数据，
		既可以显示图片，也可以显示文字
*/

// CDataWnd

class CDataWnd : public CWnd
{
	DECLARE_DYNAMIC(CDataWnd)

public:
	CDataWnd();
	virtual ~CDataWnd();

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CString m_wndClassName;

	int				m_closeWndSize;
	RECT			m_closeWnd;
	TEXTMETRIC		m_textMetric;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
