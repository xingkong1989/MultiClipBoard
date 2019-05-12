#pragma once
#include "stdafx.h"
class ConfigData
{
public:
	unsigned char cAlpha;
	unsigned char cyRecordCount;
};

class CConfig
{
public:
	static CConfig* GetInstance()
	{
		if (nullptr == m_instance)
		{
			m_instance = new CConfig();
		}
		return m_instance;
	}

	void Get();
	void Set();

private:
	CConfig(){}
	~CConfig(){}

	int Read();
	int Write();

	static CConfig *m_instance;
};

CConfig* CConfig::m_instance = nullptr;

void CConfig::Get()
{

}

void CConfig::Set()
{

}

int CConfig::Read()
{
	int retSize = 0;
	return retSize;
}

int CConfig::Write()
{
	int retSize = 0;
	return retSize;
}