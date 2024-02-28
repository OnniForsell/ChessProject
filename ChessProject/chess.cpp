#include "chess.h"

// Check if a piece is white or black
int get_piece_color(int piece) {

	switch (piece) {
	case wR: case wN: case wB: case wQ: case wK: case wP:
		return WHITE;
		break;
	default:
		return BLACK;
		break;

	}
}

// Get the turn player's opponent as a variable value
int get_opponent(int player) {
	return player == WHITE ? BLACK : WHITE;
}