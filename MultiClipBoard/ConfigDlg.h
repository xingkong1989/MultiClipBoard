#pragma once

/**
	describe: 配置文件，单例模式，简单实现读写注册表。
		启动项路径为：HKEY_CURRENT_USER\Software\YunXing\Multiclipboard
		配置路径为：HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run");
*/
class CConfig
{
public:
	static CConfig* GetInstance()
	{
		if (nullptr == m_instance)
		{
			m_instance = new CConfig();
		}
		return m_instance;
	}

	BOOL Read();
	BOOL Write();

	BOOL m_autoStart;				// 开机自启动
	BOOL m_writtenRecord;			// 退出时写记录
	COLORREF m_switchColor;			// 切换界面颜色
	COLORREF m_mouseColor;			// 剪切板边框色（有鼠标）
	COLORREF m_noMouseColor;		// 剪切板边框色（没有鼠标）
	COLORREF m_selectedColor;		// 剪切板被选中
	COLORREF m_noSelectedColor;		// 剪切板未被选中
	COLORREF m_textColor;			// 文本颜色
	int m_switchAlpha;				// 切换对话框alpha
	int m_settingAlpha;				// 设置对话框alpha

private:
	CConfig();
	~CConfig();

	CString m_keyPath;
	CString m_startRunPath;
	static CConfig *m_instance;
};


// CConfigDlg 对话框

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSwitchColor();
	afx_msg void OnClickedBtnMouseColor();
	afx_msg void OnClickedBtnNoMouseColor();
	afx_msg void OnClickedBtnNoSelectedColor();
	afx_msg void OnClickedBtnSelectedColor();
	afx_msg void OnClickedBtnTextColor();
	afx_msg void OnClickedBtnQuit();
	afx_msg void OnClickedBtnSave();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	BOOL m_saveRecord;
	BOOL m_autoStart;
	COLORREF m_switchColor;			// 切换界面颜色
	COLORREF m_mouseColor;			// 剪切板边框色（有鼠标）
	COLORREF m_noMouseColor;		// 剪切板边框色（没有鼠标）
	COLORREF m_selectedColor;		// 剪切板被选中
	COLORREF m_noSelectedColor;		// 剪切板未被选中
	COLORREF m_textColor;			// 文本颜色
	CSliderCtrl m_switchAlpha;
	CSliderCtrl m_settingAlpha;

};
