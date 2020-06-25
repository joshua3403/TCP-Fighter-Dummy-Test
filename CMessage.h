#pragma once
#include "stdafx.h"
#include "CExceptClass.h"

class CMessage
{
	/*---------------------------------------------------------------
	Packet Enum.
	
	----------------------------------------------------------------*/
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1000,		// ��Ŷ�� �⺻ ���� ������.
		eBUFFER_UPSCALE_BYTE = 1,
		eBUFFER_UPSCALE_SHORT = 2,
		eBUFFER_UPSCALE_INT = 4,
		eBUFFER_UPSCALE_INT64 = 8

	};

private:
	int m_iMaxSize;
	int m_iFront;
	int m_iRear;
	int m_iUsingSize;
	char* m_cpBuffer = nullptr;

public:
	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CMessage()
	{
		m_iMaxSize = en_PACKET::eBUFFER_DEFAULT;
		m_iFront = m_iRear = m_iUsingSize = 0;
		m_cpBuffer = (char*)malloc(sizeof(char) * m_iMaxSize);
	}

	CMessage(int iBufferSize)
	{
		m_iMaxSize = iBufferSize;
		m_iFront = m_iRear = m_iUsingSize = 0;
		m_cpBuffer = (char*)malloc(sizeof(char) * m_iMaxSize);
	}

	virtual	~CMessage()
	{
		Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ  �ı�.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void)
	{
		if (m_cpBuffer != nullptr)
		{
			free(m_cpBuffer);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ û��.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Clear(void)
	{
		m_iFront = m_iRear = m_iUsingSize = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)��Ŷ ���� ������ ���.
	//////////////////////////////////////////////////////////////////////////
	int	GetBufferSize(void) { return m_iMaxSize; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)������� ����Ÿ ������.
	//////////////////////////////////////////////////////////////////////////
	int	GetDataSize(void) { return m_iUsingSize; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ��� ������ ������ ���.
	//
	// Parameters: ����.
	// Return: (int) ��� ������ ������ ������
	//////////////////////////////////////////////////////////////////////////
	int GetFreeSize(void) { return m_iMaxSize - m_iUsingSize; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char* GetBufferPtr(void) { return m_cpBuffer; }


	//////////////////////////////////////////////////////////////////////////
	// Front��ġ ���
	//
	// Parameters: ����.
	// Return: (int) ���� Front�� ��ġ
	//////////////////////////////////////////////////////////////////////////
	int GetFront(void) { return m_iFront; }

	//////////////////////////////////////////////////////////////////////////
	// Rear��ġ ���
	//
	// Parameters: ����.
	// Return: (int) ���� Rear�� ��ġ
	//////////////////////////////////////////////////////////////////////////
	int GetRear(void) { return m_iRear; }

	void PutData(char* data, int size);

	void GetData(char* data, int size);


	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize)
	{
		int temp = m_iRear + iSize;
		if (temp < m_iMaxSize)
		{
			m_iRear += iSize;
			temp = iSize;
			m_iUsingSize += iSize;
		}
		else if (temp == m_iMaxSize)
		{
			m_iRear = m_iMaxSize;
			temp = iSize;
			m_iUsingSize = m_iMaxSize;
		}
		else
		{
			temp = temp - m_iMaxSize;
			m_iRear = m_iMaxSize;
			m_iUsingSize = m_iMaxSize;
		}

		return temp;
	}

	int		MoveReadPos(int iSize)
	{
		int temp = m_iFront + iSize;
		if (temp <= m_iRear)
		{
			m_iFront += iSize;
			m_iUsingSize -= iSize;
			temp = iSize;
		}
		else if (temp > m_iRear)
		{
			temp = m_iRear - m_iFront;
			m_iUsingSize -= temp;
		}
		return temp;
	}

	// ���� ����� ������ �ʿ��� ��ŭ Ű��� �Լ�
	void IncreaseBufferSize(int size);

	/* ============================================================================= */
	// ������ �����ε�
	/* ============================================================================= */
	CMessage& operator = (CMessage& clSrCMessage);

	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	CMessage& operator << (BYTE byValue);
	CMessage& operator << (char chValue);

	CMessage& operator << (short shValue);
	CMessage& operator << (WORD wValue);

	CMessage& operator << (int iValue);
	CMessage& operator << (DWORD dwValue);
	CMessage& operator << (float fValue);

	CMessage& operator << (__int64 iValue);
	CMessage& operator << (double dValue);

	//////////////////////////////////////////////////////////////////////////
// ����.	�� ���� Ÿ�Ը��� ��� ����.
//////////////////////////////////////////////////////////////////////////
	CMessage& operator >> (BYTE& byValue);
	CMessage& operator >> (char& chValue);

	CMessage& operator >> (short& shValue);
	CMessage& operator >> (WORD& wValue);


	CMessage& operator >> (int& iValue);
	CMessage& operator >> (DWORD& dwValue);
	CMessage& operator >> (float& fValue);

	CMessage& operator >> (__int64& iValue);
	CMessage& operator >> (double& dValue);
	CMessage& operator >> (UINT64& dValue);
};
