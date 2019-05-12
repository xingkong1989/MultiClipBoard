
// MultiClipBoardDlg.h : 头文件
//

#pragma once
#include "ClipboardData.h"

typedef CArray<CRect> CRectArray;

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
		@function:	绘制ClipboardData中的数据

		@param clipboardData	数据内容
		@param pDc				为绘制的目标dc句柄，可以为空
		@param rect				矩形框大小，传入矩形框的大小，并且返回实际所用的大小

		@return		返回成功或者失败
	*/
	BOOL DrawClipboardData(const ClipboardData& clipboardData, CDC* pDc, _Inout_  CRect &rect);


	/**
		@function:	绘制ClipboardList中的数据

		@param clipboardList	剪切板数据链表
		@param pDc				为绘制目标句柄dc句柄
		@param rect				矩形框大小，传入矩形框的大小，并且返回实际所用的大小
							同DrawClipboardData 的 第三个参数

		@return		返回成功或者失败
	*/
	BOOL DrawClipboardArray(const ClipboardArrayData& clipboardArrayData, CDC* pDc, CRect &rect);

	/**
		@function:	根据屏幕大小及需要绘制的矩形框数量计算出矩形框的大小

		@param dwCount			需要绘制矩形框的数量
		@param rect				返回矩形框的大小

		@return		返回每行显示矩形框的个数
	*/
	int CalculateDrawRectSize(const DWORD dwCount, _Out_ CRect &rect);

	/**
		@function:	计算窗口的大小
		
		@param rect				用来返回窗口的大小

		@return		返回成功还是失败
	*/
	BOOL CalculateWindow(_Out_ CRect &rect);

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

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

	CList<ClipboardArrayData> m_clipboardList;	// 剪切板数据链表
	ClipboardArrayData m_curClipboardArrayData;	// 当前选中的剪切板数据

	CString m_sInvalidChar;						// 绘制文字时不现实的字符

	COLORREF m_bkColor;							// 背景颜色
	int m_nAlpha;								// 窗口透明度

	COLORREF m_rectBKColor;						// 矩形框的背景色
	COLORREF m_frameColor;						// 矩形框的边框色
	int m_frameWidth;							// 矩形框的边框宽度

	COLORREF m_textColor;						// 文字的颜色
	int m_textWidth;							// 文字的字体宽度
	int m_textSize;								// 文字的大小
};
