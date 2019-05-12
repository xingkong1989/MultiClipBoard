#include "stdafx.h"
#include "ClipboardData.h"


ClipboardData::ClipboardData()
{
	DWORD id = 0;
	m_dwFormat = 0;
	m_hWnd = NULL;
	m_lpData = nullptr;
	m_dwDataSize = 0;
}
ClipboardData::~ClipboardData()
{
	if (m_lpData != nullptr)
	{
		free(m_lpData);
	}
}
ClipboardData::ClipboardData(const ClipboardData& other)
{
	m_dwFormat = other.m_dwFormat;
	m_hWnd = other.m_hWnd;
	if (other.m_lpData != nullptr && other.m_dwDataSize > 0)
	{
		m_dwDataSize = other.m_dwDataSize;
		m_lpData = malloc(m_dwDataSize);
		memcpy_s(m_lpData, m_dwDataSize, other.m_lpData, other.m_dwDataSize);
	}
	else
	{
		m_lpData = nullptr;
	}
}