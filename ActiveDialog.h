#pragma once


// CActiveDialog �Ի���

class CActiveDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CActiveDialog)

public:
	CActiveDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CActiveDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACTIVE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
