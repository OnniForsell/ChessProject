#include <iostream>
#include "state.h"


void State::erase_board() {
	// TODO:
	// Turn every space on the board to NA
	// (with a nested loop)

	for (int row = 0; row < 8; row++) {

		for (int collumn = 0; collumn < 8; collumn++) {
			_board[row][collumn] = NA;
		}
	}
}


// Visualizes the given move on the board. We assume the move is legal
void State::make_move(const Move& m) {

	// Declare the piece to be moved
	int piece = _board[m._s_r][m._s_c];

	// Erase the starting square
	_board[m._s_r][m._s_c] = NA;

	// Move the original piece to the ending square
	_board[m._e_r][m._e_c] = piece;
}

// Take the coordinates of a given piece along with the directions
// it should be moved in (both on the x and y axis), then perform simulated
// raw movements to see where the piece can move

void State::raw_move_in_direction(int row, int collumn, int player, std::vector<Move>& moves, int row_delta, int collumn_delta) {
	int row_now = row;
	int collumn_now = collumn;

	while (true) {
		row_now += row_delta;
		collumn_now += collumn_delta;

		// Check if the piece is going outside the board
		if (row_now < 0 || row_now >= 8 || collumn_now < 0 || collumn_now >= 8) {
			break;
		}

		// Check if the space is empty
		if (_board[row_now][collumn_now] == NA) {
			moves.push_back(Move(row, collumn, row_now, collumn_now));
			continue;
		}

		// Check if we're colliding with one of our own pieces
		if (get_piece_color(_board[row_now][collumn_now]) == player) {
			std::cout << "Collided with own piece \n";
			break;
		}

		moves.push_back(Move(row, collumn, row_now, collumn_now));
		break;
	}
}


void State::give_raw_move_rook(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up
	raw_move_in_direction(row, collumn, player, moves, -1, 0);

	// Down
	raw_move_in_direction(row, collumn, player, moves, 1, 0);

	// Left
	raw_move_in_direction(row, collumn, player, moves, 0, -1);

	// Right
	raw_move_in_direction(row, collumn, player, moves, 0, 1);
}


void State::give_raw_move_bishop(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up-Left
	raw_move_in_direction(row, collumn, player, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, collumn, player, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, collumn, player, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, collumn, player, moves, 1, 1);
}


void State::print_board() const {
	/* for (int row = 0; row < 8; row++) {
		std::cout << "\n";
		std::cout << "-----------------------------------------" << "\n" << "|";
		for (int collumn = 0; collumn < 8; collumn++) {
			if (_board[row][collumn] >= 10) {
				std::cout << " " << _board[row][collumn] << " |";
			}
			else {
				std::cout << " 0" << _board[row][collumn] << " |";
			}
		}
	} */

	const std::string pieces[] = { "R", "N", "B", "Q" , "K" , "P", "r", "n", "b", "q", "k", "p", " "};

	std::cout << "  A   B   C   D   E   F   G   H";

	for (int row = 0; row < 8; row++) {
		std::cout << "\n";
		std::cout << "+---+---+---+---+---+---+---+---+" << "\n" << "|";
		for (int collumn = 0; collumn < 8; collumn++) {
			std::cout << " " << pieces[_board[row][collumn]] << " |";
		}
	}

	std::cout << "\n+---+---+---+---+---+---+---+---+" << "\n";
}