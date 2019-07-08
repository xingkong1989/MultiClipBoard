// ActiveDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MultiClipBoard.h"
#include "ActiveDialog.h"
#include "afxdialogex.h"


// CActiveDialog 对话框

IMPLEMENT_DYNAMIC(CActiveDialog, CDialogEx)

CActiveDialog::CActiveDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ACTIVE_DIALOG, pParent)
{

}

CActiveDialog::~CActiveDialog()
{
}

void CActiveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CActiveDialog, CDialogEx)
END_MESSAGE_MAP()


// CActiveDialog 消息处理程序


BOOL CActiveDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
