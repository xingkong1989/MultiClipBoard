
// MultiClipBoardDlg.h : 头文件
//

#pragma once
#include "ClipboardWnd.h"
#include "ClipboardData.h"


// CMultiClipBoardDlg 对话框
class CMultiClipBoardDlg : public CDialogEx
{
// 构造
public:
	CMultiClipBoardDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SWITCH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClipboardUpdate();
	afx_msg void OnDestroyClipboard();
	afx_msg void OnDrawClipboard();
	afx_msg void OnTrayQuit();
	afx_msg void OnTrayConfig();
	afx_msg void OnTrayAbout();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTraynotify(WPARAM wParam, LPARAM lParam);

	// 剪切板对话框切换
	afx_msg LRESULT OnSwitchClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 销毁剪切板对话框，查找内部待销毁链表
	void DestoryClipboardWnd();

	// 消息响应函数， 将剪切板对话框id添加到链表
	afx_msg LRESULT OnDestoryClipboardWnd(WPARAM wParam, LPARAM lParam);

	// 子窗口发送消息，告知选中的剪切板对话框
	afx_msg LRESULT OnClipboardSelected(WPARAM wParam, LPARAM lParam);

	// 消息响应函数，Alpha透明度改变
	afx_msg LRESULT OnSwitchAlphaChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	
	/**
		@function:	根据屏幕大小及需要绘制的矩形框数量计算出矩形框的大小

		@param dwCount			需要绘制矩形框的数量
		@param rect				返回矩形框的大小

		@return		返回每行显示矩形框的个数
	*/
	int CalculateDrawRectSize(const DWORD dwCount, _Out_ RECT &rect);

	/**
		@function:	计算显示所有数据所需窗口的大小
		
		@param rect 用来返回窗口的大小

		@return		返回成功还是失败
	*/
	BOOL CalculateWindow(_Out_ RECT &rect);

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 分配一个新的窗口，并且添加到列表
	BOOL AllocNewWnd(ClipboardWnd& clipboardWnd);

	// 调整窗口的布局
	BOOL AdjustWndLayout();

	// 设置剪切板窗口内容
	BOOL SetClipboardDataByCurrent();

	// 根据id从链表里面查找
	ClipboardWnd* FindClipboardWndById(int id);

	/**
		brief: 剪切板消息命中，通过剪切板位置判断点击的是哪个剪切板

		@param_1: 点的位置

		return： 返回命中的窗口， NULL 表示没有命中
	*/
	ClipboardWnd* ClipboardHitTest(POINT &pt);

	NOTIFYICONDATAW m_notifyData;				// 系统托盘数据

	bool m_isPressedControl;					// ctrl 键是否按下
	bool m_isPressedShift;						// shift 键是否按下
	bool m_isFirstShow;							// 应用程序第一次启动，不显示界面
	bool m_bSwitchNext;							// 由定时器控制，矩形框是否可用切换到下一个

	bool m_openClipboardLock;					// 当前是否是自己赋值剪切板

	HWND m_hNextClipboardViewer;

	DWORD m_rectHeight;							// 绘图的矩形框高度
	DWORD m_rectWidth;							// 绘图的矩形框宽度
	DWORD m_rectPadding;						// 矩形框周围间距

	CList<ClipboardWnd*> m_clipboardWndList;	// 剪切板数据链表
	ClipboardWnd* m_curClipboardWnd;			// 当前选中的剪切板数据
	ClipboardWnd* m_curMouseClipboardWnd;		// 当前鼠标位置
	ClipboardWnd* m_curlBtnClickWnd;			// 当前鼠标单击的位置

	CList<DWORD> m_destoryList;					// 需要删除的剪切板数据

	CConfig* m_cfg;								// 配置文件

	CMenu m_menu;								// 右键菜单
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTrayPause();
};
