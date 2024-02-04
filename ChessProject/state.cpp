#include <iostream>
#include "state.h"


void State::give_all_raw_moves(int player, std::vector<Move>& moves) const {
	int raw_moves = 0;
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			if (player == WHITE) {
				switch (_board[row][column]) {
				case wR:
					give_raw_move_rook(row, column, player, moves);
					break;
				case wN:
					give_raw_move_knight(row, column, player, moves);
					break;
				case wB:
					give_raw_move_bishop(row, column, player, moves);
					break;
				case wQ:
					give_raw_move_queen(row, column, player, moves);
					break;
				case wK:
					give_raw_move_king(row, column, player, moves);
					break;
				case wP:
					give_raw_move_pawn(row, column, player, moves);
					break;
				}
			}
			else {
				switch (_board[row][column]) {
				case bR:
					give_raw_move_rook(row, column, player, moves);
					break;
				case bN:
					give_raw_move_knight(row, column, player, moves);
					break;
				case bB:
					give_raw_move_bishop(row, column, player, moves);
					break;
				case bQ:
					give_raw_move_queen(row, column, player, moves);
					break;
				case bK:
					give_raw_move_king(row, column, player, moves);
					break;
				case bP:
					give_raw_move_pawn(row, column, player, moves);
					break;
				}
			}
		}
	}
}

void State::erase_board() {
	// TODO:
	// Turn every space on the board to NA
	// (with a nested loop)

	for (int row = 0; row < 8; row++) {

		for (int column = 0; column < 8; column++) {
			_board[row][column] = NA;
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
	// _board[m._e_r][m._e_c] = piece;

	if (m._piece_to_promote != NA) {
		std::cout << m._piece_to_promote;
		_board[m._e_r][m._e_c] = m._piece_to_promote;
	}
	else {
		_board[m._e_r][m._e_c] = piece;
	}

	_current_turn = get_opponent(_current_turn);
}


/// Find all of the piece's possible moves in the current direction
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player		The player who's piece is being moved
/// \param max_steps	The maximum amount of squares the piece is allowed to move
/// \param moves		An array to store the simulated moves in
/// \param can_take		Checks if the piece is allowed to take other pieces
/// \param must_take	Checks if the piece has to take when moving, this only applies to the pawn
/// \param row_delta	The direction that the piece will move on the x-axis
/// \param column_delta	The direction that the piece will move on the y-axis
void State::raw_move_in_direction(int row, int column, int player, int max_steps, bool can_take, bool must_take, std::vector<Move>& moves, int row_delta, int column_delta) const {
	int row_now = row;
	int column_now = column;
	int steps = 0;

	while (steps < max_steps) {
		row_now += row_delta;
		column_now += column_delta;

			// Check if the piece is going outside the board
			if (row_now < 0 || row_now > 7 || column_now < 0 || column_now > 7) {
				break;
			}

			// Check if the space is empty
			if (_board[row_now][column_now] == NA) {
				if (must_take) break;

				moves.push_back(Move(row, column, row_now, column_now));
				steps++;
				continue;
			}

			if (can_take)
			{
				// Check if we're colliding with one of our own pieces
				if (get_piece_color(_board[row_now][column_now]) == player) {
					std::cout << "Collided with own piece \n";
					break;
				}
				if (get_piece_color(_board[row_now][column_now]) != player) {
					std::cout << "Collided with opponent's " << _board[row_now][column_now] << "\n";
					moves.push_back(Move(row, column, row_now, column_now));
				}
			}

			moves.push_back(Move(row, column, row_now, column_now));
			break;
	}
}


// Simulate all the possible moves the rook could do to determine where it can and can't move
// in this current turn
void State::give_raw_move_rook(int row, int column, int player, std::vector<Move>& moves) const {

	// Up
	raw_move_in_direction(row, column, player, 7, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(row, column, player, 7, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(row, column, player, 7, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(row, column, player, 7, true, false, moves, 0, 1);
}


// Simulate all the possible moves the knight could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_knight(int row, int column, int player, std::vector<Move>& moves) const {

	// Up then left
	raw_move_in_direction(row, column, player, 1, true, false, moves, -2, 1);

	// Up then right
	raw_move_in_direction(row, column, player, 1, true, false, moves, -2, -1);

	// Up then left
	raw_move_in_direction(row, column, player, 1, true, false, moves, 2, 1);

	// Down then right
	raw_move_in_direction(row, column, player, 1, true, false, moves, 2, -1);

	// Left then up
	raw_move_in_direction(row, column, player, 1, true, false, moves, -1, -2);

	// Left then down
	raw_move_in_direction(row, column, player, 1, true, false, moves, 1, -2);

	// Right then up
	raw_move_in_direction(row, column, player, 1, true, false, moves, -1, 2);

	// Right then down
	raw_move_in_direction(row, column, player, 1, true, false, moves, -1, 2);
}


// Simulate all the possible moves the bishop could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_bishop(int row, int column, int player, std::vector<Move>& moves) const {

	// Up-Left
	raw_move_in_direction(row, column, player, 7, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, column, player, 7, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, column, player, 7, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, column, player, 7, true, false, moves, 1, 1);
}


// Simulate all the possible moves the king could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_queen(int row, int column, int player, std::vector<Move>& moves) const {

	// Up
	raw_move_in_direction(row, column, player, 7, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(row, column, player, 7, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(row, column, player, 7, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(row, column, player, 7, true, false, moves, 0, 1);

	// Up-Left
	raw_move_in_direction(row, column, player, 7, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, column, player, 7, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, column, player, 7, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, column, player, 7, true, false, moves, 1, 1);
}


// Simulate all the possible moves the king could do to determine 
// where it can and can't move in this current turn
void State::give_raw_move_king(int row, int column, int player, std::vector<Move>& moves) const {

	// Up
	raw_move_in_direction(row, column, player, 1, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(row, column, player, 1, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(row, column, player, 1, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(row, column, player, 1, true, false, moves, 0, 1);

	// Up-Left
	raw_move_in_direction(row, column, player, 1, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(row, column, player, 1, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(row, column, player, 1, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(row, column, player, 1, true, false, moves, 1, 1);
}


void State::give_raw_move_pawn(int row, int column, int player, std::vector<Move>& moves) const {

	std::vector<Move> pawn_moves;

	if (player == WHITE) {

		if (row == 6) {
			// Up
			raw_move_in_direction(row, column, player, 2, true, false, pawn_moves, -1, 0);
		}
		else {
			// Up
			raw_move_in_direction(row, column, player, 1, false, false, pawn_moves, -1, 0);
			// Left
			raw_move_in_direction(row, column, player, 1, true, true, pawn_moves, -1, -1);
			// Right
			raw_move_in_direction(row, column, player, 1, true, true, pawn_moves, -1, 1);
		}
	}
	else {

		if (row == 1) {
			// Down
			raw_move_in_direction(row, column, player, 2, true, false, pawn_moves, 1, 0);
		}
		else {
			// Down
			raw_move_in_direction(row, column, player, 1, false, false, pawn_moves, 1, 0);
			// Left
			raw_move_in_direction(row, column, player, 1, true, true, pawn_moves, 1, -1);
			// Right
			raw_move_in_direction(row, column, player, 1, true, true, pawn_moves, 1, 1);
		}
	}

	for (Move& s : pawn_moves)
	{
		// if (s._e_r == 0....

		// jos loppurivi ei ole per�rivi, niin
		// lis�� siirto s moves-vektoriin

		// jos loppurivi on per�rivi, niin
		// lis�� 4 eri siirtoa moves-vektoriin


		// Check if the last row is the
		// last row on the board, if so, add the promotions
		// as possible moves
		if (s._e_r == 0) {
			// White's pawns
			moves.push_back(Move(row, column, s._e_r, s._e_c, wR));
			moves.push_back(Move(row, column, s._e_r, s._e_c, wN));
			moves.push_back(Move(row, column, s._e_r, s._e_c, wB));
			moves.push_back(Move(row, column, s._e_r, s._e_c, wQ));
		}
		else if (s._e_r == 7) {
			// Black's pawns
			moves.push_back(Move(row, column, s._e_r, s._e_c, bR));
			moves.push_back(Move(row, column, s._e_r, s._e_c, bN));
			moves.push_back(Move(row, column, s._e_r, s._e_c, bB));
			moves.push_back(Move(row, column, s._e_r, s._e_c, bQ));
		}
		else {
			// If the pawn cannot promote
			// itself in this move
			moves.push_back(s);
		}
	}
}


// Print an ascii-graphic visualising the pieces and the board
void State::print_board() const {
	/* for (int row = 0; row < 8; row++) {
		std::cout << "\n";
		std::cout << "-----------------------------------------" << "\n" << "|";
		for (int column = 0; column < 8; column++) {
			if (_board[row][column] >= 10) {
				std::cout << " " << _board[row][column] << " |";
			}
			else {
				std::cout << " 0" << _board[row][column] << " |";
			}
		}
	} */

	const std::string pieces[] = { "R", "N", "B", "Q" , "K" , "P", "r", "n", "b", "q", "k", "p", " "};
	int rows = 8;

	std::cout << "  A   B   C   D   E   F   G   H";

	for (int row = 0; row < 8; row++) {
		std::cout << "\n";
		std::cout << "+---+---+---+---+---+---+---+---+" << "\n" << "|";
		for (int column = 0; column < 8; column++) {
			std::cout << " " << pieces[_board[row][column]] << " |";
		}
		std::cout << " " << (rows);
		rows--;
	}

	std::cout << "\n+---+---+---+---+---+---+---+---+" << "\n";
}