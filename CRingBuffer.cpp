#include "stdafx.h"
#include "CRingBuffer.h"


CRingBuffer::CRingBuffer()
{
	_iBufferSize = BUFSIZE;
	_iFront = _iRear = _iUsingSize = 0;
	_cBufferArray = (char*)malloc(BUFSIZE);
	if (_cBufferArray == nullptr)
	{
		wprintf(L"malloc error\n");
	}
}

CRingBuffer::~CRingBuffer(void)
{
	free(_cBufferArray);
}

int CRingBuffer::GetUsingSize(void)
{
	return _iUsingSize;
}

int CRingBuffer::GetFreeSize(void)
{
	return _iBufferSize - _iUsingSize;
}

int CRingBuffer::Enqueue(char* data, int size)
{
	// ���� �������� 0����
	if (GetFreeSize() == 0)
		return 0;
	// ���� ���� ������ ũ�� Ȯ��
	int realsize = GetFreeSize() - size;

	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realsize < 0)
	{
		realsize = GetFreeSize();
	}
	// ���� ������ �����ϰ��� �ϴ� ũ�⺸�� �� ũ�� �����ϰ��� �ϴ� ũ�⸸ŭ ����
	else
	{
		realsize = size;
	}

	// Rear�� ��ġ�� �迭�� �������� ���� Ȯ��
	int rearTomax = _iBufferSize - _iRear;
	char* ptr = _cBufferArray;
	ptr += _iRear;

	// �迭 ������ ���� ���̰� �����ϰ��� �ϴ� ũ�⺸�� ��ų� ������
	if (rearTomax > realsize)
	{
		memcpy(ptr, data, realsize);
		_iRear += realsize;
	}
	else if (rearTomax == realsize)
	{
		memcpy(ptr, data, rearTomax);
		_iRear += realsize;
	}
	else
	{
		memcpy(ptr, data, rearTomax);
		data += rearTomax;
		size -= rearTomax;
		memcpy(_cBufferArray, data, size);
		_iRear = 0;
		_iRear += realsize - rearTomax;
	}

	_iUsingSize += realsize;

	if (_iRear == _iBufferSize)
		_iRear = 0;

	return realsize;
}

int CRingBuffer::Dequeue(char* dest, int size)
{
	// ���� �������� 0����
	if (GetUsingSize() == 0)
		return 0;
	// ���� ���� ������ ũ�� Ȯ��
	int realsize = GetUsingSize() - size;

	// ������ִ� ������ �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realsize < 0)
	{
		realsize = GetUsingSize();
	}
	// ���� ������ �����ϰ��� �ϴ� ũ�⺸�� �� ũ�� �����ϰ��� �ϴ� ũ�⸸ŭ ����
	else
	{
		realsize = size;
	}

	// Front�� ��ġ�� �迭�� �������� ���� Ȯ��
	int frontTomax = _iBufferSize - _iFront;
	char* ptr = _cBufferArray;
	ptr += _iFront;

	if (frontTomax > realsize)
	{
		memcpy(dest, ptr, realsize);
		_iFront += realsize;
	}
	else if (frontTomax == realsize)
	{
		memcpy(dest, ptr, realsize);
		_iFront += realsize;
	}
	else
	{
		memcpy(dest, ptr, frontTomax);
		dest += frontTomax;
		size -= frontTomax;
		memcpy(dest, _cBufferArray, size);
		_iFront = 0;
		_iFront += realsize - frontTomax;
	}

	_iUsingSize -= realsize;

	if (_iFront == _iBufferSize)
		_iFront = 0;

	return realsize;
}

int CRingBuffer::DirectEnqueueSize(void)
{
	if (_iRear >= _iFront)
		return _iBufferSize - _iRear;
	else
		return _iFront - _iRear;
}

int CRingBuffer::DirectDequeueSize(void)
{
	if (_iRear >= _iFront)
	{
		return _iRear - _iFront;
	}
	else
	{
		return _iBufferSize - _iFront;
	}
}

int CRingBuffer::Peek(char* dest, int size)
{
	// ���� �������� 0����
	if (GetUsingSize() == 0)
		return 0;
	// ���� ���� ������ ũ�� Ȯ��
	int realsize = GetUsingSize() - size;

	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realsize < 0)
	{
		realsize = GetUsingSize();
	}
	// ���� ������ �����ϰ��� �ϴ� ũ�⺸�� �� ũ�� �����ϰ��� �ϴ� ũ�⸸ŭ ����
	else
	{
		realsize = size;
	}

	// Front�� ��ġ�� �迭�� �������� ���� Ȯ��
	int frontTomax = _iBufferSize - _iFront;
	char* ptr = _cBufferArray;
	ptr += _iFront;

	if (frontTomax > realsize)
	{
		memcpy(dest, ptr, realsize);
	}
	else if (frontTomax == realsize)
	{
		memcpy(dest, ptr, realsize);
	}
	else
	{
		memcpy(dest, ptr, frontTomax);
		dest += frontTomax;
		size -= frontTomax;
		memcpy(dest, _cBufferArray, size);
	}


	return realsize;
}

void CRingBuffer::ClearBuffer(void)
{
	_iUsingSize = _iRear = _iFront = 0;
}

char* CRingBuffer::GetFrontBufferPtr(void)
{
	char* ptr = _cBufferArray;
	ptr += _iFront;

	return ptr;
}

char* CRingBuffer::GetRearBufferPtr(void)
{
	char* ptr = _cBufferArray;
	ptr += _iRear;

	return ptr;
}

void CRingBuffer::MoveFront(int size)
{
	// ���� �������� 0����
	if (GetUsingSize() == 0)
		return ;
	// ���� ���� ������ ũ�� Ȯ��
	int realsize = GetUsingSize() - size;

	// ���� �������� �����ϰ��� �ϴ� ũ�Ⱑ �� ũ�� �����ִ� ũ�⸸ŭ ����
	if (realsize < 0)
	{
		realsize = GetUsingSize();
	}
	// ���� ������ �����ϰ��� �ϴ� ũ�⺸�� �� ũ�� �����ϰ��� �ϴ� ũ�⸸ŭ ����
	else
	{
		realsize = size;
	}
	// Front�� ��ġ�� �迭�� �������� ���� Ȯ��
	int frontTomax = _iBufferSize - _iFront;

	if (frontTomax > realsize)
	{
		_iFront += size;
	}
	else if (frontTomax == realsize)
	{
		_iFront = 0;
	}
	else
	{
		int temp = size;
		temp = temp - frontTomax;
		_iFront = 0;
		_iFront += temp;
	}

	_iUsingSize -= size;
	//
	//if (_iUsingSize <= 0)
	//{
	//	ClearBuffer();
	//}
}

void CRingBuffer::MoveRear(int size)
{
	// Rear�� ��ġ�� �迭�� �������� ���� Ȯ��
	int rearTomax = _iBufferSize - _iRear;
	if (rearTomax > size)
	{
		_iRear += size;
	}
	else if (rearTomax == size)
	{
		_iRear = 0;
	}
	else
	{
		int temp = size;
		temp = temp - rearTomax;
		_iRear = 0;
		_iRear += temp;
	}
	
	_iUsingSize += size;

	//if (_iUsingSize >= _iBufferSize)
	//{
	//	_iUsingSize = _iBufferSize;
	//}
}
