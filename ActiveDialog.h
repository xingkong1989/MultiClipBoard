#pragma once


// CActiveDialog 对话框

class CActiveDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CActiveDialog)

public:
	CActiveDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CActiveDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACTIVE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
