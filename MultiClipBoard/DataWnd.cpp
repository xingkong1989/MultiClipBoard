// DataWnd.cpp: 实现文件
//

#include "stdafx.h"
#include "DataWnd.h"


// ClipboardWnd

IMPLEMENT_DYNAMIC(ClipboardWnd, CWnd)

ClipboardWnd::ClipboardWnd()
{
	m_wndClassName = _T("STATIC");
	m_id = -1;
}

ClipboardWnd::~ClipboardWnd()
{
}

BOOL ClipboardWnd::operator == (ClipboardWnd data)
{
	if (m_id != data.m_id)
	{
		return CWnd::operator==(data);
	}
	return TRUE;
}

ClipboardWnd& ClipboardWnd::operator= (const ClipboardWnd& other)
{
	if (other.m_id != m_id)
	{
		for (INT_PTR i = 0; i < other.m_clipboardArray.GetSize(); i++)
		{
			m_clipboardArray.Add(other.m_clipboardArray.GetAt(i));
		}
		m_id = other.m_id;
	}

	return *this;
}


BEGIN_MESSAGE_MAP(ClipboardWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()



// ClipboardWnd 消息处理程序


LRESULT ClipboardWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWnd::WindowProc(message, wParam, lParam);
}


void ClipboardWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()

	// 计算关闭按钮的位置
	RECT rect = { 0 };
	POINT roundPoint = { 2,2 };	// 弧度
	int padding = (m_closeWndSize - m_textMetric.tmAveCharWidth) / 2;

	GetWindowRect(&rect);
	dc.SetBkMode(TRANSPARENT);
	dc.RoundRect(&m_closeWnd, roundPoint);
	dc.TextOut(m_closeWnd.left + padding, m_closeWnd.top, _T("X"));

	CRect clientRc;
	GetClientRect(clientRc);
	DrawClipboardWnd(&dc, clientRc);
}


void ClipboardWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnClose();
}


void ClipboardWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (YunXing::YxWindowUtility::PointInRect(&point, &m_closeWnd))
	{
		OnClose();
		return;
	}
	CWnd::OnLButtonDown(nFlags, point);
}


int ClipboardWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (lpCreateStruct->x < 0 || lpCreateStruct->cx < 20
		|| lpCreateStruct->y < 0 || lpCreateStruct->cy < 20)
	{
		return -1;
	}

	lpCreateStruct->style = WS_CHILD | WS_VISIBLE ;
	lpCreateStruct->lpszClass = m_wndClassName.GetBuffer();
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 算出关闭按钮的大小及位置
	GetTextMetrics(::GetWindowDC(m_hWnd), &m_textMetric);
	m_closeWndSize = m_textMetric.tmAveCharWidth + 10;

	m_textColor = RGB(0, 240, 240);
	m_textWidth = 1;
	m_textSize = 20;

	return 0;
}


BOOL ClipboardWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}


void ClipboardWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_RESTORED)
	{
		m_closeWnd.right = cy - 2;
		m_closeWnd.top = 2;
		m_closeWnd.left = m_closeWnd.right - m_closeWndSize;
		m_closeWnd.bottom = m_closeWnd.top + m_closeWndSize;
	}
}


BOOL ClipboardWnd::DrawClipboardWnd(CDC* pDc, CRect &rect)
{
	int dwCount = m_clipboardArray.GetSize();
	const ClipboardData* pData = NULL;

	for (int i = 0; i < dwCount; i++)
	{
		pData = &m_clipboardArray.GetAt(i);

		if (DrawClipboardData(*pData, pDc, rect))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL ClipboardWnd::DrawClipboardData(const ClipboardData& clipboardData, CDC* pDc, _Inout_  CRect &rect)
{
	BOOL bSuccess = FALSE;
	wchar_t* wzText = nullptr;

	do
	{
		switch (clipboardData.m_dwFormat)
		{
			// 文本格式
		case CF_TEXT:
		case CF_OEMTEXT:
		case CF_UNICODETEXT:
		{
			int cvalid = 0;
			wzText = new wchar_t[clipboardData.m_dwDataSize + 1];
			if (wzText)
			{
				wmemset(wzText, 0, clipboardData.m_dwDataSize + 1);
				if (clipboardData.m_dwFormat == CF_TEXT || clipboardData.m_dwFormat == CF_OEMTEXT)
				{
					MultiByteToWideChar(CP_ACP, 0, (char*)clipboardData.m_lpData, clipboardData.m_dwDataSize, wzText, clipboardData.m_dwDataSize + 1);
				}
				else
				{
					wmemcpy_s(wzText, clipboardData.m_dwDataSize + 1, (wchar_t*)clipboardData.m_lpData, clipboardData.m_dwDataSize);
				}

				CPen pen;
				pen.CreatePen(PS_SOLID, 1, m_textColor);
				HGDIOBJ hOldObj = NULL;
				hOldObj = pDc->SelectObject(pen.m_hObject);
				DrawClipboardText(wzText, pDc, &rect, m_sInvalidChar);
				pDc->SelectObject(hOldObj);
				delete[] wzText;
			}
		}
		break;

		// 位图句柄(HBITMAP)
		case CF_BITMAP:
		{
			HBITMAP hTmpBitmap = NULL;
			hTmpBitmap = (HBITMAP)clipboardData.m_lpData;
			DrawBitmap(pDc, hTmpBitmap, &rect);
		}
		break;

		// BITMAPINFO 结构体信息 (似乎不能绘制)
		case CF_DIB:
		{
			BITMAPINFO* pBitmapInfo = NULL;
			pBitmapInfo = (PBITMAPINFO)clipboardData.m_lpData;
			if (pBitmapInfo->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
			{
				break;
			}

		}
		break;

		// BITMAPV5HEADER 结构体信息 (似乎不能绘制)
		case CF_DIBV5:
		{
			PBITMAPV5HEADER pDIBV5 = NULL;
			pDIBV5 = (PBITMAPV5HEADER)clipboardData.m_lpData;
			if (pDIBV5->bV5Size != sizeof(BITMAPV5HEADER))
			{
				break;
			}
		}
		break;

		// 增强图元文件句柄(HENHMETAFILE)
		case CF_ENHMETAFILE:
		{
			WCHAR wzDescription[MAX_PATH] = { 0 };
			HENHMETAFILE hEnMetafile = NULL;
			hEnMetafile = (HENHMETAFILE)clipboardData.m_lpData;
			if (hEnMetafile == NULL)
			{
				break;
			}
			GetEnhMetaFileDescriptionW(hEnMetafile, MAX_PATH, wzDescription);
			PlayEnhMetaFile(pDc->m_hDC, hEnMetafile, &rect);
			OutPutString(L"图元文件描述：%s", wzDescription);
		}
		break;

		// 拖拽文件句柄(HDROP)
		case CF_HDROP:
		{
			HDROP hDrop = NULL;
			hDrop = (HDROP)clipboardData.m_lpData;
			if (hDrop == NULL)
			{
				break;
			}

			CString sShowString = L"拷贝文件：";
			WCHAR wzFilePath[MAX_PATH] = { 0 };
			int nCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
			for (int i = 0; i < nCount; i++)
			{
				if (DragQueryFileW(hDrop, i, wzFilePath, MAX_PATH) > 0)
				{
					sShowString.Append(wzFilePath);
					sShowString.Append(L"\r\n");
				}
			}

			DrawClipboardText(sShowString.GetBuffer(), pDc, &rect, m_sInvalidChar);
		}
		break;

		// 图元图片的句柄
		case CF_METAFILEPICT:
		{

		}
		break;

		// 比波形音频更复杂的音频数据
		case CF_RIFF:
		{

		}
		break;

		// 微软符号链接格式
		case CF_SYLK:
		{

		}
		break;

		// 标签图片文件
		case CF_TIFF:
		{

		}
		break;

		// 标准的音频数据
		case CF_WAVE:
		{

		}
		break;

		default:
			break;
		}

	} while (FALSE);

	return bSuccess;
}



int FilterInValidCharW(const wchar_t* wzSrc, int cSrc, wchar_t* wzDest, int cDest, CString invalidChar)
{
	int ret = 0;
	int i = 0, j = 0;

	do
	{
		if (wzSrc == nullptr || wzDest == nullptr)
		{
			break;
		}
		for (; i < cSrc && j < cDest; i++, j++)
		{
			for (int k = 0; k < invalidChar.GetLength() && i < cSrc; k++)
			{
				if (wzSrc[i] == invalidChar[k])
				{
					i++;
				}
			}
			wzDest[j] = wzSrc[i];
		}
		if (cDest > j)
		{
			wzDest[j] = 0;
		}
		else
		{
			wzDest[cDest] = 0;
		}
		ret = j;
	} while (false);

	return ret;
}

bool DrawClipboardText(const wchar_t* wzStr, CDC* pDc, RECT* rt, CString invalidChar)
{
	bool bRet = false;
	int iStrWidth = 0, iStrLen = 0, iCharMaxPerLine = 0;
	int iRemainder, iRow, iMaxRow;
	TEXTMETRIC tm;
	int cValid = 0;

	do
	{
		if (pDc == nullptr || rt == nullptr)
		{
			break;
		}
		if (rt->right - rt->left == 0 || rt->bottom - rt->top == 0)
		{
			break;
		}

		iStrLen = wcslen(wzStr);

		WCHAR* tmp = new WCHAR[iStrLen + 1];
		if (tmp == NULL)
		{
			break;
		}
		cValid = FilterInValidCharW(wzStr, iStrLen, tmp, iStrLen + 1, invalidChar);

		GetTextMetrics(pDc->m_hDC, &tm);
		iStrWidth = tm.tmAveCharWidth * cValid;
		iRow = iStrWidth / (rt->right - rt->left);
		iCharMaxPerLine = (rt->right - rt->left) / tm.tmAveCharWidth;
		if (iCharMaxPerLine <= 0)
		{
			break;
		}

		iRemainder = iStrWidth % (rt->right - rt->left);
		if (iRemainder > 0)
		{
			iRow++;
		}

		iMaxRow = (rt->bottom - rt->top) / (tm.tmExternalLeading + tm.tmHeight);
		iRow = min(iMaxRow, iRow);

		wchar_t* wzLine = NULL;
		wzLine = new wchar_t[iCharMaxPerLine + 1];

		for (int i = 0; i < iRow; i++)
		{
			wmemset(wzLine, 0, iCharMaxPerLine + 1);
			wmemcpy_s(wzLine, iCharMaxPerLine + 1, tmp + i * iCharMaxPerLine, iCharMaxPerLine);
			pDc->TextOutW(rt->left, rt->top + i * (tm.tmExternalLeading + tm.tmHeight), wzLine);
		}

		delete[] wzLine;
		if (tmp)
		{
			delete[] tmp;
		}
		bRet = true;
	} while (false);

	return bRet;
}

BOOL DrawBitmap(CDC* pDC, const HBITMAP hBitmap, RECT *rt)
{
	BOOL bSuccess = FALSE;

	do
	{
		if (pDC == NULL || hBitmap == NULL || rt == NULL)
		{
			break;
		}

		CDC compatibleDC;
		if (!compatibleDC.CreateCompatibleDC(pDC))
		{
			break;
		}
		HBITMAP hOldBitmap = (HBITMAP)compatibleDC.SelectObject(hBitmap);
		BITMAP bmp;

		if (GetObject(hBitmap, sizeof(BITMAP), &bmp) <= 0)
		{
			break;
		}
		int iDestWidth, iDestHeight;
		double dScale = 0;
		iDestWidth = rt->right - rt->left;
		iDestHeight = rt->bottom - rt->top;

		// 如果宽度大于高度则改变传入矩形的高度，反之改变宽度
		if (bmp.bmWidth > bmp.bmHeight)
		{
			dScale = iDestWidth / bmp.bmWidth;
			iDestHeight = (int)(dScale * bmp.bmHeight);
			rt->bottom = rt->top + iDestHeight;
		}
		else
		{
			dScale = iDestHeight / bmp.bmHeight;
			iDestWidth = (int)(dScale * bmp.bmWidth);
			rt->right = rt->left + iDestWidth;
		}
		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(rt->left, rt->top, iDestWidth, iDestHeight, &compatibleDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

		compatibleDC.SelectObject(hOldBitmap);

	} while (FALSE);

	return bSuccess;
}


void ClipboardWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

}


void ClipboardWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

}


LRESULT ClipboardWnd::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnNcHitTest(point);
}
