#pragma once
#include "stdafx.h"


#define BUFSIZE 10000


class CRingBuffer
{
private:
	int _iBufferSize;
	int _iFront;
	int _iRear;
	int _iUsingSize;
	char* _cBufferArray;

public:
	CRingBuffer();
	
	~CRingBuffer(void);

	// ������� �뷮 ��ȯ
	int GetUsingSize(void);

	// ���� �뷮 ��ȯ
	int GetFreeSize(void);

	// ���ۿ� ������ �Է�
	int Enqueue(char* data, int size);

	// ���ۿ� ������ ����
	int Dequeue(char* dest, int size);

	/////////////////////////////////////////////////////////////////////////
	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
	// (������ ���� ����)
	//
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	int DirectEnqueueSize(void);

	int DirectDequeueSize(void);
	
	// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	int Peek(char* chpDest, int iSize);

	// ������ ��� ����Ÿ ����.
	void ClearBuffer(void);

	// ������ Front ������ ����.
	char* GetFrontBufferPtr(void);

	// ������ RearPos ������ ����.
	char* GetRearBufferPtr(void);


	// Front�� �̵�
	void MoveFront(int size);

	// Rear�� �̵�
	void MoveRear(int size);
};