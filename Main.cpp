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

//-----------------------패킷 대응 함수-----------------------
bool NetWork_ResCreateMyCharacter(SESSION* pSession, CMessage* pPacket);


//-----------------------서버용 변수-----------------------
// 세션 ID
DWORD g_dwSessionID = 1;
// 접속을 시도할 소켓을 관리할 Vector
std::vector<SOCKET> g_SocketVector;
// 세션을 관리할 Map
std::map<SOCKET, SESSION*> g_SessionMap;
// 클라이언트를 관리할 Unordered_Map
std::map<DWORD, CLIENT*> g_ClientMap;
// select함수의 리턴 대시 기산
timeval selecttime = { 0, 0 };
// 입력받은 IP를 저장할 변수
WCHAR szIPInput[32] = { 0 };
// 입력받은 포트번호를 저장할 변수
short shPort = 0;
// 입력받은 클라이언트 수를 저장할 변수
int iClientCount = 0;
// 소켓 구조체 변수
SOCKADDR_IN g_ServerAddr;

//-----------------------서버용 함수-----------------------
// 소켓을 생성하는 함수
void CreateSocket(void);
// 생성한 소켓에 대해 접속을 시도하는 함수
void ConnectToServer(const SOCKADDR* name, int len);
// connect()를 시도한 소켓에 대해 select로 결과를 알아내는 함수
void SelectConnect(SOCKET* pTableSocket, FD_SET* pWirteSet, FD_SET* pErrorSet);
// 세션을 생성하는 함수
SESSION* CreateSession(SOCKET sock);
// 클라이언트를 생성하는 함수
void CreateClient(CMessage* pPacket);
// 네트워크 프로시져
void NetWorkProc(void);
// Set에 저장되있는 socket을 대상으로 Select()함수를 호출하는 함수
void SelectSocket(SOCKET* pTableSocket, FD_SET* pReadSet, FD_SET* pWriteSet);
// 특정 소켓에 대해서 recv()를 하는 함
void NetWork_Recv(SOCKET clientSock);
// 특정 소켓에 대해서 send()를 하는 함수
BOOL NetWork_Send(SOCKET clinetSock);
// 패킷을 만드는 함수
int CompletePacket(SESSION* session);
// 실제 패킷을 처리하는 함수
bool PacketProc(SESSION* session, WORD dwType, CMessage* message);
//-----------------------로그용 변수-----------------------
int iConnectSuccessCount = 0;
int iConnectFailCount = 0;

//-----------------------로그용 함수-----------------------
// 메모리를 해제하는 함수
void DeleteMemory();

//-----------------------자료구조용 변수-----------------------
void InsertSession(SOCKET sock, SESSION* pSession);
void InsertClient(DWORD dwClientID, CLIENT* pClient);
SESSION* FindSession(SOCKET sock);

int main()
{
	int retval = 0;
	setlocale(LC_ALL, "korean");
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		wprintf(L"WSAStartup() eror\n");
		return 1;
	}

	ZeroMemory(&g_ServerAddr, sizeof(SOCKADDR_IN));
	wprintf(L"접속하고자 하는 IP주소를 입력하세요 : ");
	wscanf_s(L"%s", szIPInput, 32);
	wprintf(L"원하는 포트 번호를 입력하세요 : ");
	wscanf_s(L"%hd", &shPort);
	wprintf(L"접속하고자 하는 클라이언트 수를 입력하세요 : ");
	wscanf_s(L"%d", &iClientCount);
	g_ServerAddr.sin_family = AF_INET;
	InetPton(AF_INET, szIPInput, &g_ServerAddr.sin_addr);
	g_ServerAddr.sin_port = htons(shPort);

	// 소켓 생성
	CreateSocket();

	// 접속 시도
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

	// 접속 시도

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

		// 넌블록 소켓으로 전환
		u_long on = 1;
		retval = ioctlsocket(sock, FIONBIO, &on);
		if (retval == SOCKET_ERROR)
		{
			wprintf(L"ioctlsocket() eror\n");
			return;
		}

		// 네이글 알고리즘 해제
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
		//	// Read Set과 Write Set에 등록
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
				// 접속 성공, 세션과 클라이언트 생성
				CreateSession(pTableSocket[i]);
				iConnectSuccessCount++;
				continue;
			}

			if (FD_ISSET(pTableSocket[i], pErrorSet))
			{
				// 접속 실패
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
		// Read Set과 Write Set에 등록
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

			// FD_WRITE 체크
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

	// 마지막으로 받은 메시지 타임
	pSession->dwRecvTime = timeGetTime();

	// 받기 작업
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
			// 패킷 처리 오류
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

	// 받은 내용을 검사해야한다. 패킷헤더 크기 이상인 경우에만!
	if (iRecvQSize < sizeof(stPacketHeader))
		return 1;

	// 패킷 검사
	session->RecvQ.Peek((char*)&stPacketHeader, sizeof(st_PACKET_HEADER));

	// 패킷 코드 검사
	if (stPacketHeader.byCode != dfPACKET_CODE)
	{
		wprintf(L"PacketCode Error! Session ID : %d, PacketData : %d\n", stPacketHeader.byCode);
		return -1;
	}

	// 큐에 저장된 데이터가 패킷의 크기만큼 있는지 확인. EndCode가 있으므로 + 1로 계산
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

	// EndCode 부분 버퍼로 빼옴
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

