#include "stdafx.h"
#include "DrawFunction.h"

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
			iDestHeight = dScale * bmp.bmHeight;
			rt->bottom = rt->top + iDestHeight;
		}
		else
		{
			dScale = iDestHeight / bmp.bmHeight;
			iDestWidth = dScale * bmp.bmWidth;
			rt->right = rt->left + iDestWidth;
		}
		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(rt->left, rt->top, iDestWidth, iDestHeight, &compatibleDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

		compatibleDC.SelectObject(hOldBitmap);

	} while (FALSE);

	return bSuccess;
}