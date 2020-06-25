#pragma once
#include "stdafx.h"

class CExceptClass
{
public:
	// ���� �ڵ�� �޽����� �Է¹޴´�.
	CExceptClass(const WCHAR* error, char* data, int size)
	{
		time_t now = time(NULL);
		struct tm date;
		localtime_s(&date, &now);

		wcsftime(m_textTitle, 64, L"StreamBuffer_%Y%m%d_%H%M%S.txt", &date);
		pFile = _wfopen(m_textTitle, L"wb");
		wcscat(m_szError, error);
		memcpy(m_szData, data, size);
		fclose(pFile);
	}

	// ������ ��Ʈ���� ������ ���� ������� ���, ����ȭ ������ ������� ũ�⸦ ���ڷ� ����
	void PrintInput(int size);


private:
	FILE* pFile;
	int m_iErrorCode;
	WCHAR m_textTitle[64] = { 0 };
	WCHAR m_szError[128] = { 0 };
	char m_szData[1400] = { 0 };

};