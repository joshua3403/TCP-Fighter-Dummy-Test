#include "stdafx.h"
#include "CMessage.h"

void CMessage::PutData(char* data, int size)
{
	int realSize = GetFreeSize() - size;
	if (realSize < 0)
	{
		while (realSize < 0)
		{
			IncreaseBufferSize(size);
			realSize = GetFreeSize() - size;
		}
	}

	realSize = size;
	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)data, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;
}

void CMessage::GetData(char* data, int size)
{
	int realSize = GetDataSize() - size;
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> SHORT ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = size;
	char* ptr = m_cpBuffer;
	ptr += m_iFront;
	memcpy(data, ptr, size);
	m_iUsingSize -= size;
	m_iFront += realSize;
}

void CMessage::IncreaseBufferSize(int size)
{
	int realSize = GetFreeSize() - size;
	char newBuffer[1000] = { 0 };
	while (realSize < 0)
	{
		memcpy(newBuffer, m_cpBuffer, m_iMaxSize);
		m_iMaxSize += size;
		free(m_cpBuffer);
		m_cpBuffer = (char*)malloc(sizeof(char) * (m_iMaxSize));
		memcpy(m_cpBuffer, newBuffer, m_iMaxSize);
		realSize = GetFreeSize() - size;
	}
}

CMessage& CMessage::operator=(CMessage& clSrCMessage)
{
	// TODO: ���⿡ return ���� �����մϴ�.
	if (this == &clSrCMessage)
		return *this;

	free(m_cpBuffer);
	m_cpBuffer = (char*)malloc(((clSrCMessage).GetBufferSize()));
	memcpy(m_cpBuffer, (clSrCMessage).GetBufferPtr(), sizeof(m_cpBuffer));
	m_iFront = (clSrCMessage).GetFront();
	m_iRear = (clSrCMessage).GetRear();
	m_iMaxSize = (clSrCMessage).GetBufferSize();
	m_iUsingSize = (clSrCMessage).GetDataSize();

	return *this;
}

CMessage& CMessage::operator<<(BYTE byValue)
{
	int realSize = GetFreeSize() - sizeof(byValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_BYTE);
		realSize = sizeof(byValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&byValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< BYTE ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
	}

	realSize = sizeof(byValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&byValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(char chValue)
{
	int realSize = GetFreeSize() - sizeof(chValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_BYTE);
		realSize = sizeof(chValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&chValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< CHAR ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
	}

	realSize = sizeof(chValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&chValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(short shValue)
{
	int realSize = GetFreeSize() - sizeof(shValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_SHORT);
		realSize = sizeof(shValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&shValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< SHORT ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
	}

	realSize = sizeof(shValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&shValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(WORD wValue)
{
	int realSize = GetFreeSize() - sizeof(wValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_SHORT);
		realSize = sizeof(wValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&wValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< WORD ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
	}

	realSize = sizeof(wValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&wValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(int iValue)
{
	int realSize = GetFreeSize() - sizeof(iValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_INT);
		realSize = sizeof(iValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&iValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< INT ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
	}

	realSize = sizeof(iValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&iValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(DWORD dwValue)
{
	int realSize = GetFreeSize() - sizeof(dwValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_INT);
		realSize = sizeof(dwValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&dwValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< DWORD ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
		return *this;
	}

	realSize = sizeof(dwValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&dwValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(float fValue)
{
	int realSize = GetFreeSize() - sizeof(fValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_INT);
		realSize = sizeof(fValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&fValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< FLOAT ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
		return *this;
	}

	realSize = sizeof(fValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&fValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(__int64 iValue)
{
	int realSize = GetFreeSize() - sizeof(iValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_INT64);
		realSize = sizeof(iValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&iValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		//// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		//CExceptClass* Except = new CExceptClass(L"<< INT64 ERROR", m_cpBuffer, m_iMaxSize);
		//throw Except;
		return *this;
	}

	realSize = sizeof(iValue);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&iValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator<<(double dValue)
{
	int realSize = GetFreeSize() - sizeof(dValue);
	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realSize < 0)
	{
		IncreaseBufferSize(eBUFFER_UPSCALE_INT64);
		realSize = sizeof(dValue);

		char* ptr = m_cpBuffer;
		ptr += m_iRear;
		memcpy(ptr, (char*)&dValue, realSize);
		m_iUsingSize += realSize;
		m_iRear += realSize;

		// TODO: ���⿡ �޸𸮸� �ٽ� �ø��� ���� ��ü throw
		CExceptClass* Except = new CExceptClass(L"<< DOUBLE ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(double);

	char* ptr = m_cpBuffer;
	ptr += m_iRear;
	memcpy(ptr, (char*)&dValue, realSize);
	m_iUsingSize += realSize;
	m_iRear += realSize;

	return *this;
}

CMessage& CMessage::operator>>(BYTE& byValue)
{
	int realSize = GetDataSize() - sizeof(byValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> BYTE ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(byValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&byValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(char& chValue)
{
	int realSize = GetDataSize() - sizeof(chValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> CHAR ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(chValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&chValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(short& shValue)
{
	int realSize = GetDataSize() - sizeof(shValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> SHORT ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(shValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&shValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(WORD& wValue)
{
	int realSize = GetDataSize() - sizeof(wValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> WORD ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(wValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&wValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}



CMessage& CMessage::operator>>(int& iValue)
{
	int realSize = GetDataSize() - sizeof(iValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> INT ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(iValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&iValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(DWORD& dwValue)
{
	int realSize = GetDataSize() - sizeof(dwValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> DWORD ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(dwValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&dwValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(float& fValue)
{
	int realSize = GetDataSize() - sizeof(fValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> FLOAT ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(fValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&fValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(__int64& iValue)
{
	int realSize = GetDataSize() - sizeof(iValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> INT64 ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(iValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&iValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(double& dValue)
{
	int realSize = GetDataSize() - sizeof(dValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> DOUBLE ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(dValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&dValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}

CMessage& CMessage::operator>>(UINT64& dValue)
{
	int realSize = GetDataSize() - sizeof(dValue);
	if (realSize < 0)
	{
		CExceptClass* Except = new CExceptClass(L">> DOUBLE ERROR", m_cpBuffer, m_iMaxSize);
		throw Except;
	}

	realSize = sizeof(dValue);
	char* ptr = m_cpBuffer;
	ptr += m_iFront;

	memcpy(&dValue, ptr, realSize);
	m_iUsingSize -= realSize;
	m_iFront += realSize;

	return *this;
}
