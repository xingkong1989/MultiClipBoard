
// MultiClipBoardDlg.h : ͷ�ļ�
//

#pragma once
#include "ClipboardWnd.h"
#include "ClipboardData.h"


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
	afx_msg void OnTrayQuit();
	afx_msg void OnTrayConfig();
	afx_msg void OnTrayAbout();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTraynotify(WPARAM wParam, LPARAM lParam);

	// ���а�Ի����л�
	afx_msg LRESULT OnSwitchClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// ���ټ��а�Ի��򣬲����ڲ�����������
	void DestoryClipboardWnd();

	// ��Ϣ��Ӧ������ �����а�Ի���id��ӵ�����
	afx_msg LRESULT OnDestoryClipboardWnd(WPARAM wParam, LPARAM lParam);

	// �Ӵ��ڷ�����Ϣ����֪ѡ�еļ��а�Ի���
	afx_msg LRESULT OnClipboardSelected(WPARAM wParam, LPARAM lParam);

	// ��Ϣ��Ӧ������Alpha͸���ȸı�
	afx_msg LRESULT OnSwitchAlphaChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	
	/**
		@function:	������Ļ��С����Ҫ���Ƶľ��ο�������������ο�Ĵ�С

		@param dwCount			��Ҫ���ƾ��ο������
		@param rect				���ؾ��ο�Ĵ�С

		@return		����ÿ����ʾ���ο�ĸ���
	*/
	int CalculateDrawRectSize(const DWORD dwCount, _Out_ RECT &rect);

	/**
		@function:	������ʾ�����������贰�ڵĴ�С
		
		@param rect �������ش��ڵĴ�С

		@return		���سɹ�����ʧ��
	*/
	BOOL CalculateWindow(_Out_ RECT &rect);

private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// ����һ���µĴ��ڣ�������ӵ��б�
	BOOL AllocNewWnd(ClipboardWnd& clipboardWnd);

	// �������ڵĲ���
	BOOL AdjustWndLayout();

	// ���ü��а崰������
	BOOL SetClipboardDataByCurrent();

	// ����id�������������
	ClipboardWnd* FindClipboardWndById(int id);

	/**
		brief: ���а���Ϣ���У�ͨ�����а�λ���жϵ�������ĸ����а�

		@param_1: ���λ��

		return�� �������еĴ��ڣ� NULL ��ʾû������
	*/
	ClipboardWnd* ClipboardHitTest(POINT &pt);

	NOTIFYICONDATAW m_notifyData;				// ϵͳ��������

	bool m_isPressedControl;					// ctrl ���Ƿ���
	bool m_isPressedShift;						// shift ���Ƿ���
	bool m_isFirstShow;							// Ӧ�ó����һ������������ʾ����
	bool m_bSwitchNext;							// �ɶ�ʱ�����ƣ����ο��Ƿ�����л�����һ��

	bool m_openClipboardLock;					// ��ǰ�Ƿ����Լ���ֵ���а�

	HWND m_hNextClipboardViewer;

	DWORD m_rectHeight;							// ��ͼ�ľ��ο�߶�
	DWORD m_rectWidth;							// ��ͼ�ľ��ο���
	DWORD m_rectPadding;						// ���ο���Χ���

	CList<ClipboardWnd*> m_clipboardWndList;	// ���а���������
	ClipboardWnd* m_curClipboardWnd;			// ��ǰѡ�еļ��а�����
	ClipboardWnd* m_curMouseClipboardWnd;		// ��ǰ���λ��
	ClipboardWnd* m_curlBtnClickWnd;			// ��ǰ��굥����λ��

	CList<DWORD> m_destoryList;					// ��Ҫɾ���ļ��а�����

	CConfig* m_cfg;								// �����ļ�

	CMenu m_menu;								// �Ҽ��˵�
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTrayPause();
};
