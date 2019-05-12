#pragma once


/**
	@function:	������Ч�ַ���

	@param wzSrc			Դ�ַ���
	@param cSrc				Դ�ַ�������
	@param wzDest			Ŀ���ַ���
	@param cDest			Ŀ���ַ�������
	@parm invalidChar		��Ч���ַ�����CString��ʽ����

@return		���˺���Ч���ַ�������
*/
int FilterInValidCharW(const wchar_t* wzSrc, int cSrc, wchar_t* wzDest, int cDest, CString invalidChar);

/**
	@function:	�ھ��ο��ϻ�������

	@param wzStr			��Ҫ���Ƶ����֣�����unicode
	@param pDc				CDC ָ��
	@param rt				���ο�Χ
	@parm invalidChar		��Ч���ַ�����CString��ʽ����

	@return		���سɹ�����ʧ��
*/
bool DrawClipboardText(const wchar_t* wzStr, CDC* pDc, RECT *rt, CString invalidChar);

/**
	@functino:	�ھ��ο��л���λͼ�����ݾ��ο�ĸ߶�ѡ���ʵ��Ŀ��

	@param pDC				CDCָ��
	@param hBitmap			λͼ���
	@param rt				���ο�Χ������Χʵ��ʹ�õķ�Χ

	@return		���سɹ�����ʧ��
*/
BOOL DrawBitmap(CDC* pDC, const HBITMAP hBitmap, RECT *rt);
