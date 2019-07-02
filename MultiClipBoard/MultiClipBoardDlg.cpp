
// MultiClipBoardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <Imm.h>
#include "MultiClipBoard.h"
#include "MultiClipBoardDlg.h"
#include "afxdialogex.h"

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
	m_curClipboardWnd = NULL;

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
	ON_WM_MOUSEMOVE()
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
		CBrush *oldBrush = NULL, bkBrush;
		GetClientRect(&bkGroundRect);
		bkBrush.CreateSolidBrush(m_bkColor);
		dc.SetBkMode(TRANSPARENT);
		dc.FillRect(&bkGroundRect, &bkBrush);

		CDialogEx::OnPaint();
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

// ����id, �����ݱ��浽����������
BOOL CMultiClipBoardDlg::AllocNewWnd(ClipboardWnd & clipboardWnd)
{
	BOOL ret = FALSE;
	DWORD dwListSize = m_clipboardWndList.GetSize() + 1;
	ClipboardWnd* pClipboardWnd = NULL;

	// ����δ�����id
	DWORD i = 0;
	for (; i < dwListSize; i++)
	{
		BOOL find = FALSE;
		POSITION position = m_clipboardWndList.GetHeadPosition();
		for (; position != nullptr;)
		{
			pClipboardWnd = m_clipboardWndList.GetNext(position);
			if (pClipboardWnd->m_id == i)
			{
				find = TRUE;
				ret = FALSE;
				break;
			}
		}
		if (find == FALSE)
		{
			// ����id�����Ҵ����µĴ��ڣ���ӵ�list
			clipboardWnd.m_id = i;
			pClipboardWnd = new ClipboardWnd();
			CRect rc(0, 0, 100, 100);
			if (pClipboardWnd->Create(_T("static"), NULL, WS_CHILD | WS_VISIBLE | WS_THICKFRAME, rc, this, 0, NULL))
			{
				m_clipboardWndList.AddTail(pClipboardWnd);
				*pClipboardWnd = clipboardWnd;
				if (m_curClipboardWnd == NULL)
				{
					m_curClipboardWnd = pClipboardWnd;
				}
				ret = TRUE;
			}
			else
			{
				delete pClipboardWnd;
				ret = FALSE;
			}
			break;
		}
	}
	return ret;
}

BOOL CMultiClipBoardDlg::FreeWnd(DWORD wndId)
{
	BOOL ret = FALSE;

	return ret;
}


BOOL CMultiClipBoardDlg::AdjustWndLayout()
{
	BOOL ret = FALSE;

	// �������ο��λ��
	CRect averageRectSize;							// ���ο�ƽ���ߴ�
	ClipboardWnd* pClipboardWnd = NULL;						// ÿ����а�����
	DWORD dwListSize = m_clipboardWndList.GetSize();
	POSITION arrayPosition = m_clipboardWndList.GetHeadPosition();

	// ��ȡÿ�еĸ���
	int eachLineCount = CalculateDrawRectSize(dwListSize, averageRectSize);

	for (DWORD i = 0; i < dwListSize && arrayPosition; i++)
	{
		// ���� tmpRect��λ��
		int x = 0, y = 0;
		CRect destRect = averageRectSize;
		pClipboardWnd = m_clipboardWndList.GetNext(arrayPosition);

		x = i % eachLineCount * (RectPadding + averageRectSize.Width()) + RectPadding;
		y = i / eachLineCount * (RectPadding + averageRectSize.Height()) + RectPadding;
		destRect.left += x;
		destRect.right += x;
		destRect.top += y;
		destRect.bottom += y;

			// �����´��ڵ�λ��
		pClipboardWnd->MoveWindow(&destRect, FALSE);
	}

	return ret;
}

/*
	��Ӧ�л����а���ͼ����Ϣ
*/
LRESULT CMultiClipBoardDlg::OnSwitchClipboard(WPARAM wParam, LPARAM lParam)
{
	POSITION position = m_clipboardWndList.GetHeadPosition();
	POSITION headPos = position;
	ClipboardWnd* clipboardData = nullptr;
	bool bFind = false;

	if (!m_bSwitchNext)
	{
		return S_OK;
	}
	m_bSwitchNext = false;
	SetTimer(ID_TIMER_INTERVAL, 100, NULL);

	for (; position != nullptr; )
	{
		clipboardData = m_clipboardWndList.GetNext(position);

		if (m_curClipboardWnd->m_id == clipboardData->m_id)
		{
			// ���Һ�һ��
			if (!wParam)
			{
				if (position == nullptr)
				{
					position = m_clipboardWndList.GetHeadPosition();
				}
				m_curClipboardWnd = m_clipboardWndList.GetNext(position);
				bFind = true;
				break;
			}
			// ����ǰһ��
			else
			{
				if (position == nullptr)
				{
					position = m_clipboardWndList.GetTailPosition();
				}
				else
				{
					m_clipboardWndList.GetPrev(position);
				}
				m_clipboardWndList.GetPrev(position);
				if (position == nullptr)
				{
					assert(TRUE);
					position = m_clipboardWndList.GetTailPosition();
				}
				m_curClipboardWnd = m_clipboardWndList.GetPrev(position);

				bFind = true;
				break;
			}
		}
	}

	if (bFind)
	{
		WCHAR clipboardFormatName[MAX_PATH] = { 0 };
		m_openClipboardLock = true;
		BOOL bOperate = OpenClipboard();
		if (!bOperate)
		{
			OutPutString(L"OnDrawClipboard :OpenClipboard error = %d", GetLastError());
		}
		EmptyClipboard();
		for (int i = 0; i < m_curClipboardWnd->m_clipboardArray.GetSize(); i++)
		{
			ClipboardData tmpData = m_curClipboardWnd->m_clipboardArray.GetAt(i);
			HGLOBAL hData = GlobalAlloc(GHND, tmpData.m_dwDataSize);
			LPVOID pData = GlobalLock(hData);
			memcpy_s(pData, tmpData.m_dwDataSize, tmpData.m_lpData, tmpData.m_dwDataSize);
			GlobalUnlock(hData);
			SetClipboardData(tmpData.m_dwFormat, hData);

			GetClipboardFormatName(tmpData.m_dwFormat, clipboardFormatName, MAX_PATH);
			OutPutString(L"format = %x, name = %s", tmpData.m_dwFormat, clipboardFormatName);
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
	{	// �Լ���ֵ
		return;
	}

	// ���յ���Ϣ�������ݱ��浽��ǰ��������
	BOOL bOperate = OpenClipboard();
	if (!bOperate)
	{
		OutPutString(L"OnDrawClipboard :OpenClipboard error = %d", GetLastError());
		return;
	}

	// ö�ټ��а��е����ݱ�������
	DWORD dwCount = 0;
	DWORD dwEnumFormat = 0;
	ClipboardWnd clipboardWnd;
	WCHAR formatName[MAX_PATH] = { 0 };

	while (TRUE)
	{
		dwEnumFormat = EnumClipboardFormats(dwEnumFormat);
		if (dwEnumFormat == 0)
		{
			break;
		}
		GetClipboardFormatName(dwEnumFormat, formatName, MAX_PATH);
		OutPutString(L"[%s] format = %x, name = %s", __FUNCTIONW__, dwEnumFormat, formatName);

		HGLOBAL hData = GetClipboardData(dwEnumFormat);
		if (hData)
		{
			DWORD dwSize = GlobalSize(hData);
			if (dwSize > 0)
			{
				int index = 0;
				ClipboardData tmp;
				tmp.m_dwFormat = dwEnumFormat;
				tmp.m_dwDataSize = dwSize;
				LPVOID pData = GlobalLock(hData);
				if (pData)
				{
					tmp.m_lpData = malloc(dwSize);
					memcpy_s(tmp.m_lpData, dwSize, pData, dwSize);
					GlobalUnlock(hData);
				}
				index = clipboardWnd.m_clipboardArray.Add(tmp);
			}
		}
	}
	CloseClipboard();

	// �浽��������ҵ�������
	if (clipboardWnd.m_clipboardArray.GetSize() == 0)
	{	// û������
		return;
	}
	else
	{
		if (AllocNewWnd(clipboardWnd))
		{
			AdjustWndLayout();
		}
	}

	//CDialogEx::OnPaint();
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

		if (iMaxCountPerLine <= 0)
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

	DWORD dwCount = m_clipboardWndList.GetSize();
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
	{
		DWORD focusProcessId = 0;
		GetWindowThreadProcessId(::GetFocus(), &focusProcessId);
		if (GetCurrentProcessId() != focusProcessId)
		{
			m_isPressedControl = false;
			m_isPressedShift = false;
			ShowWindow(SW_HIDE);
			RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');
		}
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


void CMultiClipBoardDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnMouseMove(nFlags, point);
}
