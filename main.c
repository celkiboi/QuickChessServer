#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <tchar.h>

#include "protocol.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5454
#define MAX_GAMES 1
#define MAX_CLIENTS MAX_GAMES * 2

typedef struct GameInfo
{
	char player1Name[256];
	UINT8 player1NameLen;
	SOCKET player1Socket;
	char player2Name[256];
	UINT8 player2NameLen;
	SOCKET player2Socket;
	UINT8 gameId;
}GAME_INFO;

DWORD WINAPI PlayChess(LPVOID lpParam)
{
	GAME_INFO* gameInfo = (GAME_INFO*)lpParam;
	UINT16 communicationBuffer;
	UINT32 bytesRecieved = 0;

	printf("GAME #%d: Starting game with %s - white %s - black\n", gameInfo->gameId, gameInfo->player1Name, gameInfo->player2Name);

	send(gameInfo->player1Socket, gameInfo->player2Name, gameInfo->player2NameLen, 0);
	send(gameInfo->player2Socket, gameInfo->player1Name, gameInfo->player1NameLen, 0);

	if ((bytesRecieved = recv(gameInfo->player1Socket, &communicationBuffer, 2, 0)) > 0)
	{
		communicationBuffer = htons(startGameWhite);
		send(gameInfo->player1Socket, &communicationBuffer, 2, 0);
		bytesRecieved = 0;
	}
	if ((bytesRecieved = recv(gameInfo->player2Socket, &communicationBuffer, 2, 0)) > 0)
	{
		communicationBuffer = htons(startGameBlack);
		send(gameInfo->player2Socket, &communicationBuffer, 2, 0);
		bytesRecieved = 0;
	}
}

INT32 main()
{
	INT32 returnValue = 0;

	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		printf("Cannot start Winsock. Error: %d", WSAGetLastError());
		return 0;
	}

	UINT16 port = PORT; // REPLACE WITH LOADING LOGIC

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Cannot start listening socket. Error: %d", WSAGetLastError());
		returnValue = 1;
		goto exit;
	}

	SOCKADDR_IN listenSocketAddr = { 0 };
	listenSocketAddr.sin_family = AF_INET;
	listenSocketAddr.sin_port = htons(port);
	listenSocketAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(listenSocket, &listenSocketAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Cannot bind listening socket. Error: %d", WSAGetLastError());
		returnValue = 2;
		goto exit;
	}

	GAME_INFO gamesInfo[MAX_GAMES] = { 0 };

	listen(listenSocket, 10);
	printf("Server started on port %d\n", port);

	for (UINT32 i = 0; i < MAX_GAMES; i++)
	{
		gamesInfo[i].player1Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (gamesInfo[i].player1Socket == INVALID_SOCKET)
		{
			printf("Cannot create a client socket for player 1 number %d. Error: %d", i, WSAGetLastError());
			returnValue = 3;
			goto exit;
		}
		gamesInfo[i].player2Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (gamesInfo[i].player2Socket == INVALID_SOCKET)
		{
			printf("Cannot create a client socket for player 2 number %d. Error: %d", i, WSAGetLastError());
			returnValue = 4;
			goto exit;
		}
	}

	HANDLE gameThreads[MAX_GAMES] = { 0 };

	for (UINT32 i = 0; i < MAX_GAMES; i++)
	{
		gamesInfo[i].player1Socket = accept(listenSocket, NULL, NULL);
		INT32 bytesRecieved = 0;
		if ((bytesRecieved = recv(gamesInfo[i].player1Socket, &(gamesInfo[i].player1Name), sizeof(gamesInfo[i].player1Name), 0)) > 0)
		{
			printf("MAIN: Player %s has joined, waiting for a companion...\n", gamesInfo[i].player1Name);
		}
		gamesInfo[i].player1NameLen = bytesRecieved;

		bytesRecieved = 0;
		gamesInfo[i].player2Socket = accept(listenSocket, NULL, NULL);
		if ((bytesRecieved = recv(gamesInfo[i].player2Socket, &(gamesInfo[i].player2Name), sizeof(gamesInfo[i].player2Name), 0)) > 0)
		{
			printf("MAIN: Player %s has joined, starting match with %s...\n", gamesInfo[i].player2Name, gamesInfo[i].player1Name);
		}
		gamesInfo[i].player2NameLen = bytesRecieved;

		gamesInfo[i].gameId = i + 1;
		gameThreads[i] = CreateThread(NULL, 0, PlayChess, (LPVOID*)&gamesInfo[i], 0, NULL);
	}

	WaitForMultipleObjects(MAX_GAMES, &gameThreads, TRUE, INFINITE);

exit:
	if (listenSocket != INVALID_SOCKET)
		closesocket(listenSocket);

	for (INT32 i = 0; i < MAX_GAMES; i++)
	{
		if (gamesInfo[i].player1Socket != INVALID_SOCKET)
			closesocket(gamesInfo[i].player1Socket);
		if (gamesInfo[i].player2Socket != INVALID_SOCKET)
			closesocket(gamesInfo[i].player2Socket);
	}

	WSACleanup();

	return returnValue;
}