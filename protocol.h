#pragma once

enum protocol
{
	/* SERVER TO CLIENT */
	// signals used for starting the game
	startGameWhite = 0xFF00,
	startGameBlack = 0x00FF,

	/* CLIENT TO SERVER AND SERVER TO CLIENT */
	// last 6 bits signal the starting position of a piece that is moving
	// white the previous 6 bits signal where the piece will move
	startPosition = 63, // 16 bit MSB ..000000111111 LSB
	endPosition = 4032, // 16 bit MSB ..111111000000 LSB

	// when a pawn reaches the end he can swap himself for a different piece.
	// when that happens the 3rd and 4th (from MSB) bits will communicate which piece is swapped
	swapPiece = 12288, // 16 bit MSB 0011... LSB
	swapForKnight = 0, //3rd and 4th bit 00
	swapForBishop = 4096, // 3rd and 4th bit 01
	swapForRook = 8192, // 3rd and 4th bit 10
	swapForQueen  = 12288, // 3rd and 4th bit 11

	// when a game ends this is sent to a client to communicate victory
	// client can send this to a server to communicate error or surrender (if time ran out), 
	// upon which the server will send the other client a victory
	endGame = 16384,

	// the MSB will be used to comunicate the winning side
	// no bits should be sent in combination, 
	// the victory message is checked using full bit AND operation (C/C++C#/Java &&)
	whiteVictory = 49152,
	blackVictory = 16384
};