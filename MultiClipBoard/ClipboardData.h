#pragma once

class ClipboardData
{
public:
	DWORD m_dwFormat;
	LPVOID m_lpData;
	DWORD m_dwDataSize;	// �ֽ�

	ClipboardData();
	~ClipboardData();
	ClipboardData(const ClipboardData& other);

	ClipboardData& operator= (const ClipboardData& other);
};
