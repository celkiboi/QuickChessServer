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
	UINT32 moverColor = (board[start] & color);
	BOOL hasPawnPromoted = (moveData & doPawnPromotion) != 0;

	BOOL hasCaptured = FALSE;
	if (board[end] != empty)
		hasCaptured = TRUE;
	
	UINT32 type = board[start] & 0xFF;
	
	UINT32 startHeight = start / 8;
	UINT32 startWidth = start % 8;
	BOOL hasEnPassant = FALSE;
	// Case: white en passant
	if (startHeight == 4 && (moverColor & color) == white && type == pawn)
	{
		// Case: left
		if ((end - start) == 7 && (board[start - 1] & type) == pawn && (board[start - 1] & color) == black)
		{
			hasEnPassant = TRUE;
			board[start - 1] = empty;
		}
		// Case: right
		if ((end - start) == 9 && (board[start + 1] & type) == pawn && (board[start + 1] & color) == black)
		{
			hasEnPassant = TRUE;
			board[start + 1] = empty;
		}
	}
	// Case: black en passant
	else if (startHeight == 3 && (moverColor & color) == black && type == pawn)
	{
		// Case: left
		if ((end - start) == -9 && (board[start - 1] & type) == pawn && (board[start - 1] & color) == white)
		{
			hasEnPassant = TRUE;
			board[start - 1] = empty;
		}
		// Case: right
		if ((end - start) == -7 && (board[start + 1] & type) == pawn && (board[start + 1] & color) == white)
		{
			hasEnPassant = TRUE;
			board[start + 1] = empty;
		}
	}
	hasCaptured = hasEnPassant ? TRUE : hasCaptured;

	board[end] = board[start];
	board[start] = empty;

	UINT8 startRow = start / 8 + 49;
	CHAR startColumn = (start % 8) + 65;
	UINT8 endRow = end / 8 + 49;
	CHAR endColumn = (end % 8) + 65;

	CHAR buffer[32] = { '\0' };
	DWORD numberOfBytesWritten;

	if (enterNewRow)
	{
		CHAR moveNumberBuffer[32] = { '\0' };
		sprintf_s(&moveNumberBuffer, sizeof(moveNumberBuffer), "\n%d. ", moveNumber);
		WriteFile(replayFile, moveNumberBuffer, strlen(&moveNumberBuffer), &numberOfBytesWritten, NULL);
	}

	UINT8 i = 0;

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

	if (hasEnPassant)
	{
		buffer[i++] = 'e';
		buffer[i++] = '.';
		buffer[i++] = 'p';
		buffer[i++] = '.';
		buffer[i++] = ' ';

	}
	WriteFile(replayFile, buffer, i, &numberOfBytesWritten, NULL);
}