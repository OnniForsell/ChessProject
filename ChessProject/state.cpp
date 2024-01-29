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

	_current_turn = get_opponent(_current_turn);
}


/// Find all of the piece's possible moves in the current direction
/// 
/// \param row		The starting row of the piece
/// \param collumn	The starting collumn of the piece
/// \param player		The player who's piece is being moved
/// \param max_steps	The maximum amount of squares the piece is allowed to move
/// \param moves		An array to store the simulated moves in
/// \param can_take		Checks if the piece is allowed to take other pieces
/// \param must_take	Checks if the piece has to take, this only applies to the pawn
/// \param row_delta	The direction that the piece will move on the x-axis
/// \param collumn_delta	The direction that the piece will move on the y-axis
void State::raw_move_in_direction(int row, int collumn, int player, int max_steps, bool can_take, bool must_take, std::vector<Move>& moves, int row_delta, int collumn_delta) {
	int row_now = row;
	int collumn_now = collumn;
	int steps = 0;

	while (steps < max_steps) {
		row_now += row_delta;
		collumn_now += collumn_delta;

			// Check if the piece is going outside the board
			if (row_now < 0 || row_now > 7 || collumn_now < 0 || collumn_now > 7) {
				break;
			}

			// Check if the space is empty
			if (_board[row_now][collumn_now] == NA) {
				moves.push_back(Move(row, collumn, row_now, collumn_now));
				steps++;
				continue;
			}

			if (can_take)
			{
				// Check if we're colliding with one of our own pieces
				if (get_piece_color(_board[row_now][collumn_now]) == player) {
					std::cout << "Collided with own piece \n";
					break;
				}
			}

			moves.push_back(Move(row, collumn, row_now, collumn_now));
			break;
	}
}

// Simulate all the possible moves the rook could do to determine where it can and can't move
// in this current turn
void State::give_raw_move_rook(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 0, 1);
}

// Simulate all the possible moves the knight could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_knight(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up then left
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -2, 1);

	// Up then right
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -2, -1);

	// Up then left
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 2, 1);

	// Down then right
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 2, -1);

	// Left then up
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -1, -2);

	// Left then down
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 1, -2);

	// Right then up
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -1, 2);

	// Right then down
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -1, 2);
}

// Simulate all the possible moves the bishop could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_bishop(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up-Left
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 1, 1);
}

// Simulate all the possible moves the king could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_queen(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 0, 1);

	// Up-Left
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, collumn, player, 7, true, false, moves, 1, 1);
}

// Simulate all the possible moves the king could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_king(int row, int collumn, int player, std::vector<Move>& moves) {

	// Up
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 0, 1);

	// Up-Left
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, collumn, player, 1, true, false, moves, 1, 1);
}

// Print an ascii-graphic visualising the pieces and the board
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