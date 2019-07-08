
// MultiClipBoardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <Imm.h>
#include "MultiClipBoard.h"
#include "MultiClipBoardDlg.h"
#include "afxdialogex.h"
#include "ConfigDlg.h"
#include "ActiveDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMultiClipBoardDlg 对话框
#define ID_HOTKEY 123
#define ID_TIMER_INTERVAL 132			// 用来切换选中项
#define ID_TIMER_CHECKFOCUS 133			// 检查焦点是否离开程序
#define ID_TIMER_DESTORYCLIPBOARD 134	// 销毁剪切板数据

const int RectPadding = 20;				// 矩形框之间的空白间距

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
	m_curMouseClipboardWnd = NULL;
	m_curlBtnClickWnd = NULL;

	ZeroMemory(&m_notifyData, sizeof(m_notifyData));

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
	ON_MESSAGE(WM_DESTORYCLIPBOARDWND, &CMultiClipBoardDlg::OnDestoryClipboardWnd)
	ON_MESSAGE(WM_CLIPBOARDSELECTED, &CMultiClipBoardDlg::OnClipboardSelected)
	ON_COMMAND(IDM_TRAY_QUIT, &CMultiClipBoardDlg::OnTrayQuit)
	ON_COMMAND(IDM_TRAY_CONFIG, &CMultiClipBoardDlg::OnTrayConfig)
	ON_COMMAND(IDM_TRAY_ABOUT, &CMultiClipBoardDlg::OnTrayAbout)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_SWITCHALPHACHANGED, &CMultiClipBoardDlg::OnSwitchAlphaChanged)
	ON_COMMAND(IDM_TRAY_PAUSE, &CMultiClipBoardDlg::OnTrayPause)
END_MESSAGE_MAP()


// CMultiClipBoardDlg 消息处理程序

BOOL CMultiClipBoardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

	m_cfg = CConfig::GetInstance();

	// 添加系统托盘	
	m_notifyData.cbSize = sizeof(NOTIFYICONDATAW);
	m_notifyData.uCallbackMessage = WM_TRAYNOTIFY;
	m_notifyData.hIcon = m_hIcon;
	m_notifyData.uID = IDR_MAINFRAME;
	m_notifyData.hWnd = m_hWnd;
	m_notifyData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	wcscpy_s(m_notifyData.szTip, 128, L"云星剪切板正在运行");
	Shell_NotifyIconW(NIM_ADD, &m_notifyData);

	// 窗口透明设置
	ULONG wndStype = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, wndStype | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0xFFFFFF, m_cfg->m_switchAlpha, LWA_ALPHA);

	// 注册快捷键
	RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');

	// 添加剪切板链
	m_hNextClipboardViewer = SetClipboardViewer();
	if (m_hNextClipboardViewer == NULL && GetLastError() != NULL)
	{
		OutPutString(L"OnInitDialog, error = %d", GetLastError());
		PostQuitMessage(0);
	}

	// 定时检测是窗口是否有焦点
	SetTimer(ID_TIMER_CHECKFOCUS, 200, NULL);

	m_menu.LoadMenuW(MAKEINTRESOURCE(IDR_TRAY_MENU));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMultiClipBoardDlg::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 绘制背景
		RECT bkGroundRect;
		CBrush bkBrush;
		GetClientRect(&bkGroundRect);
		bkBrush.CreateSolidBrush(m_cfg->m_switchColor);
		dc.SetBkMode(TRANSPARENT);
		dc.FillRect(&bkGroundRect, &bkBrush);

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMultiClipBoardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 响应快捷键消息
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
			ShowWindow(SW_NORMAL);

			// 收到快捷键消息后取消注册快捷键
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
// 1.取消系统托盘
// 2.取消快捷键注册
// 3.在剪切板链中删除剪切板
//

void CMultiClipBoardDlg::OnClose()
{
	m_menu.Detach();
	m_menu.DestroyMenu();
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

		GetCursorPos(&point);
		CMenu* firstMenu = m_menu.GetSubMenu(0);

		SetForegroundWindow();
		firstMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	return 0;
}

// 按下快捷键之后，如果ctrl一直被按下则显示这个对话框
// 松掉之后则不显示
BOOL CMultiClipBoardDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYUP:
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
		break;
	case WM_KEYDOWN:
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
		return TRUE;
	case WM_SETFOCUS:
		OutPutString(L"PreTranslateMessage SetFocus");
		break;
	case WM_KILLFOCUS:		// 失去焦点，无法获得键盘消息，模拟发送CTRL弹起消息
		SendMessage(WM_KEYUP, VK_CONTROL, 0xC0000001);
		OutPutString(L"PreTranslateMessage KillFocus");
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	{	// 如果左键的按下和弹起在子窗口就向下传递
		ClipboardWnd* wnd = ClipboardHitTest(pMsg->pt);
		// 改变窗口是否被选中 start
		if (pMsg->message == WM_LBUTTONUP)
		{
			if (wnd == m_curlBtnClickWnd && wnd != NULL)
			{
				if (m_curClipboardWnd != m_curlBtnClickWnd)
				{
					if (m_curClipboardWnd != NULL)
					{
						::SendMessage(m_curClipboardWnd->m_hWnd, WM_SWITCHCLIPBOARD, FALSE, NULL);
					}
					m_curClipboardWnd = m_curlBtnClickWnd;
					::SendMessage(m_curClipboardWnd->m_hWnd, WM_SWITCHCLIPBOARD, TRUE, NULL);
				}
			}
		}
		if (pMsg->message == WM_LBUTTONDOWN)
		{
			m_curlBtnClickWnd = wnd;
		}
		// 改变窗口是否被选中 end

		if (wnd)
		{
			RECT rect = { 0 };
			::GetWindowRect(wnd->m_hWnd, &rect);
			int x = pMsg->pt.x, y = pMsg->pt.y;
			x -= rect.left;
			y -= rect.top;
			::SendMessage(wnd->m_hWnd, pMsg->message, pMsg->wParam, (LPARAM)(x | y << 16));
			OutPutString(_T("[%s] message = %d, pt.x = %d, pt.y = %d"), __FUNCTIONW__, pMsg->message, pMsg->pt.x, pMsg->pt.y);

			return TRUE;
		}
	}
	break;
	case WM_MOUSEMOVE:
	{	// 如果鼠标的移动在子窗口就向下传递
		ClipboardWnd* wnd = ClipboardHitTest(pMsg->pt);
		if (wnd != m_curMouseClipboardWnd)
		{
			if (m_curMouseClipboardWnd != NULL)
			{
				::SendMessage(m_curMouseClipboardWnd->m_hWnd, WM_MOUSEMOVE, pMsg->wParam, (LPARAM)(0xFFFFFFFF));
			}
			if (wnd != NULL)
			{
				RECT rect = { 0 };
				::GetWindowRect(wnd->m_hWnd, &rect);
				int x = pMsg->pt.x, y = pMsg->pt.y;
				x -= rect.left;
				y -= rect.top;
				::SendMessage(wnd->m_hWnd, WM_MOUSEMOVE, pMsg->wParam, (LPARAM)(MAKELONG(x,y)));
			}
			m_curMouseClipboardWnd = wnd;
		}
		if (wnd)
		{
			return TRUE;
		}
	}
	break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


ClipboardWnd* CMultiClipBoardDlg::ClipboardHitTest(POINT &pt)
{
	ClipboardWnd* clipboardWnd = NULL, *tmpWnd = NULL;
	POSITION pos = m_clipboardWndList.GetHeadPosition();

	while (pos)
	{
		RECT rect = { 0 };
		tmpWnd = m_clipboardWndList.GetNext(pos);
		::GetWindowRect(tmpWnd->m_hWnd, &rect);
		if (YunXing::YxWindowUtility::PointInRect(&pt, &rect))
		{
			clipboardWnd = tmpWnd;
			break;
		}
	}
	return clipboardWnd;
}


// 分配id, 将数据保存到数据链表中
BOOL CMultiClipBoardDlg::AllocNewWnd(ClipboardWnd & clipboardWnd)
{
	BOOL ret = FALSE;
	DWORD dwListSize = m_clipboardWndList.GetSize() + 1;
	ClipboardWnd* pClipboardWnd = NULL;

	// 查找未分配的id
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
			// 分配id，并且创建新的窗口，添加到list
			clipboardWnd.m_id = i;
			pClipboardWnd = new ClipboardWnd();
			CRect rc(0, 0, 100, 100);
			if (pClipboardWnd->Create(_T("static"), WS_VISIBLE | WS_SYSMENU | WS_CHILD, rc, this))
			{
				m_clipboardWndList.AddTail(pClipboardWnd);
				*pClipboardWnd = clipboardWnd;
				if (m_curClipboardWnd == NULL)
				{
					m_curClipboardWnd = pClipboardWnd;
					::SendMessage(m_curClipboardWnd->m_hWnd, WM_SWITCHCLIPBOARD, TRUE, 0);
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


BOOL CMultiClipBoardDlg::AdjustWndLayout()
{
	BOOL ret = FALSE;

	// 调整矩形框的位置
	CRect averageRectSize;							// 矩形框平均尺寸
	ClipboardWnd* pClipboardWnd = NULL;						// 每组剪切板数据
	DWORD dwListSize = m_clipboardWndList.GetSize();
	POSITION arrayPosition = m_clipboardWndList.GetHeadPosition();

	// 获取每行的个数
	int eachLineCount = CalculateDrawRectSize(dwListSize, averageRectSize);

	for (DWORD i = 0; i < dwListSize && arrayPosition; i++)
	{
		// 调整 tmpRect的位置
		int x = 0, y = 0;
		CRect destRect = averageRectSize;
		pClipboardWnd = m_clipboardWndList.GetNext(arrayPosition);

		x = i % eachLineCount * (RectPadding + averageRectSize.Width()) + RectPadding;
		y = i / eachLineCount * (RectPadding + averageRectSize.Height()) + RectPadding;
		destRect.left += x;
		destRect.right += x;
		destRect.top += y;
		destRect.bottom += y;

		// 设置新窗口的位置
		pClipboardWnd->MoveWindow(&destRect, FALSE);
	}

	return ret;
}

/*
	响应切换剪切板视图的消息
*/
LRESULT CMultiClipBoardDlg::OnSwitchClipboard(WPARAM wParam, LPARAM lParam)
{
	POSITION position = m_clipboardWndList.GetHeadPosition();
	POSITION headPos = position;
	ClipboardWnd* clipboardWnd = NULL, *tmpWnd = NULL;

	// 避免快速切换
	if (!m_bSwitchNext)
	{
		return S_OK;
	}
	m_bSwitchNext = false;
	SetTimer(ID_TIMER_INTERVAL, 200, NULL);
	OutPutString(_T("[%s]"), __FUNCTIONW__);

	for (; position != nullptr; )
	{
		clipboardWnd = m_clipboardWndList.GetNext(position);

		if (m_curClipboardWnd->m_id == clipboardWnd->m_id)
		{
			// 查找后一个
			if (!wParam)
			{
				if (position == nullptr)
				{
					position = m_clipboardWndList.GetHeadPosition();
				}
				tmpWnd = m_clipboardWndList.GetNext(position);
				break;
			}
			// 查找前一个
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
				tmpWnd = m_clipboardWndList.GetPrev(position);
				break;
			}
		}
	}

	if (tmpWnd != NULL && m_curClipboardWnd != tmpWnd)
	{
		if (m_curClipboardWnd != NULL)
		{
			::SendMessage(m_curClipboardWnd->m_hWnd, WM_SWITCHCLIPBOARD, FALSE, 0);
		}
		::SendMessage(tmpWnd->m_hWnd, WM_SWITCHCLIPBOARD, TRUE, 0);
		m_curClipboardWnd = tmpWnd;
		SetClipboardDataByCurrent();
	}

	return 0;
}

BOOL CMultiClipBoardDlg::SetClipboardDataByCurrent()
{
	BOOL success = FALSE;

	do 
	{
		WCHAR clipboardFormat[MAX_PATH] = { 0 };
		m_openClipboardLock = true;

		BOOL bOperate = OpenClipboard();
		if (!bOperate)
		{
			OutPutString(L"OnDrawClipboard :OpenClipboard error = %d", GetLastError());
		}

		// 数据拷贝到系统剪切板
		EmptyClipboard();
		for (int i = 0; i < m_curClipboardWnd->m_clipboardArray.GetSize(); i++)
		{
			ClipboardData tmpData = m_curClipboardWnd->m_clipboardArray.GetAt(i);
			HGLOBAL hData = GlobalAlloc(GHND, tmpData.m_dwDataSize);
			LPVOID pData = GlobalLock(hData);

			memcpy_s(pData, tmpData.m_dwDataSize, tmpData.m_lpData, tmpData.m_dwDataSize);
			GlobalUnlock(hData);
			SetClipboardData(tmpData.m_dwFormat, hData);

			GetClipboardFormatName(tmpData.m_dwFormat, clipboardFormat, MAX_PATH);
			OutPutString(L"format = %x, name = %s", tmpData.m_dwFormat, clipboardFormat);
		}

		CloseClipboard();
		Sleep(10);
		m_openClipboardLock = false;
	} while (FALSE);

	return success;
}


void CMultiClipBoardDlg::OnClipboardUpdate()
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClipboardUpdate();
}


void CMultiClipBoardDlg::OnDestroyClipboard()
{
	CDialogEx::OnDestroyClipboard();

	// TODO: 在此处添加消息处理程序代码
}

// 处理SetClipboardData 事件，从此事件中获取数据
// 并存储到内存中
void CMultiClipBoardDlg::OnDrawClipboard()
{
	CDialogEx::OnDrawClipboard();
	if (m_openClipboardLock)
	{	// 自己赋值
		return;
	}

	// 接收到消息，将数据保存到当前的数据类
	BOOL bOperate = OpenClipboard();
	if (!bOperate)
	{
		OutPutString(L"OnDrawClipboard :OpenClipboard error = %d", GetLastError());
		return;
	}

	// 枚举剪切板中的数据保存下来
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

	// 存到链表里，并且调整布局
	if (clipboardWnd.m_clipboardArray.GetSize() == 0)
	{	// 没有数据
		return;
	}
	else
	{
		if (AllocNewWnd(clipboardWnd))
		{
			RECT clientRect = { 0 };
			AdjustWndLayout();
			if (CalculateWindow(clientRect))
			{
				MoveWindow(&clientRect);
				RedrawWindow();
			}
		}
	}

	//CDialogEx::OnPaint();
}


/**
	选取屏幕上可供显示的范围。
	具体算法：
			先缩小矩形框的尺寸，再增加行数。如果排不下，先将大小减小，如果再排不下，
		则增加行数，如此循环。
*/
int CMultiClipBoardDlg::CalculateDrawRectSize(const DWORD dwCount, _Out_ RECT &rect)
{
	const int RectMinWidth = 100;			// 显示矩形框的最小宽度
	const int RectMinHeight = 100;			// 显示矩形框的最小高度
	const int RectMaxPercent = 80;			// 矩形框加起来最大不超过屏幕的范围 0.8
	const int RectDecrease = 20;			// 矩形框大小每次递减的像素

	int iScreenX = 0, iScreenY = 0;			// 可供显示范围

	float screenScale = 0, windowScale = 0;		// 屏幕的长款比，和窗口的长宽比，用来调整是增加行还是减少宽度

	// 可供显示的范围
	iScreenX = GetSystemMetrics(SM_CXSCREEN) * 4 / 5;
	iScreenY = GetSystemMetrics(SM_CYSCREEN) * 4 / 5;

	screenScale = (float)iScreenX / iScreenY;

	int RectMaxWidth;						// 显示矩形框的最大宽度，最大为显示范围的一半
	int RectMaxHeight;						// 显示矩形框的最小高度
	// 矩形框显示的最大区域
	RectMaxWidth = iScreenX / 2;
	RectMaxHeight = iScreenY / 2;

	RectMaxWidth = RectMaxHeight = min(RectMaxWidth, RectMaxHeight);

	ZeroMemory(&rect, sizeof(RECT));

	int iMaxCountPerLine = dwCount;			// 每行最多存放的个数
	int iLine = 1;							// 显示多少行
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

		do
		{
			// 暂时只判断横向是否够，矩形框的大小和数量递减
			int iRemainPixWidth = (iScreenX - RectPadding) - (iMaxCountPerLine * (RectPadding + RectMaxWidth));
			int iRemainPixHeight = (iScreenY - RectPadding) - (iLine * (RectPadding + RectMaxHeight));
			if (iRemainPixWidth < 0 || iRemainPixHeight < 0)
			{

				windowScale = ((float)(RectPadding + RectMaxWidth) * iMaxCountPerLine) / ((RectPadding + RectMaxHeight) * iLine);

				// 减小尺寸还是增加行数
				if (windowScale < screenScale)
				{	// 减小尺寸
					RectMaxWidth -= RectDecrease;
					RectMaxHeight = RectMaxWidth;
				}
				else
				{	// 每行个数减小
					iMaxCountPerLine--;
				}
				iLine = (dwCount + iMaxCountPerLine - 1) / iMaxCountPerLine;
				if (RectMaxWidth < RectMinWidth || iMaxCountPerLine <= 0)
				{	// 放不下，退出
					iLine = 0;
					break;
				}
			}
			else
			{
				iLine = (dwCount + iMaxCountPerLine - 1) / iMaxCountPerLine;
				rect.left = 0;
				rect.top = 0;
				rect.right = RectMaxWidth;
				rect.bottom = RectMaxWidth;
				break;
			}

		} while (true);

	} while (FALSE);

	return iMaxCountPerLine;
}

// 计算显示所有数据所需窗口的大小
BOOL CMultiClipBoardDlg::CalculateWindow(RECT &rect)
{
	BOOL bSuccess = FALSE;

	DWORD dwCount = m_clipboardWndList.GetSize();
	CRect averageRect;
	RECT windowRect = { 0 };
	int x = 0, y = 0;

	::GetWindowRect(::GetDesktopWindow(), &windowRect);

	// 传入桌面的大小，和剪切板数据的个数
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

	// 输入法对本线程无效
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
		KillTimer(ID_TIMER_INTERVAL);
		break;

	case ID_TIMER_CHECKFOCUS:
	{
		// 判断焦点和ctrl键是否按下，如果没有焦点或者ctrl键没有按下
		// 则隐藏窗口
		DWORD focusProcessId = 0;
		GetWindowThreadProcessId(::GetFocus(), &focusProcessId);
		if (GetCurrentProcessId() != focusProcessId
			|| ((GetKeyState(VK_CONTROL) & 0xF0) == 0))
		{
			if (m_isPressedControl)
			{
				m_isPressedControl = false;
				m_isPressedShift = false;
				ShowWindow(SW_HIDE);
				RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');
			}
		}
	}
	break;
	case ID_TIMER_DESTORYCLIPBOARD:
	{
		KillTimer(ID_TIMER_DESTORYCLIPBOARD);
		DestoryClipboardWnd();
	}
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


ClipboardWnd* CMultiClipBoardDlg::FindClipboardWndById(int id)
{
	ClipboardWnd* wnd = NULL;

	do 
	{
		if (m_clipboardWndList.GetSize() > 0)
		{
			POSITION pos = NULL;
			pos = m_clipboardWndList.GetHeadPosition();
			while (pos != NULL)
			{
				wnd = m_clipboardWndList.GetNext(pos);
				if (wnd->m_id == id)
				{
					break;
				}
			}
		}
	} while (FALSE);

	return wnd;
}

void CMultiClipBoardDlg::DestoryClipboardWnd()
{
	POSITION pos = m_destoryList.GetHeadPosition();

	while (pos)
	{
		DWORD wndId = m_destoryList.GetNext(pos);
		m_destoryList.RemoveHead();
		ClipboardWnd* wnd = FindClipboardWndById(wndId);
		if (wnd != NULL)
		{
			if (m_curClipboardWnd == wnd)
			{
				OnSwitchClipboard(TRUE, 0);
			}

			// 移除，并且调整窗口位置
			m_clipboardWndList.RemoveAt(m_clipboardWndList.Find(wnd));
			AdjustWndLayout();
			RECT rect = { 0 };
			if (CalculateWindow(rect))
			{
				MoveWindow(&rect);
				RedrawWindow();
			}
			delete wnd;
		}
	}
}


LRESULT CMultiClipBoardDlg::OnDestoryClipboardWnd(WPARAM wParam, LPARAM lParam)
{
	int wndId = (wParam);
	if (wndId < 0)
	{
		return 1;
	}
	SetTimer(ID_TIMER_DESTORYCLIPBOARD, 100, NULL);
	m_destoryList.AddTail(wndId);
	return 0;
}


LRESULT CMultiClipBoardDlg::OnClipboardSelected(WPARAM wParam, LPARAM lParam)
{
	int wndId = (wParam);
	if (wndId < 0)
	{
		return 1;
	}

	ClipboardWnd* wnd = FindClipboardWndById(wndId);
	if (wnd != NULL)
	{
		m_curClipboardWnd = wnd;
		SetClipboardDataByCurrent();
	}

	return 0;
}


void CMultiClipBoardDlg::OnTrayQuit()
{
	OnClose();
}


void CMultiClipBoardDlg::OnTrayConfig()
{
	CConfigDlg dlg(this);
	dlg.DoModal();
}


void CMultiClipBoardDlg::OnTrayAbout()
{
	CActiveDialog dlg;
	dlg.DoModal();
}


void CMultiClipBoardDlg::OnTrayPause()
{
	CMenu* subMenu = m_menu.GetSubMenu(0);
	CString itemString;
	subMenu->GetMenuStringW(IDM_TRAY_PAUSE, itemString, MF_BYCOMMAND);

	if (itemString.Compare(_T("暂停")) == 0)
	{
		ChangeClipboardChain(m_hNextClipboardViewer);
		m_hNextClipboardViewer = NULL;
		UnregisterHotKey(m_hWnd, ID_HOTKEY);
		subMenu->ModifyMenuW(IDM_TRAY_PAUSE, MF_BYCOMMAND | MF_STRING, IDM_TRAY_PAUSE, _T("继续"));
	}
	else if (itemString.Compare(_T("继续")) == 0)
	{
		m_hNextClipboardViewer = SetClipboardViewer();
		// 注册快捷键
		RegisterHotKey(m_hWnd, ID_HOTKEY, MOD_CONTROL, L'Q');

		subMenu->ModifyMenuW(IDM_TRAY_PAUSE, MF_BYCOMMAND | MF_STRING, IDM_TRAY_PAUSE, _T("暂停"));
	}
}


void CMultiClipBoardDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	OutPutString(_T("[%s], wnd = %x"), __FUNCTIONW__, m_hWnd);
}


void CMultiClipBoardDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	OutPutString(_T("[%s], wnd = %x"), __FUNCTIONW__, m_hWnd);
}


afx_msg LRESULT CMultiClipBoardDlg::OnSwitchAlphaChanged(WPARAM wParam, LPARAM lParam)
{
	SetLayeredWindowAttributes(0xFFFFFF, (byte)wParam, LWA_ALPHA);
	return 0;
}