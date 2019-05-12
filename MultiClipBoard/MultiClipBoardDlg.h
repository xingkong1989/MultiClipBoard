
// MultiClipBoardDlg.h : ͷ�ļ�
//

#pragma once
#include "ClipboardData.h"

typedef CArray<CRect> CRectArray;

// CMultiClipBoardDlg �Ի���
class CMultiClipBoardDlg : public CDialogEx
{
// ����
public:
	CMultiClipBoardDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SWITCH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClipboardUpdate();
	afx_msg void OnDestroyClipboard();
	afx_msg void OnDrawClipboard();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
		@function:	����ClipboardData�е�����

		@param clipboardData	��������
		@param pDc				Ϊ���Ƶ�Ŀ��dc���������Ϊ��
		@param rect				���ο��С��������ο�Ĵ�С�����ҷ���ʵ�����õĴ�С

		@return		���سɹ�����ʧ��
	*/
	BOOL DrawClipboardData(const ClipboardData& clipboardData, CDC* pDc, _Inout_  CRect &rect);


	/**
		@function:	����ClipboardList�е�����

		@param clipboardList	���а���������
		@param pDc				Ϊ����Ŀ����dc���
		@param rect				���ο��С��������ο�Ĵ�С�����ҷ���ʵ�����õĴ�С
							ͬDrawClipboardData �� ����������

		@return		���سɹ�����ʧ��
	*/
	BOOL DrawClipboardArray(const ClipboardArrayData& clipboardArrayData, CDC* pDc, CRect &rect);

	/**
		@function:	������Ļ��С����Ҫ���Ƶľ��ο�������������ο�Ĵ�С

		@param dwCount			��Ҫ���ƾ��ο������
		@param rect				���ؾ��ο�Ĵ�С

		@return		����ÿ����ʾ���ο�ĸ���
	*/
	int CalculateDrawRectSize(const DWORD dwCount, _Out_ CRect &rect);

	/**
		@function:	���㴰�ڵĴ�С
		
		@param rect				�������ش��ڵĴ�С

		@return		���سɹ�����ʧ��
	*/
	BOOL CalculateWindow(_Out_ CRect &rect);

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	NOTIFYICONDATAW m_notifyData;				// ϵͳ��������

	bool m_isPressedControl;					// ctrl ���Ƿ���
	bool m_isPressedShift;						// shift ���Ƿ���
	bool m_isFirstShow;							// Ӧ�ó����һ������������ʾ����
	bool m_bSwitchNext;							// �ɶ�ʱ�����ƣ����ο��Ƿ�����л�����һ��

	bool m_openClipboardLock;					// ��ǰ�Ƿ����Լ���ֵ���а壬�˴�Ӧ�����ٽ���critical_section

	HWND m_hNextClipboardViewer;

	DWORD m_rectHeight;							// ��ͼ�ľ��ο�߶�
	DWORD m_rectWidth;							// ��ͼ�ľ��ο���
	DWORD m_rectPadding;						// ���ο���Χ���

	CList<ClipboardArrayData> m_clipboardList;	// ���а���������
	ClipboardArrayData m_curClipboardArrayData;	// ��ǰѡ�еļ��а�����

	CString m_sInvalidChar;						// ��������ʱ����ʵ���ַ�

	COLORREF m_bkColor;							// ������ɫ
	int m_nAlpha;								// ����͸����

	COLORREF m_rectBKColor;						// ���ο�ı���ɫ
	COLORREF m_frameColor;						// ���ο�ı߿�ɫ
	int m_frameWidth;							// ���ο�ı߿���

	COLORREF m_textColor;						// ���ֵ���ɫ
	int m_textWidth;							// ���ֵ�������
	int m_textSize;								// ���ֵĴ�С
};
