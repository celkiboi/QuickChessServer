#pragma once

enum ChessPieceType
{
    empty = 0,
    pawn = 1,
    rook = 2,
    knight = 4,
    bishop = 8,
    queen = 16,
    king = 32,

    white = 0x10000000,
    black = 0x01000000,

    color = 0x11000000,
    type = 0x0000FFFF
};

void InitializeBoard(UINT32* board);

void ProcessMove(UINT32* board, UINT16 moveData, HANDLE replayFile, BOOL enterNewRow);