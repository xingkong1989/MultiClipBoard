#pragma once

class ClipboardData
{
public:
	DWORD m_dwFormat;
	HWND m_hWnd;
	LPVOID m_lpData;
	DWORD m_dwDataSize;	// ×Ö½Ú

	ClipboardData();
	~ClipboardData();
	ClipboardData(const ClipboardData& other);

	ClipboardData& operator = (const ClipboardData& other)
	{
		if (&other != this)
		{
			m_dwFormat = other.m_dwFormat;
			m_hWnd = other.m_hWnd;
			if (other.m_lpData != nullptr && other.m_dwDataSize > 0)
			{
				m_dwDataSize = other.m_dwDataSize;
				m_lpData = malloc(m_dwDataSize);
				memcpy_s(m_lpData, m_dwDataSize, other.m_lpData, other.m_dwDataSize);
			}
		}
		return *this;
	}
};

class ClipboardArrayData
{
public:
	DWORD m_id;
	CArray<ClipboardData> m_clipboardArray;

	BOOL operator == (ClipboardArrayData data)
	{
		return m_id == data.m_id;
	}

	ClipboardArrayData& operator = (const ClipboardArrayData& other)
	{
		if (other.m_id != m_id)
		{
			for (size_t i = 0; i < other.m_clipboardArray.GetSize(); i++)
			{
				m_clipboardArray.Add(other.m_clipboardArray.GetAt(i));
			}
			m_id = other.m_id;
		}

		return *this;
	}
};