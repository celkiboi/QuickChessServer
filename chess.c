#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include "chess.h"
#include "protocol.h"

void InitializeBoard(UINT32* board)
{
	board[0] = white | rook;
	board[1] = white | knight;
	board[2] = white | bishop;
	board[3] = white | queen;
	board[4] = white | king;
	board[5] = white | bishop;
	board[6] = white | knight;
	board[7] = white | rook;
	for (UINT32 i = 8; i < 16; i++)
		board[i] = white | pawn;

	for (UINT32 i = 16; i < 48; i++)
		board[i] = empty;

	for (UINT32 i = 48; i < 56; i++)
		board[i] = black | pawn;
	board[56] = black | rook;
	board[57] = black | knight;
	board[58] = black | bishop;
	board[59] = black | queen;
	board[60] = black | king;
	board[61] = black | bishop;
	board[62] = black | knight;
	board[63] = black | rook;
}

void ProcessMove(UINT32* board, UINT16 moveData, HANDLE replayFile, BOOL enterNewRow, UINT32 moveNumber)
{
	UINT32 start = moveData & startPosition;
	UINT32 end = (moveData & endPosition) >> 6;
	BOOL hasPawnPromoted = (moveData & doPawnPromotion) != 0;
	CHAR buffer[16] = { '\0' };
	DWORD numberOfBytesWritten;

	BOOL hasCaptured = FALSE;
	if (board[end] != empty)
		hasCaptured = TRUE;

	board[end] = board[start];
	board[start] = empty;

	UINT8 startRow = start / 8 + 49;
	CHAR startColumn = (start % 8) + 65;
	UINT8 endRow = end / 8 + 49;
	CHAR endColumn = (end % 8) + 65;

	if (enterNewRow)
	{
		CHAR moveNumberBuffer[32] = { '\0' };
		sprintf_s(&moveNumberBuffer, sizeof(moveNumberBuffer), "\n%d. ", moveNumber);
		WriteFile(replayFile, moveNumberBuffer, strlen(&moveNumberBuffer), &numberOfBytesWritten, NULL);
	}

	UINT8 i = 0;

	UINT32 type = board[end] & 0xFF;
	if (type == queen)
		buffer[i++] = 'Q';

	if (type == bishop)
		buffer[i++] = 'B';
	else if (type == rook)
		buffer[i++] = 'R';
	else if (type == knight)
		buffer[i++] = 'N';
	else if (type == knight)
		buffer[i++] = 'K';

	buffer[i++] = startColumn;
	buffer[i++] = startRow;

	buffer[i++] = hasCaptured ? 'x' : ' ';

	buffer[i++] = endColumn;
	buffer[i++] = endRow;
	buffer[i++] = ' ';

	WriteFile(replayFile, buffer, i, &numberOfBytesWritten, NULL);
}