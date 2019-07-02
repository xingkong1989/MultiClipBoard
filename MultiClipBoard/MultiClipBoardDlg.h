
// MultiClipBoardDlg.h : 头文件
//

#pragma once
#include "DataWnd.h"
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

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTraynotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSwitchClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuQuit();
	DECLARE_MESSAGE_MAP()
	
	
	/**
		@function:	根据屏幕大小及需要绘制的矩形框数量计算出矩形框的大小

		@param dwCount			需要绘制矩形框的数量
		@param rect				返回矩形框的大小

		@return		返回每行显示矩形框的个数
	*/
	int CalculateDrawRectSize(const DWORD dwCount, _Out_ CRect &rect);

	/**
		@function:	计算显示所有数据所需窗口的大小
		
		@param rect 用来返回窗口的大小

		@return		返回成功还是失败
	*/
	BOOL CalculateWindow(_Out_ CRect &rect);

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 分配一个新的窗口，并且添加到列表
	BOOL AllocNewWnd(ClipboardWnd& clipboardWnd);

	// 是否一个窗口
	BOOL FreeWnd(DWORD wndId);

	// 调整窗口的布局
	BOOL AdjustWndLayout();

	NOTIFYICONDATAW m_notifyData;				// 系统托盘数据

	bool m_isPressedControl;					// ctrl 键是否按下
	bool m_isPressedShift;						// shift 键是否按下
	bool m_isFirstShow;							// 应用程序第一次启动，不显示界面
	bool m_bSwitchNext;							// 由定时器控制，矩形框是否可用切换到下一个

	bool m_openClipboardLock;					// 当前是否是自己赋值剪切板，此处应该用临界区critical_section

	HWND m_hNextClipboardViewer;

	DWORD m_rectHeight;							// 绘图的矩形框高度
	DWORD m_rectWidth;							// 绘图的矩形框宽度
	DWORD m_rectPadding;						// 矩形框周围间距

	CList<ClipboardWnd*> m_clipboardWndList;	// 剪切板数据链表
	ClipboardWnd* m_curClipboardWnd;	// 当前选中的剪切板数据

	CString m_sInvalidChar;						// 绘制文字时不现实的字符

	COLORREF m_bkColor;							// 背景颜色
	int m_nAlpha;								// 窗口透明度

	COLORREF m_rectBKColor;						// 矩形框的背景色
	COLORREF m_frameColor;						// 矩形框的边框色
	int m_frameWidth;							// 矩形框的边框宽度

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
