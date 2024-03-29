// DataWnd.cpp: 实现文件
//

#include "stdafx.h"
#include "ClipboardWnd.h"


// ClipboardWnd

IMPLEMENT_DYNAMIC(ClipboardWnd, CStatic)

ClipboardWnd::ClipboardWnd()
{
	m_id = -1;
	m_lBtnDownClose = FALSE;
	m_currentSelected = FALSE;
	m_mouseMove = FALSE;
	
	m_textWidth = 1;
	m_textSize = 20;

	m_cfg = CConfig::GetInstance();

	m_frameWidth = 4;
	m_sInvalidChar = L"\r\n\t";
}

ClipboardWnd::~ClipboardWnd()
{
}

BOOL ClipboardWnd::operator == (ClipboardWnd data)
{
	if (m_id != data.m_id)
	{
		return CStatic::operator==(data);
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


BEGIN_MESSAGE_MAP(ClipboardWnd, CStatic)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_SWITCHCLIPBOARD, &ClipboardWnd::OnSwitchclipboard)
	ON_WM_MOUSEMOVE()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// ClipboardWnd 消息处理程序


LRESULT ClipboardWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CStatic::WindowProc(message, wParam, lParam);
}


void ClipboardWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()

	CPen pen;				// 边框
	CBrush brush, nullBrush, *oldBrush = NULL;			// 背景
	RECT rect = { 0 };

	GetClientRect(&rect);
	// 窗口被选中
	if (m_currentSelected)
	{
		brush.CreateSolidBrush(m_cfg->m_selectedColor);
	}
	else
	{
		brush.CreateSolidBrush(m_cfg->m_noSelectedColor);
	}
	// 背景
	dc.FillRect(&m_drawRect, &brush);

	// 鼠标移动到窗口上
	if (m_mouseMove)
	{
		pen.CreatePen(0, m_frameWidth, m_cfg->m_mouseColor);
	}
	else
	{
		pen.CreatePen(0, m_frameWidth, m_cfg->m_noMouseColor);
	}

	dc.SetBkMode(TRANSPARENT);
	// 绘制边框
	CPen* oldPen = NULL;
	nullBrush.CreateStockObject(NULL_BRUSH);
	oldBrush = dc.SelectObject(&nullBrush);
	oldPen = dc.SelectObject(&pen);
	dc.Rectangle(&m_wndRect);
	dc.SelectObject(oldBrush);
	dc.SelectObject(oldPen);

	DrawClipboardWnd(&dc, m_drawRect);

	// 计算关闭按钮的位置
	POINT roundPoint = { 2,2 };	// 弧度
	int padding = (m_closeWndSize - m_textMetric.tmAveCharWidth) / 2;

	dc.RoundRect(&m_closeWnd, roundPoint);
	dc.TextOut(m_closeWnd.left + padding, m_closeWnd.top, _T("X"));
}


void ClipboardWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OutPutString(_T("[%s] 窗口关闭"), __FUNCTIONW__);

	HWND parentWnd = GetParent()->m_hWnd;
	::SendMessage(parentWnd, WM_DESTORYCLIPBOARDWND, (WPARAM)m_id, 0);
	CStatic::OnClose();
}


void ClipboardWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (YunXing::YxWindowUtility::PointInRect(&point, &m_closeWnd))
	{
		m_lBtnDownClose = TRUE;
	}
	else
	{
		m_lBtnDownClose = FALSE;
	}
	CStatic::OnLButtonDown(nFlags, point);
}


void ClipboardWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (YunXing::YxWindowUtility::PointInRect(&point, &m_closeWnd)
		&& m_lBtnDownClose == TRUE)
	{
		SendMessage(WM_CLOSE);
		OutPutString(_T("[%s] 鼠标左键在关闭按钮上释放, 之前状态 = %d"), __FUNCTIONW__, m_lBtnDownClose);
	}

	m_lBtnDownClose = FALSE;
	CStatic::OnLButtonUp(nFlags, point);
}


int ClipboardWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (lpCreateStruct->x < 0 || lpCreateStruct->cx < 20
		|| lpCreateStruct->y < 0 || lpCreateStruct->cy < 20)
	{
		return -1;
	}

	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 算出关闭按钮的大小及位置
	GetTextMetrics(::GetWindowDC(m_hWnd), &m_textMetric);
	m_closeWndSize = m_textMetric.tmAveCharWidth + 10;

	return 0;
}


BOOL ClipboardWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}


void ClipboardWnd::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_RESTORED)
	{
		m_closeWnd.right = cy - 2 - m_frameWidth;
		m_closeWnd.top = 2 + m_frameWidth;
		m_closeWnd.left = m_closeWnd.right - m_closeWndSize;
		m_closeWnd.bottom = m_closeWnd.top + m_closeWndSize;
	}

	ZeroMemory(&m_wndRect, sizeof(RECT));
	m_wndRect.right = cx;
	m_wndRect.bottom = cy;

	int frameWidth = m_frameWidth - 2;

	m_drawRect.left = m_wndRect.left + frameWidth;
	m_drawRect.right = m_wndRect.right - frameWidth;
	m_drawRect.top = m_wndRect.top + frameWidth;
	m_drawRect.bottom = m_wndRect.bottom - frameWidth;
}


void ClipboardWnd::OnMove(int x, int y)
{
	CStatic::OnMove(x, y);
}


void ClipboardWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CStatic::OnShowWindow(bShow, nStatus);

	m_bShow = bShow;
}


void ClipboardWnd::OnSetFocus(CWnd* pOldWnd)
{
	OutPutString(_T("[%s], wnd = %x"), __FUNCTIONW__, m_hWnd);
	CStatic::OnSetFocus(pOldWnd);
}


void ClipboardWnd::OnKillFocus(CWnd* pNewWnd)
{
	OutPutString(_T("[%s], wnd = %x"), __FUNCTIONW__, m_hWnd);
	CStatic::OnKillFocus(pNewWnd);
}


afx_msg LRESULT ClipboardWnd::OnSwitchclipboard(WPARAM wParam, LPARAM lParam)
{
	m_currentSelected = (BOOL)wParam;
	RedrawWindow();
	return 0;
}


void ClipboardWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (YunXing::YxWindowUtility::PointInRect(&point, &m_wndRect))
	{
		if (m_mouseMove == FALSE)
		{
			m_mouseMove = TRUE;
			RedrawWindow();
		}
	}
	else
	{
		if (m_mouseMove == TRUE)
		{
			m_mouseMove = FALSE;
			RedrawWindow();
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}


BOOL ClipboardWnd::DrawClipboardWnd(CDC* pDc, RECT &rect)
{
	int dwCount = m_clipboardArray.GetSize();
	const ClipboardData* pData = NULL;

	OutPutString(_T("[%s] id = %d"), __FUNCTIONW__, m_id);
	for (int i = 0; i < dwCount; i++)
	{
		pData = &m_clipboardArray.GetAt(i);

		if (DrawClipboardData(*pData, pDc, rect))
		{
			return TRUE;
		}
	}

	WCHAR* notDraw = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR));

	wsprintfW(notDraw,L"无法识别_%d", m_id);

	ClipboardData data;
	data.m_dwFormat = CF_UNICODETEXT;
	data.m_lpData = notDraw;
	data.m_dwDataSize = wcslen(notDraw) * sizeof(WCHAR);

	DrawClipboardData(data, pDc, rect);

	return FALSE;
}


BOOL ClipboardWnd::DrawClipboardData(const ClipboardData& clipboardData, CDC* pDc, _Inout_  RECT &rect)
{
	BOOL bSuccess = FALSE;
	wchar_t* wzText = nullptr;

	do
	{
		OutPutString(_T("[%s] format = %d, size = %d"), __FUNCTIONW__, clipboardData.m_dwFormat, clipboardData.m_dwDataSize);
		switch (clipboardData.m_dwFormat)
		{
			// 文本格式
		case CF_TEXT:
		case CF_OEMTEXT:
		case CF_UNICODETEXT:
		{
			wzText = new wchar_t[clipboardData.m_dwDataSize + 1];
			if (wzText)
			{
				ZeroMemory(wzText, (clipboardData.m_dwDataSize + 1) * sizeof(WCHAR));
				if (clipboardData.m_dwFormat == CF_TEXT || clipboardData.m_dwFormat == CF_OEMTEXT)
				{
					MultiByteToWideChar(CP_ACP, 0, (char*)clipboardData.m_lpData, clipboardData.m_dwDataSize, wzText, clipboardData.m_dwDataSize + 1);
				}
				else
				{
					wmemcpy_s(wzText, clipboardData.m_dwDataSize + 1, (wchar_t*)clipboardData.m_lpData, clipboardData.m_dwDataSize / sizeof(WCHAR));
				}

				CPen pen;
				pen.CreatePen(PS_SOLID, 1, m_cfg->m_textColor);
				HGDIOBJ hOldObj = NULL;
				hOldObj = pDc->SelectObject(pen.m_hObject);

				DrawClipboardText(wzText, pDc, &rect);
				pDc->SelectObject(hOldObj);
				delete[] wzText;
			}
			bSuccess = TRUE;
		}
		break;

		// 位图句柄(HBITMAP)
		case CF_BITMAP:
		{
			HBITMAP hTmpBitmap = NULL;
			hTmpBitmap = (HBITMAP)clipboardData.m_lpData;
			DrawBitmap(pDc, hTmpBitmap, &rect);
			bSuccess = TRUE;
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

			BITMAPINFOHEADER* pBih = &pBitmapInfo->bmiHeader;
			DWORD offset = clipboardData.m_dwDataSize - pBih->biSizeImage;
			HBITMAP hBitmap = CreateDIBitmap(pDc->m_hDC, &pBitmapInfo->bmiHeader, CBM_INIT,
				(BYTE*)pBitmapInfo + sizeof(BITMAPINFOHEADER) + pBih->biClrUsed * sizeof(RGBQUAD), pBitmapInfo, DIB_PAL_COLORS);
			if (hBitmap)
			{
				DrawBitmap(pDc, hBitmap, &rect);
				DeleteObject(hBitmap);
			}
			bSuccess = TRUE;
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

			DrawClipboardText(sShowString.GetBuffer(), pDc, &rect);
			bSuccess = TRUE;
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


int ClipboardWnd::FilterInValidCharW(const wchar_t* wzSrc, int cSrc, wchar_t* wzDest, int cDest, CString invalidChar)
{
	int ret = 0;

	do
	{
		if (wzSrc == nullptr || wzDest == nullptr)
		{
			break;
		}

		int i = 0, j = 0;
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

bool ClipboardWnd::DrawClipboardText(const wchar_t* wzStr, CDC* pDc, RECT* rt)
{
	bool bRet = false;
	int iStrWidth = 0, iCharMaxPerLine = 0;	/*字符串总的宽度， 每行显示的最大宽度*/
	int iRow = 0, iMaxRow = 0;	/*行数，界面能显示的最大行数*/
	TEXTMETRIC tm;

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

		int iStrLen = 0;
		iStrLen = wcslen(wzStr);
		WCHAR* tmp = new WCHAR[iStrLen + 1];
		if (tmp == NULL)
		{
			break;
		}
		ZeroMemory(tmp, (iStrLen + 1) * sizeof(WCHAR));

		int cValid = 0;
		cValid = FilterInValidCharW(wzStr, iStrLen, tmp, iStrLen + 1, m_sInvalidChar);

		GetTextMetrics(pDc->m_hDC, &tm);
		iStrWidth = tm.tmAveCharWidth * cValid;
		iRow = iStrWidth / (rt->right - rt->left);
		if (iStrWidth % (rt->right - rt->left) > 0)
		{
			iRow++;
		}

		iCharMaxPerLine = ((rt->right - rt->left) / tm.tmAveCharWidth) - 1;
		if (iCharMaxPerLine <= 0)
		{
			break;
		}

		iMaxRow = (rt->bottom - rt->top) / (tm.tmExternalLeading + tm.tmHeight);
		iRow = min(iMaxRow, iRow);

		wchar_t* wzLine = NULL;
		wzLine = new wchar_t[iCharMaxPerLine + 8];

		for (int i = 0; i < iRow; i++)
		{
			ZeroMemory(wzLine, (iCharMaxPerLine + 8) * sizeof(WCHAR));
			wmemcpy_s(wzLine, iCharMaxPerLine + 8, tmp + i * iCharMaxPerLine, iCharMaxPerLine);
			pDc->TextOutW(rt->left + tm.tmAveCharWidth / 2, rt->top + i * (tm.tmExternalLeading + tm.tmHeight), wzLine);
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

BOOL ClipboardWnd::DrawBitmap(CDC* pDC, const HBITMAP hBitmap, RECT *rt)
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

		int iStartX = rt->left, iStartY = rt->top;
		int iDestWidth = 0, iDestHeight = 0;
		double dScale = 0;
		iDestWidth = rt->right - rt->left;
		iDestHeight = rt->bottom - rt->top;

		// 如果宽度大于高度则改变传入矩形的高度，反之改变宽度
		if (bmp.bmWidth > bmp.bmHeight)
		{
			dScale = (double)iDestWidth / bmp.bmWidth;
			iDestHeight = (int)(dScale * bmp.bmHeight) - 1;
			iStartY = (rt->bottom - rt->top - iDestHeight) / 2;
		}
		else
		{
			dScale = (double)iDestHeight / bmp.bmHeight;
			iDestWidth = (int)(dScale * bmp.bmWidth) - 1;
			iStartX = (rt->right - rt->left - iDestWidth) / 2;
		}
		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(iStartX, iStartY, iDestWidth, iDestHeight, &compatibleDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

		compatibleDC.SelectObject(hOldBitmap);

	} while (FALSE);

	return bSuccess;
}