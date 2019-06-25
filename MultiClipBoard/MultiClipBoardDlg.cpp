
// MultiClipBoardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <Imm.h>
#include "MultiClipBoard.h"
#include "MultiClipBoardDlg.h"
#include "afxdialogex.h"
#include "DataWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMultiClipBoardDlg �Ի���
#define ID_HOTKEY 123
#define ID_TIMER_INTERVAL 132			// �����л�ѡ����
#define ID_TIMER_CHECKFOCUS 133			// ��齹���Ƿ��뿪����

const int RectPadding = 20;				// ���ο�֮��Ŀհ׼��

CMultiClipBoardDlg::CMultiClipBoardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SWITCH_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_isPressedControl = false;
	m_isPressedShift = false;
	m_hNextClipboardViewer = NULL;
	m_rectHeight = 100;
	m_rectWidth = 100;
	m_rectPadding = 20;
	m_isFirstShow = true;
	m_bSwitchNext = true;
	ZeroMemory(&m_notifyData, sizeof(m_notifyData));

	m_sInvalidChar = L"\r\n\t";
}

void CMultiClipBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMultiClipBoardDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HOTKEY()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRAYNOTIFY, &CMultiClipBoardDlg::OnTraynotify)
	ON_MESSAGE(WM_SWITCHCLIPBOARD, &CMultiClipBoardDlg::OnSwitchClipboard)
	ON_WM_CLIPBOARDUPDATE()
	ON_WM_DESTROYCLIPBOARD()
	ON_WM_DRAWCLIPBOARD()
	ON_WM_HSCROLLCLIPBOARD()
	ON_WM_PAINTCLIPBOARD()
	ON_WM_SIZECLIPBOARD()
	ON_WM_VSCROLLCLIPBOARD()
	ON_WM_NCCREATE()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_COMMAND(IDM_TRAY_QUIT, &CMultiClipBoardDlg::OnMenuQuit)
END_MESSAGE_MAP()


// CMultiClipBoardDlg ��Ϣ�������

BOOL CMultiClipBoardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);


	// ���ϵͳ����	
	m_notifyData.cbSize = sizeof(NOTIFYICONDATAW);
	m_notifyData.uCallbackMessage = WM_TRAYNOTIFY;
	m_notifyData.hIcon = m_hIcon;
	m_notifyData.uID = IDR_MAINFRAME;
	m_notifyData.hWnd = m_hWnd;
	m_notifyData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	wcscpy_s(m_notifyData.szTip, 128, L"���Ǽ��а���������");
	Shell_NotifyIconW(NIM_ADD, &m_notifyData);

	// ע���ݼ�
	RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');

	// ��Ӽ��а���
	m_hNextClipboardViewer = SetClipboardViewer();
	if (m_hNextClipboardViewer == NULL && GetLastError() != NULL)
	{
		OutPutString(L"OnInitDialog, error = %d", GetLastError());
		PostQuitMessage(0);
	}

	// ��ʱ����Ǵ����Ƿ��н���
	SetTimer(ID_TIMER_CHECKFOCUS, 100, NULL);
	m_bkColor = RGB(150, 150, 150);
	m_nAlpha = 50;

	m_rectBKColor = RGB(20, 240, 240);
	m_frameColor = RGB(240, 240, 240);
	m_frameWidth = 4;

	m_textColor = RGB(0, 240, 240);
	m_textWidth = 1;
	m_textSize = 20;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMultiClipBoardDlg::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// ���Ʊ���
		RECT bkGroundRect;
		CBrush bkBrush, *oldBrush = NULL;
		GetClientRect(&bkGroundRect);
		bkBrush.CreateSolidBrush(m_bkColor);
		dc.FillRect(&bkGroundRect, &bkBrush);

		CRect averageRectSize;							// ���ο�ƽ���ߴ�
		ClipboardArrayWnd* pClipboardArrayWnd = nullptr;		// ÿ����а�����
		int dwListCount = m_clipboardList.GetSize();	// ���а�����ĸ���
		POSITION arraryPosition = m_clipboardList.GetHeadPosition();

		// ���ı�����ɫ
		CBrush brush;
		brush.CreateSolidBrush(m_rectBKColor);
		oldBrush = dc.SelectObject(&brush);
		dc.SetBkMode(TRANSPARENT);

		int iEachLineCount = CalculateDrawRectSize(dwListCount, averageRectSize);
		for (int i = 0; i < dwListCount && arraryPosition; i++)
		{
			HDC hCompatibleDc = CreateCompatibleDC(dc.m_hDC);
			HBITMAP hBitmap = CreateCompatibleBitmap(hCompatibleDc, 0, 0);

			// ���� tmpRect��λ��
			int x = 0, y = 0;
			CRect destRect = averageRectSize;

			x = i % iEachLineCount * (RectPadding + averageRectSize.Width()) + RectPadding;
			y = i / iEachLineCount * (RectPadding + averageRectSize.Height()) + RectPadding;
			destRect.left += x;
			destRect.right += x;
			destRect.top += y; (RectPadding + averageRectSize.Height()) + RectPadding;
			destRect.bottom += y;

			pClipboardArrayWnd = &m_clipboardList.GetNext(arraryPosition);

			// ���Ʊ߿�
			if (pClipboardArrayWnd->m_id == m_curClipboardArrayData.m_id)
			{
				CPen pen, *oldPen = NULL;
				pen.CreatePen(PS_DASH, m_frameWidth, m_frameColor);
				oldPen = dc.SelectObject(&pen);
				dc.Rectangle(&destRect);
				dc.SelectObject(oldPen);
			}

			destRect.DeflateRect(m_frameWidth, m_frameWidth);
			dc.FillRect(destRect, &brush);

			DrawClipboardArray(*pClipboardArrayWnd, &dc, destRect);
		}

		//CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMultiClipBoardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ��Ӧ��ݼ���Ϣ
void CMultiClipBoardDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId == ID_HOTKEY)
	{
		m_isPressedControl = true;
		//Invalidate();
		CRect clientRect;

		if (CalculateWindow(clientRect))
		{
			MoveWindow(&clientRect);
			// SetWindowPos(NULL, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom,
			//	SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			ShowWindow(SW_NORMAL);
			UnregisterHotKey(m_hWnd, ID_HOTKEY);
		}
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CMultiClipBoardDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
}

// 
// 1.ȡ��ϵͳ����
// 2.ȡ����ݼ�ע��
// 3.�ڼ��а�����ɾ�����а�
//

void CMultiClipBoardDlg::OnClose()
{
	ChangeClipboardChain(m_hNextClipboardViewer);
	Shell_NotifyIcon(NIM_DELETE, &m_notifyData);
	UnregisterHotKey(m_hWnd, ID_HOTKEY);
	//CDialogEx::OnClose();
	PostQuitMessage(0);
}


int CMultiClipBoardDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


afx_msg LRESULT CMultiClipBoardDlg::OnTraynotify(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN)
	{

	}
	else if (lParam == WM_RBUTTONDOWN)
	{
		POINT point;
		CMenu menu;

		GetCursorPos(&point);
		menu.LoadMenuW(MAKEINTRESOURCE(IDR_TRAY_MENU));
		CMenu* firstMenu = menu.GetSubMenu(0);

		SetForegroundWindow();
		firstMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		menu.Detach();
		menu.DestroyMenu();
	}
	return 0;
}

// ���¿�ݼ�֮�����ctrlһֱ����������ʾ����Ի���
// �ɵ�֮������ʾ
BOOL CMultiClipBoardDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP)				// ��������ע���ȼ�
	{
		if (pMsg->wParam == VK_CONTROL)
		{
			m_isPressedControl = false;
			m_isPressedShift = false;
			ShowWindow(SW_HIDE);
			RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');
		}
		else if (pMsg->wParam == VK_SHIFT)
		{
			m_isPressedShift = false;
		}
	}
	else if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == L'Q')
		{
			if (m_isPressedControl == true)
			{
				SendMessage(WM_SWITCHCLIPBOARD, (WPARAM)m_isPressedShift, 0);
				Invalidate();
			}
		}
		else if (pMsg->wParam == VK_SHIFT)
		{
			m_isPressedShift = true;
		}
		if (pMsg->wParam != 0xE5)
		{
			m_isPressedControl = true;
		}
	}
	else if (pMsg->message == WM_SETFOCUS)
	{
		OutPutString(L"PreTranslateMessage SetFocus");
	}
	else if (pMsg->message == WM_KILLFOCUS)		// ʧȥ���㣬�޷���ü�����Ϣ��ģ�ⷢ��CTRL������Ϣ
	{
		SendMessage(WM_KEYUP, VK_CONTROL, 0xC0000001);
		OutPutString(L"PreTranslateMessage KillFocus");
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CMultiClipBoardDlg::OnSwitchClipboard(WPARAM wParam, LPARAM lParam)
{
	POSITION position = m_clipboardList.GetHeadPosition();
	POSITION headPos = position;
	ClipboardArrayWnd* clipboardData = nullptr;
	bool bFind = false;

	if (!m_bSwitchNext)
	{
		return S_OK;
	}
	m_bSwitchNext = false;
	SetTimer(ID_TIMER_INTERVAL, 100, NULL);

	for (; position != nullptr; )
	{
		clipboardData = &m_clipboardList.GetNext(position);

		if (m_curClipboardArrayData.m_id == clipboardData->m_id)
		{
			// ���Һ�һ��
			if (!wParam)
			{
				if (position == nullptr)
				{
					position = m_clipboardList.GetHeadPosition();
				}
				m_curClipboardArrayData = m_clipboardList.GetNext(position);
				bFind = true;
				break;
			}
			// ����ǰһ��
			else
			{
				if (position == nullptr)
				{
					position = m_clipboardList.GetTailPosition();
				}
				else
				{
					m_clipboardList.GetPrev(position);
				}
				m_clipboardList.GetPrev(position);
				if (position == nullptr)
				{
					position = m_clipboardList.GetTailPosition();
				}
				m_curClipboardArrayData = m_clipboardList.GetPrev(position);

				bFind = true;
				break;
			}
		}
	}

	if (bFind)
	{
		WCHAR wszFormatName[MAX_PATH] = { 0 };
		m_openClipboardLock = true;
		BOOL bOperate = OpenClipboard();
		if (!bOperate)
		{
			OutPutString(L"OnDrawClipboard :OpenClipboard error = %d", GetLastError());
		}
		EmptyClipboard();
		for (int i = 0; i < m_curClipboardArrayData.m_clipboardArray.GetSize(); i++)
		{
			ClipboardData tmpData = m_curClipboardArrayData.m_clipboardArray.GetAt(i);
			HGLOBAL hData = GlobalAlloc(GHND, tmpData.m_dwDataSize);
			LPVOID pData = GlobalLock(hData);
			memcpy_s(pData, tmpData.m_dwDataSize, tmpData.m_lpData, tmpData.m_dwDataSize);
			GlobalUnlock(hData);
			SetClipboardData(tmpData.m_dwFormat, hData);

			GetClipboardFormatName(tmpData.m_dwFormat, wszFormatName, MAX_PATH);
			OutPutString(L"format = %x, name = %s", tmpData.m_dwFormat, wszFormatName);
		}
		CloseClipboard();
		Sleep(10);
		m_openClipboardLock = false;
	}

	return 0;
}

void CMultiClipBoardDlg::OnClipboardUpdate()
{
	// �˹���Ҫ�� Windows Vista ����߰汾��
	// _WIN32_WINNT ���ű��� >= 0x0600��
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClipboardUpdate();
}


void CMultiClipBoardDlg::OnDestroyClipboard()
{
	CDialogEx::OnDestroyClipboard();

	// TODO: �ڴ˴������Ϣ����������
}

// ����SetClipboardData �¼����Ӵ��¼��л�ȡ����
// ���洢���ڴ���
void CMultiClipBoardDlg::OnDrawClipboard()
{
	CDialogEx::OnDrawClipboard();
	if (m_openClipboardLock)
	{
		return;
	}

	// ���յ���Ϣ�������ݱ��浽��ǰ��������
	ClipboardArrayWnd clipboardArrayWnd;

	BOOL bOperate = OpenClipboard();
	if (!bOperate)
	{
		OutPutString(L"OnDrawClipboard :OpenClipboard error = %d", GetLastError());
		return;
	}

	DWORD dwEnumFormat = 0;
	WCHAR wszFormatName[MAX_PATH] = { 0 };
	DWORD dwCount = 0;
	while (TRUE)
	{
		dwEnumFormat = EnumClipboardFormats(dwEnumFormat);
		if (dwEnumFormat == 0)
		{
			break;
		}
		GetClipboardFormatName(dwEnumFormat, wszFormatName, MAX_PATH);
		OutPutString(L"format = %x, name = %s", dwEnumFormat, wszFormatName);

		HGLOBAL hData = GetClipboardData(dwEnumFormat);
		if (hData)
		{
			DWORD dwSize = GlobalSize(hData);
			if (dwSize > 0)
			{
				ClipboardData tmp, *pClipboardData = NULL;
				int index = 0;
				// ����ӵ�����������ظ������ڴ�
				index = clipboardArrayWnd.m_clipboardArray.Add(tmp);
				if (pClipboardData = &clipboardArrayWnd.m_clipboardArray.GetAt(index))
				{
					pClipboardData->m_dwFormat = dwEnumFormat;
					pClipboardData->m_dwDataSize = dwSize;
					LPVOID pData = GlobalLock(hData);
					if (pData)
					{
						pClipboardData->m_lpData = malloc(dwSize);
						memcpy_s(pClipboardData->m_lpData, dwSize, pData, dwSize);

						GlobalUnlock(hData);
					}
				}
				dwCount++;
			}
		}
	}
	CloseClipboard();

	if (clipboardArrayWnd.m_clipboardArray.GetSize() == 0)
	{
		return;
	}

	// �����ݱ��浽���������У����ҷ���id
	POSITION position = m_clipboardList.GetHeadPosition();
	DWORD dwListSize = m_clipboardList.GetSize();
	ClipboardArrayWnd* pClipboardArrayWnd = NULL;

	// ����һ���ڴ����������ĸ�idû�з���
	BYTE *bArrFlag = new BYTE[dwListSize + 1];
	if (bArrFlag)
	{
		memset(bArrFlag, 1, dwListSize + 1);

		for (; position != nullptr; )
		{
			pClipboardArrayWnd = &m_clipboardList.GetNext(position);
			bArrFlag[pClipboardArrayWnd->m_id] = 0;
		}

		clipboardArrayWnd.m_id = dwListSize;
		for (DWORD i = 0; i < dwListSize; i++)
		{
			if (bArrFlag[i] == 1)
			{
				// �ҵ�һ��id
				clipboardArrayWnd.m_id = i;
				break;
			}
		}
		
		delete[] bArrFlag;
	}
	
	CRect averageRectSize;							// ���ο�ƽ���ߴ�
	ClipboardArrayWnd* pClipboardArrayWnd = nullptr;		// ÿ����а�����
	int listCount = m_clipboardList.GetSize();	// ���а�����ĸ���
	POSITION arraryPosition = m_clipboardList.GetHeadPosition();

	int eachLineCount = CalculateDrawRectSize(listCount, averageRectSize);
	for (int i = 0; i < listCount && arraryPosition; i++)
	{
		// ���� tmpRect��λ��
		int x = 0, y = 0;
		CRect destRect = averageRectSize;

		x = i % eachLineCount * (RectPadding + averageRectSize.Width()) + RectPadding;
		y = i / eachLineCount * (RectPadding + averageRectSize.Height()) + RectPadding;
		destRect.left += x;
		destRect.right += x;
		destRect.top += y; (RectPadding + averageRectSize.Height()) + RectPadding;
		destRect.bottom += y;

		if ((clipboardArrayWnd.m_id == pClipboardArrayWnd->m_id)
			&& clipboardArrayWnd.m_wnd == NULL)
		{
			// �����µĴ���
			CDataWnd* dataWnd = new CDataWnd();
			if (dataWnd->Create(_T("static"), NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, &destRect, this, 0, NULL))
			{
				clipboardArrayWnd.m_wnd = dataWnd;
				m_clipboardList.AddTail(clipboardArrayWnd);
			}
		}
		else
		{
			// �����´��ڵ�λ��
			pClipboardArrayWnd->m_wnd->MoveWindow(&destRect, FALSE);
		}

		pClipboardArrayWnd = &m_clipboardList.GetNext(arraryPosition);
	}

	//CDialogEx::OnPaint();
}


BOOL CMultiClipBoardDlg::DrawClipboardData(const ClipboardData& clipboardData, CDC* pDc, _Inout_  CRect &rect)
{
	BOOL bSuccess = FALSE;
	wchar_t* wzText = nullptr;

	do
	{
		switch (clipboardData.m_dwFormat)
		{
			// �ı���ʽ
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

		// λͼ���(HBITMAP)
		case CF_BITMAP:
		{
			HBITMAP hTmpBitmap = NULL;
			hTmpBitmap = (HBITMAP)clipboardData.m_lpData;
			DrawBitmap(pDc, hTmpBitmap, &rect);
		}
		break;

		// BITMAPINFO �ṹ����Ϣ (�ƺ����ܻ���)
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

		// BITMAPV5HEADER �ṹ����Ϣ (�ƺ����ܻ���)
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

		// ��ǿͼԪ�ļ����(HENHMETAFILE)
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
			OutPutString(L"ͼԪ�ļ�������%s", wzDescription);
		}
		break;

		// ��ק�ļ����(HDROP)
		case CF_HDROP:
		{
			HDROP hDrop = NULL;
			hDrop = (HDROP)clipboardData.m_lpData;
			if (hDrop == NULL)
			{
				break;
			}

			CString sShowString = L"�����ļ���";
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

		// ͼԪͼƬ�ľ��
		case CF_METAFILEPICT:
		{

		}
		break;

		// �Ȳ�����Ƶ�����ӵ���Ƶ����
		case CF_RIFF:
		{

		}
		break;

		// ΢��������Ӹ�ʽ
		case CF_SYLK:
		{

		}
		break;

		// ��ǩͼƬ�ļ�
		case CF_TIFF:
		{

		}
		break;

		// ��׼����Ƶ����
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


BOOL CMultiClipBoardDlg::DrawClipboardArray(const ClipboardArrayWnd& clipboardArrayWnd, CDC* pDc, CRect &rect)
{
	int dwCount = clipboardArrayWnd.m_clipboardArray.GetSize();
	const ClipboardData* pData = NULL;

	for (int i = 0; i < dwCount; i++)
	{
		pData = &clipboardArrayWnd.m_clipboardArray.GetAt(i);

		if (DrawClipboardData(*pData, pDc, rect))
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
	ѡȡ��Ļ�Ͽɹ���ʾ�ķ�Χ��
	�����㷨��
			����С���ο�ĳߴ磬����������������Ų��£��Ƚ���С��С��������Ų��£�
		���������������ѭ����
*/
int CMultiClipBoardDlg::CalculateDrawRectSize(const DWORD dwCount, _Out_ CRect &rect)
{
	const int RectMinWidth = 100;			// ��ʾ���ο����С���
	const int RectMinHeight = 100;			// ��ʾ���ο����С�߶�
	const int RectMaxPercent = 80;			// ���ο��������󲻳�����Ļ�ķ�Χ 0.8
	const int RectDecrease = 20;			// ���ο��Сÿ�εݼ�������

	int RectMaxWidth;						// ��ʾ���ο������ȣ����Ϊ��ʾ��Χ��һ��
	int RectMaxHeight;						// ��ʾ���ο����С�߶�

	int iScreenX = 0, iScreenY = 0;			// �ɹ���ʾ��Χ
	int iWidth = 0, iHeight = 0;

	// �ɹ���ʾ�ķ�Χ
	iScreenX = GetSystemMetrics(SM_CXSCREEN) * 4 / 5;
	iScreenY = GetSystemMetrics(SM_CYSCREEN) * 4 / 5;

	// ���ο���ʾ���������
	RectMaxWidth = iScreenX / 2;
	RectMaxHeight = iScreenY / 2;

	RectMaxWidth = RectMaxHeight = min(RectMaxWidth, RectMaxHeight);

	rect.SetRectEmpty();

	int iMaxCountPerLine = dwCount;			// ÿ������ŵĸ���
	int iLine = 1;							// ��ʾ������
	do
	{
		if (RectMaxWidth < RectMinWidth || RectMaxHeight < RectMinHeight)
		{
			break;
		}

		// ��С�ߴ绹������������1��ʾ��С�ߴ�
		BOOL bSwitch = 1;
		do
		{
			// ��ʱֻ�жϺ����Ƿ񹻣����ο�Ĵ�С�������ݼ�
			int iRemainPix = (iScreenX - RectPadding) - (iMaxCountPerLine * (RectPadding + RectMaxWidth));
			if (iRemainPix < 0)
			{
				if (bSwitch)
				{	// ��С�ߴ�
					RectMaxWidth -= RectDecrease;
				}
				else
				{	// ÿ�и�����С
					iMaxCountPerLine--;
				}
				bSwitch ^= 1;
				if (RectMaxWidth < RectMinWidth || iMaxCountPerLine <= 0)
				{	// �Ų��£��˳�
					iLine = 0;
					break;
				}
			}
			else
			{
				iLine = (dwCount + iMaxCountPerLine - 1) / iMaxCountPerLine;
				rect.SetRect(0, 0, RectMaxWidth, RectMaxWidth);
				break;
			}

		} while (true);

	} while (FALSE);

	return iMaxCountPerLine;
}

// ������ʾ�����������贰�ڵĴ�С
BOOL CMultiClipBoardDlg::CalculateWindow(CRect &rect)
{
	BOOL bSuccess = FALSE;

	DWORD dwCount = m_clipboardList.GetSize();
	CRect averageRect;
	RECT windowRect = { 0 };
	int x = 0, y = 0;

	::GetWindowRect(::GetDesktopWindow(), &windowRect);

	// ��������Ĵ�С���ͼ��а����ݵĸ���
	int iLineCount = CalculateDrawRectSize(dwCount, averageRect);
	if (iLineCount > 0)
	{
		x = (averageRect.Width() + RectPadding) * iLineCount + RectPadding;
		y = (averageRect.Height() + RectPadding) * ((dwCount + iLineCount - 1) / iLineCount) + RectPadding;

		rect.left = (windowRect.right - windowRect.left - x) / 2;
		rect.top = (windowRect.bottom - windowRect.top - y) / 2;
		rect.right = rect.left + x;
		rect.bottom = rect.top + y;

		bSuccess = TRUE;
	}

	return bSuccess;
}


BOOL CMultiClipBoardDlg::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CDialogEx::OnNcCreate(lpCreateStruct))
		return FALSE;

	// ���뷨�Ա��߳���Ч
	ImmDisableIME(0);

	return TRUE;
}


BOOL CMultiClipBoardDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CMultiClipBoardDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (m_isFirstShow)
	{
		if (lpwndpos->flags & SWP_SHOWWINDOW)
		{
			lpwndpos->flags &= ~SWP_SHOWWINDOW;
			m_isFirstShow = false;
		}
	}
	CDialogEx::OnWindowPosChanging(lpwndpos);
}


void CMultiClipBoardDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ID_TIMER_INTERVAL:
		m_bSwitchNext = true;
		break;

	case ID_TIMER_CHECKFOCUS:
		if (GetFocus() != this)
		{
			m_isPressedControl = false;
			m_isPressedShift = false;
			ShowWindow(SW_HIDE);
			RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');
		}
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMultiClipBoardDlg::OnMenuQuit()
{
	OnClose();
}

