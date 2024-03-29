// CConfigDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MultiClipBoard.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"


CConfig* CConfig::m_instance = nullptr;

CConfig::CConfig()
{
	m_keyPath = _T("Software\\YunXing\\Multiclipboard");
	m_startRunPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	m_writtenRecord = FALSE;
	m_switchColor = RGB(100, 100, 100);
	m_mouseColor = RGB(240, 240, 240);
	m_noMouseColor = RGB(0, 0, 0);
	m_selectedColor = RGB(230, 0, 0);
	m_noSelectedColor = RGB(20, 240, 20);
	m_textColor = RGB(0, 200, 240);
	m_switchAlpha = 200;
	m_settingAlpha = 200;

	Read();
}
CConfig::~CConfig()
{

}

BOOL CConfig::Read()
{
	BOOL sussess = FALSE;
	DWORD retSize = 0;
	HKEY regKey = NULL;
	DWORD keyType = 0;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, m_keyPath.GetBuffer(), 0, KEY_ALL_ACCESS, &regKey) == ERROR_SUCCESS)
	{
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("AutoStart"), NULL, &keyType, (LPBYTE)&m_autoStart, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("WriteRecord"), NULL, &keyType, (LPBYTE)&m_writtenRecord, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("SwitchColor"), NULL, &keyType, (LPBYTE)&m_switchColor, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("MouseColor"), NULL, &keyType, (LPBYTE)&m_mouseColor, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("NoMouseColor"), NULL, &keyType, (LPBYTE)&m_noMouseColor, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("SelectedColor"), NULL, &keyType, (LPBYTE)&m_selectedColor, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("NoSelectedColor"), NULL, &keyType, (LPBYTE)&m_noSelectedColor, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("TextColor"), NULL, &keyType, (LPBYTE)&m_textColor, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("SwitchAlpha"), NULL, &keyType, (LPBYTE)&m_switchAlpha, &retSize);
		retSize = 4;
		keyType = REG_DWORD;
		RegQueryValueEx(regKey, _T("SettingAlpha"), NULL, &keyType, (LPBYTE)&m_settingAlpha, &retSize);
		RegCloseKey(regKey);
		regKey = NULL;
	}

	return sussess;
}

BOOL CConfig::Write()
{
	BOOL sussess = FALSE;
	HKEY regKey = NULL;
	DWORD keyType = 0;
	DWORD valueSize = 4;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, m_startRunPath.GetBuffer(), 0, KEY_ALL_ACCESS, &regKey) == ERROR_SUCCESS)
	{
		if (m_autoStart)
		{
			TCHAR modulePath[MAX_PATH] = { 0 };
			if (GetModuleFileName(NULL, modulePath, MAX_PATH))
			{
				RegSetValueEx(regKey, _T("Multiclipboard"), NULL, REG_SZ, (LPBYTE)&modulePath, sizeof(modulePath));
			}
		}
		else
		{
			RegDeleteValue(regKey, _T("Multiclipboard"));
		}
		RegCloseKey(regKey);
		regKey = NULL;
	}
	if (RegOpenKeyEx(HKEY_CURRENT_USER, m_keyPath.GetBuffer(), 0, KEY_ALL_ACCESS, &regKey) == ERROR_SUCCESS
		|| RegCreateKeyEx(HKEY_CURRENT_USER, m_keyPath.GetBuffer(), NULL, NULL, 0, KEY_ALL_ACCESS, NULL, &regKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueEx(regKey, _T("AutoStart"), NULL, REG_DWORD, (LPBYTE)&m_autoStart, valueSize);
		RegSetValueEx(regKey, _T("WriteRecord"), NULL, REG_DWORD, (LPBYTE)&m_writtenRecord, valueSize);
		RegSetValueEx(regKey, _T("SwitchColor"), NULL, REG_DWORD, (LPBYTE)&m_switchColor, valueSize);
		RegSetValueEx(regKey, _T("MouseColor"), NULL, REG_DWORD, (LPBYTE)&m_mouseColor, valueSize);
		RegSetValueEx(regKey, _T("NoMouseColor"), NULL, REG_DWORD, (LPBYTE)&m_noMouseColor, valueSize);
		RegSetValueEx(regKey, _T("SelectedColor"), NULL, REG_DWORD, (LPBYTE)&m_selectedColor, valueSize);
		RegSetValueEx(regKey, _T("NoSelectedColor"), NULL, REG_DWORD, (LPBYTE)&m_noSelectedColor, valueSize);
		RegSetValueEx(regKey, _T("TextColor"), NULL, REG_DWORD, (LPBYTE)&m_textColor, valueSize);
		RegSetValueEx(regKey, _T("SwitchAlpha"), NULL, REG_DWORD, (LPBYTE)&m_switchAlpha, valueSize);
		RegSetValueEx(regKey, _T("SettingAlpha"), NULL, REG_DWORD, (LPBYTE)&m_settingAlpha, valueSize);

		RegCloseKey(regKey);
		regKey = NULL;
	}

	return sussess;
}

// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIG_DIALOG, pParent)
	, m_autoStart(FALSE)
	, m_saveRecord(FALSE)
{
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_SWITCH_ALPHA, m_switchAlpha);
	DDX_Control(pDX, IDC_SLIDER_SETTING_ALPHA, m_settingAlpha);
	DDX_Check(pDX, IDC_CHECK_AUTO, m_autoStart);
	DDX_Check(pDX, IDC_CHECK_RECORD, m_saveRecord);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SWITCH_COLOR, &CConfigDlg::OnBnClickedBtnSwitchColor)
	ON_BN_CLICKED(IDC_BTN_MOUSE_COLOR, &CConfigDlg::OnClickedBtnMouseColor)
	ON_BN_CLICKED(IDC_BTN_NO_MOUSE_COLOR, &CConfigDlg::OnClickedBtnNoMouseColor)
	ON_BN_CLICKED(IDC_BTN_NO_SELECTED_COLOR, &CConfigDlg::OnClickedBtnNoSelectedColor)
	ON_BN_CLICKED(IDC_BTN_SELECTED_COLOR, &CConfigDlg::OnClickedBtnSelectedColor)
	ON_BN_CLICKED(IDC_BTN_TEXT_COLOR, &CConfigDlg::OnClickedBtnTextColor)
	ON_BN_CLICKED(IDC_BTN_QUIT, &CConfigDlg::OnClickedBtnQuit)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CConfigDlg::OnClickedBtnSave)
	ON_WM_CTLCOLOR()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序


void CConfigDlg::OnBnClickedBtnSwitchColor()
{
	CColorDialog dlg(m_switchColor, CC_SOLIDCOLOR);
	if (dlg.DoModal() == IDOK)
	{
		m_switchColor = dlg.GetColor();
	}
}


void CConfigDlg::OnClickedBtnMouseColor()
{
	CColorDialog dlg(m_mouseColor, CC_PREVENTFULLOPEN);
	if (dlg.DoModal() == IDOK)
	{
		m_mouseColor = dlg.GetColor();
	}
}


void CConfigDlg::OnClickedBtnNoMouseColor()
{
	CColorDialog dlg(m_noMouseColor, CC_ANYCOLOR);
	if (dlg.DoModal() == IDOK)
	{
		m_noMouseColor = dlg.GetColor();
	}
}


void CConfigDlg::OnClickedBtnNoSelectedColor()
{
	CColorDialog dlg(m_noSelectedColor, CC_ANYCOLOR);
	if (dlg.DoModal() == IDOK)
	{
		m_noSelectedColor = dlg.GetColor();
	}
}


void CConfigDlg::OnClickedBtnSelectedColor()
{
	CColorDialog dlg(m_selectedColor, CC_ANYCOLOR);
	if (dlg.DoModal() == IDOK)
	{
		m_selectedColor = dlg.GetColor();
	}
}


void CConfigDlg::OnClickedBtnTextColor()
{
	CColorDialog dlg(m_textColor, CC_ANYCOLOR);
	if (dlg.DoModal() == IDOK)
	{
		m_textColor = dlg.GetColor();
	}
}


void CConfigDlg::OnClickedBtnQuit()
{
	CDialogEx::OnCancel();
}


void CConfigDlg::OnClickedBtnSave()
{
	UpdateData(TRUE);
	CConfig* cfg = cfg->GetInstance();
	cfg->m_autoStart = m_autoStart;
	cfg->m_writtenRecord = m_saveRecord;
	cfg->m_switchColor = m_switchColor;
	cfg->m_mouseColor = m_mouseColor;
	cfg->m_noMouseColor = m_noMouseColor;
	cfg->m_selectedColor = m_selectedColor;
	cfg->m_noSelectedColor = m_noSelectedColor;
	cfg->m_textColor = m_textColor;
	cfg->m_switchAlpha = m_switchAlpha.GetPos();
	cfg->m_settingAlpha = m_settingAlpha.GetPos();

	::SendMessage(GetParent()->m_hWnd, WM_SWITCHALPHACHANGED, cfg->m_switchAlpha, 0);
	cfg->Write();
	CDialogEx::OnOK();
}


BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CConfig* cfg = CConfig::GetInstance();

	m_saveRecord = cfg->m_writtenRecord;
	m_autoStart = cfg->m_autoStart;
	m_switchColor = cfg->m_switchColor;
	m_mouseColor = cfg->m_mouseColor;
	m_noMouseColor = cfg->m_noMouseColor;
	m_selectedColor = cfg->m_selectedColor;
	m_noSelectedColor = cfg->m_noSelectedColor;
	m_textColor = cfg->m_textColor;
	m_switchAlpha.SetRange(0, 255);
	m_switchAlpha.SetTicFreq(5);
	m_switchAlpha.SetLineSize(5);
	m_switchAlpha.SetPos(cfg->m_switchAlpha);
	m_settingAlpha.SetRange(0, 255);
	m_settingAlpha.SetTicFreq(5);
	m_settingAlpha.SetLineSize(5);
	m_settingAlpha.SetPos(cfg->m_settingAlpha);

	ULONG wndStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, wndStyle | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0xFFFFFF, (byte)cfg->m_switchAlpha, LWA_ALPHA);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


HBRUSH CConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = NULL;
	LOGBRUSH logBr = { 0 };

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->m_hWnd == GetDlgItem(IDC_BTN_SWITCH_COLOR)->m_hWnd)
	{
		logBr.lbColor = m_switchColor;
		logBr.lbStyle = BS_SOLID;
		hbr = CreateBrushIndirect(&logBr);
	}
	else if (pWnd->m_hWnd == GetDlgItem(IDC_BTN_MOUSE_COLOR)->m_hWnd)
	{
		logBr.lbColor = m_mouseColor;
		logBr.lbStyle = BS_SOLID;
		hbr = CreateBrushIndirect(&logBr);
	}
	else if (pWnd->m_hWnd == GetDlgItem(IDC_BTN_NO_MOUSE_COLOR)->m_hWnd)
	{
		logBr.lbColor = m_noMouseColor;
		logBr.lbStyle = BS_SOLID;
		hbr = CreateBrushIndirect(&logBr);
	}
	else if (pWnd->m_hWnd == GetDlgItem(IDC_BTN_SELECTED_COLOR)->m_hWnd)
	{
		logBr.lbColor = m_selectedColor;
		logBr.lbStyle = BS_SOLID;
		hbr = CreateBrushIndirect(&logBr);
	}
	else if (pWnd->m_hWnd == GetDlgItem(IDC_BTN_NO_SELECTED_COLOR)->m_hWnd)
	{
		logBr.lbColor = m_noSelectedColor;
		logBr.lbStyle = BS_SOLID;
		hbr = CreateBrushIndirect(&logBr);
	}
	else if (pWnd->m_hWnd == GetDlgItem(IDC_BTN_TEXT_COLOR)->m_hWnd)
	{
		logBr.lbColor = m_textColor;
		logBr.lbStyle = BS_SOLID;
		hbr = CreateBrushIndirect(&logBr);
	}
	else
	{
		hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CConfigDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (pScrollBar->m_hWnd == m_settingAlpha.m_hWnd)
	{
		byte sliderPos = m_settingAlpha.GetPos();
		SetLayeredWindowAttributes(0xFFFFFF, sliderPos, LWA_ALPHA);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
