// ActiveDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MultiClipBoard.h"
#include "ActiveDialog.h"
#include "afxdialogex.h"


// CActiveDialog �Ի���

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


// CActiveDialog ��Ϣ�������


BOOL CActiveDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
