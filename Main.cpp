#include "stdafx.h"
#include "CExceptClass.h"
#include "CMessage.h"
#include "CRingBuffer.h"
#include "Protocol.h"

typedef struct st_SESSION
{
	SOCKET socket;
	SOCKADDR_IN clientaddr;
	DWORD ID;
	CRingBuffer RecvQ;
	CRingBuffer SendQ;
	DWORD dwRecvTime;
	DWORD dwRecvCount;
	DWORD dwRecvSecondTick;
	st_SESSION(void)
	{
		socket = INVALID_SOCKET;
		ZeroMemory(&clientaddr, sizeof(clientaddr));
		RecvQ.ClearBuffer();
		SendQ.ClearBuffer();
		dwRecvTime = 0;
		dwRecvCount = 0;
		dwRecvSecondTick = 0;
	}
	~st_SESSION(void) {}
} SESSION;

typedef struct st_CHARACTER
{
	st_SESSION* pSession;
	DWORD dwSessionID;

	DWORD dwAction;
	BYTE byDirection;
	BYTE byMoveDirection;

	short shX;
	short shY;
	short shActionX;
	short shActionY;

	char chHP;
}CLIENT;

//-----------------------��Ŷ ���� �Լ�-----------------------
bool NetWork_ResCreateMyCharacter(SESSION* pSession, CMessage* pPacket);


//-----------------------������ ����-----------------------
// ���� ID
DWORD g_dwSessionID = 1;
// ������ �õ��� ������ ������ Vector
std::vector<SOCKET> g_SocketVector;
// ������ ������ Map
std::map<SOCKET, SESSION*> g_SessionMap;
// Ŭ���̾�Ʈ�� ������ Unordered_Map
std::map<DWORD, CLIENT*> g_ClientMap;
// select�Լ��� ���� ��� ���
timeval selecttime = { 0, 0 };
// �Է¹��� IP�� ������ ����
WCHAR szIPInput[32] = { 0 };
// �Է¹��� ��Ʈ��ȣ�� ������ ����
short shPort = 0;
// �Է¹��� Ŭ���̾�Ʈ ���� ������ ����
int iClientCount = 0;
// ���� ����ü ����
SOCKADDR_IN g_ServerAddr;

//-----------------------������ �Լ�-----------------------
// ������ �����ϴ� �Լ�
void CreateSocket(void);
// ������ ���Ͽ� ���� ������ �õ��ϴ� �Լ�
void ConnectToServer(const SOCKADDR* name, int len);
// connect()�� �õ��� ���Ͽ� ���� select�� ����� �˾Ƴ��� �Լ�
void SelectConnect(SOCKET* pTableSocket, FD_SET* pWirteSet, FD_SET* pErrorSet);
// ������ �����ϴ� �Լ�
SESSION* CreateSession(SOCKET sock);
// Ŭ���̾�Ʈ�� �����ϴ� �Լ�
void CreateClient(CMessage* pPacket);
// ��Ʈ��ũ ���ν���
void NetWorkProc(void);
// Set�� ������ִ� socket�� ������� Select()�Լ��� ȣ���ϴ� �Լ�
void SelectSocket(SOCKET* pTableSocket, FD_SET* pReadSet, FD_SET* pWriteSet);
// Ư�� ���Ͽ� ���ؼ� recv()�� �ϴ� ��
void NetWork_Recv(SOCKET clientSock);
// Ư�� ���Ͽ� ���ؼ� send()�� �ϴ� �Լ�
BOOL NetWork_Send(SOCKET clinetSock);
// ��Ŷ�� ����� �Լ�
int CompletePacket(SESSION* session);
// ���� ��Ŷ�� ó���ϴ� �Լ�
bool PacketProc(SESSION* session, WORD dwType, CMessage* message);
//-----------------------�α׿� ����-----------------------
int iConnectSuccessCount = 0;
int iConnectFailCount = 0;

//-----------------------�α׿� �Լ�-----------------------
// �޸𸮸� �����ϴ� �Լ�
void DeleteMemory();

//-----------------------�ڷᱸ���� ����-----------------------
void InsertSession(SOCKET sock, SESSION* pSession);
void InsertClient(DWORD dwClientID, CLIENT* pClient);
SESSION* FindSession(SOCKET sock);

int main()
{
	int retval = 0;
	setlocale(LC_ALL, "korean");
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		wprintf(L"WSAStartup() eror\n");
		return 1;
	}

	ZeroMemory(&g_ServerAddr, sizeof(SOCKADDR_IN));
	wprintf(L"�����ϰ��� �ϴ� IP�ּҸ� �Է��ϼ��� : ");
	wscanf_s(L"%s", szIPInput, 32);
	wprintf(L"���ϴ� ��Ʈ ��ȣ�� �Է��ϼ��� : ");
	wscanf_s(L"%hd", &shPort);
	wprintf(L"�����ϰ��� �ϴ� Ŭ���̾�Ʈ ���� �Է��ϼ��� : ");
	wscanf_s(L"%d", &iClientCount);
	g_ServerAddr.sin_family = AF_INET;
	InetPton(AF_INET, szIPInput, &g_ServerAddr.sin_addr);
	g_ServerAddr.sin_port = htons(shPort);

	// ���� ����
	CreateSocket();

	// ���� �õ�
	ConnectToServer((SOCKADDR*)&g_ServerAddr, sizeof(g_ServerAddr));

	wprintf(L"Connection Sucess : %d, Connection Fail : %d\n", iConnectSuccessCount, iConnectFailCount);
	NetWorkProc();
	wprintf(L"Please wait for 5 seconds\n");


	Sleep(5000);

	//while (true)
	//{
	//	//NetWorkProc();
	//}

	DeleteMemory();

	// ���� �õ�

	WSACleanup();
	return 0;
}

bool NetWork_ResCreateMyCharacter(SESSION* pSession, CMessage* pPacket)
{
	CreateClient(pPacket);
	return true;
}

void CreateSocket(void)
{
	int retval = 0;
	for (int i = 0; i < iClientCount; i++)
	{
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			wprintf(L"socket() eror\n");
			return;
		}

		// �ͺ�� �������� ��ȯ
		u_long on = 1;
		retval = ioctlsocket(sock, FIONBIO, &on);
		if (retval == SOCKET_ERROR)
		{
			wprintf(L"ioctlsocket() eror\n");
			return;
		}

		// ���̱� �˰��� ����
		BOOL optval = TRUE;
		setsockopt(sock, IPPROTO_IP, TCP_NODELAY, (char*)&optval, sizeof(optval));

		g_SocketVector.push_back(sock);
	}
}

void ConnectToServer(const SOCKADDR* name, int len)
{
	FD_SET writeSet, errorSet;
	SOCKET UserTable_Socket[FD_SETSIZE];
	int addrlen, retval;
	int iSessionCount = 0;

	FD_ZERO(&errorSet);
	FD_ZERO(&writeSet);
	memset(UserTable_Socket, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

	for (int i = 0; i < g_SocketVector.size(); i++)
	{
		retval = connect(g_SocketVector[i], name, len);
		//if (retval == 0)
		//{
		//	// Read Set�� Write Set�� ���
			UserTable_Socket[iSessionCount] = g_SocketVector[i];
			FD_SET(g_SocketVector[i], &writeSet);
			iSessionCount++;
		//}
		//else
		//{
			if (retval == SOCKET_ERROR)
			{
				DWORD error = WSAGetLastError();

				if (error == WSAEWOULDBLOCK)
					continue;
				else
				{
					iConnectFailCount++;
					wprintf(L"Connect() error socket : %d, Error Code : %d\n", g_SocketVector[i], error);
				}
			}
		//}
		if (iSessionCount >= FD_SETSIZE)
		{
			SelectConnect(UserTable_Socket, &writeSet, &errorSet);

			FD_ZERO(&errorSet);
			FD_ZERO(&writeSet);
			memset(UserTable_Socket, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
			iSessionCount = 0;
		}
	}

	if (iSessionCount >= 0)
	{
		SelectConnect(UserTable_Socket, &writeSet, &errorSet);

		FD_ZERO(&errorSet);
		FD_ZERO(&writeSet);
		memset(UserTable_Socket, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
		iSessionCount = 0;
	}
}

void SelectConnect(SOCKET* pTableSocket, FD_SET* pWriteSet, FD_SET* pErrorSet)
{
	int iResult = 0;
	int iCnt = 0;
	BOOL bProcFlag;

	iResult = select(0, 0, pWriteSet, pErrorSet, &selecttime);
	if (iResult > 0)
	{
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (pTableSocket[i] == INVALID_SOCKET)
				continue;
			
			if (FD_ISSET(pTableSocket[i], pWriteSet))
			{
				// ���� ����, ���ǰ� Ŭ���̾�Ʈ ����
				CreateSession(pTableSocket[i]);
				iConnectSuccessCount++;
				continue;
			}

			if (FD_ISSET(pTableSocket[i], pErrorSet))
			{
				// ���� ����
				DWORD error = WSAGetLastError();
				wprintf(L"Connect() error Code : %d\n", error);
				iConnectFailCount++;
			}
		}
	}
	else if (iResult == SOCKET_ERROR)
	{
		wprintf(L"Select() Error!\n");
	}
}


SESSION* CreateSession(SOCKET sock)
{
	SESSION* newSession = new SESSION();
	newSession->clientaddr = g_ServerAddr;
	newSession->socket = sock;
	newSession->ID = g_dwSessionID++;

	InsertSession(sock, newSession);
	//wprintf(L"Session %d connected\n", newSession->ID);

	return newSession;
}

void CreateClient(CMessage* pPacket)
{
	DWORD id;
	BYTE dir;
	WORD x;
	WORD y;
	BYTE hp;

	(*pPacket) >> id;
	(*pPacket) >> dir;
	(*pPacket) >> x;
	(*pPacket) >> y;
	(*pPacket) >> hp;

	CLIENT* newClient = new CLIENT();

	newClient->dwSessionID = id;
	newClient->byDirection = dir;
	newClient->shX = x;
	newClient->shY = y;
	newClient->chHP = hp;

	InsertClient(newClient->dwSessionID, newClient);

	wprintf(L"Client %d connected\n", newClient->dwSessionID);

}


void NetWorkProc(void)
{
	SESSION* pSession = nullptr;
	FD_SET readSet, writeSet;
	SOCKET UserTable_Socket[FD_SETSIZE];
	int addrlen, retval;
	int iSessionCount = 0;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	memset(UserTable_Socket, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

	std::map<SOCKET, SESSION*>::iterator SessionIter = g_SessionMap.begin();
	for (SessionIter = g_SessionMap.begin(); SessionIter != g_SessionMap.end();)
	{
		pSession = (SessionIter)->second;
		SessionIter++;
		if (pSession->socket == INVALID_SOCKET)
		{
			// TODO
			//DisconnectSession(pSession->socket);
			continue;
		}
		// Read Set�� Write Set�� ���
		UserTable_Socket[iSessionCount] = pSession->socket;
		FD_SET(pSession->socket, &readSet);
		if (pSession->SendQ.GetUsingSize() > 0)
		{
			FD_SET(pSession->socket, &writeSet);
		}

		iSessionCount++;

		if (iSessionCount >= FD_SETSIZE)
		{
			SelectSocket(UserTable_Socket, &readSet, &writeSet);

			FD_ZERO(&readSet);
			FD_ZERO(&writeSet);
			memset(UserTable_Socket, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
			iSessionCount = 0;
		}
	}

	if (iSessionCount > 0)
	{
		SelectSocket(UserTable_Socket, &readSet, &writeSet);
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		memset(UserTable_Socket, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
		iSessionCount = 0;
	}
}

void SelectSocket(SOCKET* pTableSocket, FD_SET* pReadSet, FD_SET* pWriteSet)
{
	int iResult = 0;
	int iCnt = 0;
	BOOL bProcFlag;

	iResult = select(0, pReadSet, pWriteSet, 0, &selecttime);

	if (iResult > 0)
	{
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			bProcFlag = TRUE;
			if (pTableSocket[i] == INVALID_SOCKET)
				continue;

			// FD_WRITE üũ
			if (FD_ISSET(pTableSocket[i], pWriteSet))
			{
				--iResult;
				bProcFlag = NetWork_Send(pTableSocket[i]);
			}

			if (FD_ISSET(pTableSocket[i], pReadSet))
			{

				NetWork_Recv(pTableSocket[i]);

			}
		}
	}
	else if (iResult == SOCKET_ERROR)
	{
		//PrintError(L"Select() Error!");
	}
}

void NetWork_Recv(SOCKET clientSock)
{
	SESSION* pSession;
	int iBuffSize;
	int iResult;

	pSession = FindSession(clientSock);
	if (pSession == nullptr)
	{
		wprintf(L"Recv() session is nullptr\n");
	}

	// ���������� ���� �޽��� Ÿ��
	pSession->dwRecvTime = timeGetTime();

	// �ޱ� �۾�
	iBuffSize = pSession->RecvQ.DirectEnqueueSize();
	iResult = recv(pSession->socket, pSession->RecvQ.GetRearBufferPtr(), iBuffSize, 0);
	if (SOCKET_ERROR == iResult)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			DWORD temp = WSAGetLastError();
			WCHAR error[64] = { 0 };
			swprintf_s(error, 64, L"Socket Error UserID : %d\n", temp);
			//PrintError(error);
			// TODO
			//DisconnectSession(clientSock);
			return;
		}
	}
	else if (iResult == 0)
	{
		// TODO
		//DisconnectSession(clientSock);
		return;
	}


	if (iResult > 0)
	{
		pSession->RecvQ.MoveRear(iResult);

		while (true)
		{
			iResult = CompletePacket(pSession);
			if (iResult == 1)
				break;
			// ��Ŷ ó�� ����
			if (iResult == -1)
			{
				WCHAR error[64] = { 0 };
				wprintf(L"Packet Error UserID : %d\n", pSession->ID);
				//PrintError(error);
				return;
			}
		}
	}
}

BOOL NetWork_Send(SOCKET clinetSock)
{
	SESSION* pSession = nullptr;
	pSession = FindSession(clinetSock);
	int iResult = 0;
	int iSendBufferUsingSize = 0;
	int iDequeueSize = 0;
	if (pSession->socket == INVALID_SOCKET)
		return FALSE;
	while (true)
	{
		if (pSession->SendQ.GetUsingSize() <= 0)
			break;

		if (pSession == nullptr)
		{
			//PrintError(L"NetWork_Send() Error!");
			closesocket(clinetSock);
			return FALSE;
		}

		iSendBufferUsingSize = pSession->SendQ.DirectDequeueSize();

		if (iSendBufferUsingSize <= 0)
			return FALSE;

		iResult = send(pSession->socket, pSession->SendQ.GetFrontBufferPtr(), iSendBufferUsingSize, 0);


		if (iResult == SOCKET_ERROR || iResult == 0)
		{
			DWORD error = WSAGetLastError();
			if (WSAEWOULDBLOCK == error)
			{
				wprintf(L"Socket WOULDBLOCK - UerID : %d\n", pSession->ID);
				return TRUE;
			}
			wprintf(L"Socket Error - Error : %d, UserID : %d\n", error, pSession->ID);
			// TODO
			//DisconnectSession(clinetSock);
			return FALSE;
		}
		pSession->SendQ.MoveFront(iResult);

	}

	return TRUE;
}

int CompletePacket(SESSION* session)
{
	st_PACKET_HEADER stPacketHeader;
	int iRecvQSize = session->RecvQ.GetUsingSize();
	BYTE byEndCode;

	// ���� ������ �˻��ؾ��Ѵ�. ��Ŷ��� ũ�� �̻��� ��쿡��!
	if (iRecvQSize < sizeof(stPacketHeader))
		return 1;

	// ��Ŷ �˻�
	session->RecvQ.Peek((char*)&stPacketHeader, sizeof(st_PACKET_HEADER));

	// ��Ŷ �ڵ� �˻�
	if (stPacketHeader.byCode != dfPACKET_CODE)
	{
		wprintf(L"PacketCode Error! Session ID : %d, PacketData : %d\n", stPacketHeader.byCode);
		return -1;
	}

	// ť�� ����� �����Ͱ� ��Ŷ�� ũ�⸸ŭ �ִ��� Ȯ��. EndCode�� �����Ƿ� + 1�� ���
	if (stPacketHeader.bySize + sizeof(st_PACKET_HEADER) + 1 > iRecvQSize)
		return 1;

	session->RecvQ.MoveFront(sizeof(st_PACKET_HEADER));

	CMessage Packet;
	if (stPacketHeader.bySize != session->RecvQ.Peek(Packet.GetBufferPtr(), stPacketHeader.bySize))
	{
		//PrintError((WCHAR*)Packet.GetBufferPtr());
		return -1;
	}

	session->RecvQ.MoveFront(stPacketHeader.bySize);

	// EndCode �κ� ���۷� ����
	if (1 != session->RecvQ.Dequeue((char*)&byEndCode, 1))
		return -1;
	if (dfNETWORK_PACKET_END != byEndCode)
		return -1;

	Packet.MoveWritePos(stPacketHeader.bySize);

	if (!PacketProc(session, stPacketHeader.byType, &Packet))
	{
		wprintf(L"PacketProc Error!\n");
		return -1;
	}
	return 0;
}

bool PacketProc(SESSION* session, WORD dwType, CMessage* message)
{
	switch (dwType)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
		return NetWork_ResCreateMyCharacter(session, message);
		break;
	case dfPACKET_CS_MOVE_START:
		//return NetWork_ReqMoveStart(session, message);
		break;
	case dfPACKET_CS_MOVE_STOP:
		//return NetWork_ReqMoveStop(session, message);
		break;
	case dfPACKET_CS_ATTACK1:
		//return NetWork_ReqAttack1(session, message);
	case dfPACKET_CS_ATTACK2:
		//return NetWork_ReqAttack2(session, message);
	case dfPACKET_CS_ATTACK3:
		//return NetWork_ReqAttack3(session, message);
	case dfPACKET_CS_ECHO:
		//return NetWork_ReqEcho(session, message);
	default:
		wprintf(L"Packet type Error : %d\n", dwType);
		break;
	}
	return false;
}

void DeleteMemory()
{
	std::map<SOCKET, SESSION*>::iterator itor = g_SessionMap.begin();
	std::map<DWORD, CLIENT*>::iterator itor2 = g_ClientMap.begin();

	for (; itor != g_SessionMap.end();)
	{
		wprintf(L"Session %d deleted\n", itor->second->ID);
		closesocket(itor->first);
		delete itor->second;
		itor = g_SessionMap.erase(itor);
	}

	for (; itor2 != g_ClientMap.end();)
	{
		wprintf(L"Client %d deleted\n", itor2->first);
		delete itor2->second;
		itor2 = g_ClientMap.erase(itor2);
	}
}

void InsertSession(SOCKET sock, SESSION* pSession)
{
	g_SessionMap.insert(std::make_pair(sock, pSession));
}

void InsertClient(DWORD dwClientID, CLIENT* pClient)
{
	g_ClientMap.insert(std::make_pair(dwClientID, pClient));
}

SESSION* FindSession(SOCKET sock)
{
	return g_SessionMap.find(sock)->second;
}

