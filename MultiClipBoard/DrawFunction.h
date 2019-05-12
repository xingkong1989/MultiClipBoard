#pragma once


/**
	@function:	过滤无效字符串

	@param wzSrc			源字符串
	@param cSrc				源字符串长度
	@param wzDest			目的字符串
	@param cDest			目的字符串长度
	@parm invalidChar		无效的字符，以CString形式传入

@return		过滤后有效的字符串长度
*/
int FilterInValidCharW(const wchar_t* wzSrc, int cSrc, wchar_t* wzDest, int cDest, CString invalidChar);

/**
	@function:	在矩形框上绘制文字

	@param wzStr			需要绘制的文字，编码unicode
	@param pDc				CDC 指针
	@param rt				矩形框范围
	@parm invalidChar		无效的字符，以CString形式传入

	@return		返回成功或者失败
*/
bool DrawClipboardText(const wchar_t* wzStr, CDC* pDc, RECT *rt, CString invalidChar);

/**
	@functino:	在矩形框中绘制位图，根据矩形框的高度选择适当的宽度

	@param pDC				CDC指针
	@param hBitmap			位图句柄
	@param rt				矩形框范围，并范围实际使用的范围

	@return		返回成功或者失败
*/
BOOL DrawBitmap(CDC* pDC, const HBITMAP hBitmap, RECT *rt);
