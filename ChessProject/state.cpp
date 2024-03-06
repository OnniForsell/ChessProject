#include <iostream>
#include <fcntl.h>
#include <Windows.h>
#include <io.h>
#include "state.h"


/// 
/// \param player		The player who's pieces will be affected
/// \param moves		The turn player's list of available moves, which will be filled with the raw moves
void State::give_all_raw_moves(int player, std::vector<Move>& moves) const {
	int raw_moves = 0;
	int move_index = 0;

	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			if (player == WHITE) {
				switch (_board[row][column]) {
				case wR:
					give_raw_move_rook(move_index, row, column, player, moves);
					break;
				case wN:
					give_raw_move_knight(move_index, row, column, player, moves);
					break;
				case wB:
					give_raw_move_bishop(move_index, row, column, player, moves);
					break;
				case wQ:
					give_raw_move_queen(move_index, row, column, player, moves);
					break;
				case wK:
					give_raw_move_king(move_index, row, column, player, moves);
					break;
				case wP:
					give_raw_move_pawn(move_index, row, column, player, moves);
					break;
				}
			}
			else {
				switch (_board[row][column]) {
				case bR:
					give_raw_move_rook(move_index, row, column, player, moves);
					break;
				case bN:
					give_raw_move_knight(move_index, row, column, player, moves);
					break;
				case bB:
					give_raw_move_bishop(move_index, row, column, player, moves);
					break;
				case bQ:
					give_raw_move_queen(move_index, row, column, player, moves);
					break;
				case bK:
					give_raw_move_king(move_index, row, column, player, moves);
					break;
				case bP:
					give_raw_move_pawn(move_index, row, column, player, moves);
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

// Find the coordinates of a specific piece
//
// \param piece	The name of the piece being searched for (wR, bR, wK, bK, bP, wP etc.)
// \param row	The row where the piece is, this value is defined outside the function and will be changed to match the return value
void State::search_for_piece(int piece, int& row, int& column) const {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (_board[i][j] == piece) {
				row = i;
				column = j;
				return;
			}
		}
	}
}


// Visualizes the given move on the board. We assume the move is legal
void State::make_move(const Move& m) {

	// Declare the piece to be moved
	int piece = _board[m._s_r][m._s_c];

	// Erase the starting square
	_board[m._s_r][m._s_c] = NA;

	if ((piece == wP || piece == bP) && abs(m._s_c - m._e_c) == 1 && _board[m._e_r][m._e_c] == NA)
	{
		// Pawn moves diagonally to an empty square => must be en passant!
		// Remove the enemy pawn.
		_board[m._s_r][m._e_c] = NA;
	}

	// Move the original piece to the ending square
	// _board[m._e_r][m._e_c] = piece;

	if (m._piece_to_promote != NA) {
		_board[m._e_r][m._e_c] = m._piece_to_promote;
	}
	else {
		_board[m._e_r][m._e_c] = piece;
	}

	// Check if the player castled, 
	// at which point the rook has to also be moved

	if (piece == wK && m._s_r == 7 && m._s_c == 4 && m._e_r == 7 && m._e_c == 6) {
		// Move was white's short castle, so
		// the rook will also move
		_board[7][7] = NA;
		_board[7][5] = wR;
	}
	else if (piece == wK && m._s_r == 7 && m._s_c == 4 && m._e_r == 7 && m._e_c == 2) {
		// Move was white's long castle, so
		// the rook will also move
		_board[7][0] = NA;
		_board[7][3] = wR;
	}
	else if (piece == bK && m._s_r == 0 && m._s_c == 4 && m._e_r == 0 && m._e_c == 6) {
		// Move was black's short castle, so
		// the rook will also move
		_board[0][7] = NA;
		_board[0][5] = bR;
	}
	else if (piece == bK && m._s_r == 0 && m._s_c == 4 && m._e_r == 0 && m._e_c == 2) {
		// Move was black's long castle, so
		// the rook will also move
		_board[0][0] = NA;
		_board[0][3] = bR;
	}


	// Update the castling flags:
	// If the player moves the king, then it
	// can't castle anymore
	if (piece == wK) {
		white_long_castling_allowed = false;
		white_short_castling_allowed = false;
	}
	else if (piece == bK) {
		black_long_castling_allowed = false;
		black_short_castling_allowed = false;
	}

	// Check if white's left rook moved
	if (m._s_r == 7 && m._s_c == 0) {
		white_long_castling_allowed = false;
	}
	// Check if white's right rook moved
	else if (m._s_r == 7 && m._s_c == 7) {
		white_short_castling_allowed = false;
	}
	// Check if black's left rook moved
	else if (m._s_r == 0 && m._s_c == 0) {
		black_long_castling_allowed = false;
	}
	// Check if black's right rook moved
	else if (m._s_r == 0 && m._s_c == 7) {
		black_short_castling_allowed = false;
	}


	// Check if White's or Black's pawns moved two squares
	if (piece == wP) {
		if ((m._s_r - m._e_r) == 2) {
			_doublestep_on_column = m._s_c;
			// std::cout << "White pawn double step" << "\n";
		}

		if (m._s_c != m._e_c) {
			if (_board[m._e_r][m._e_c] == NA) {
				_board[m._s_r][m._e_c] = NA;
			}
		}
	}
	else if (piece == bP) {
		if ((m._e_r - m._s_r) == 2) {
			_doublestep_on_column = m._s_c;
			// std::cout << "Black pawn double step" << "\n";
		}

		if (m._s_c != m._e_c) {
			if (_board[m._e_r][m._e_c] == NA) {
				_board[m._s_r][m._e_c] = NA;
			}
		}
	}
	else {
		_doublestep_on_column = -1;
	}

	_current_turn = get_opponent(_current_turn);

	// Castling: wK e1-g1 or e1-c1
	//			 bK e8-g8 or e8-c8
}


/// Find all of the piece's possible moves in the current direction
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player		The player who's piece is being moved
/// \param max_steps	The maximum amount of squares the piece is allowed to move
/// \param moves		An array to store the simulated moves in
/// \param can_take	Checks if the piece is allowed to take other pieces
/// \param must_take	Checks if the piece has to take when moving, this only applies to the pawn
/// \param row_delta	The direction that the piece will move on the x-axis
/// \param column_delta	The direction that the piece will move on the y-axis
void State::raw_move_in_direction(int& move_index, int row, int column, int player, int max_steps, bool can_take, bool must_take, std::vector<Move>& moves, int row_delta, int column_delta) const {
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
			if (must_take)
				break;

			//moves.push_back(Move(row, column, row_now, column_now));
			set_move_with_index(move_index, moves, Move(row, column, row_now, column_now));
			steps++;
			continue;
		}

		// Check if we're colliding with one of our own pieces
		if (get_piece_color(_board[row_now][column_now]) == player) {
			// std::cout << "Collided with own piece \n";
			break;
		}

		if (can_take)
		{
			// std::cout << "Collided with opponent's " << _board[row_now][column_now] << "\n";
			//moves.push_back(Move(row, column, row_now, column_now));
			set_move_with_index(move_index, moves, Move(row, column, row_now, column_now));
		}
		break;
	}
}


// Simulate all the possible moves the rook could do to determine where it can and can't move
// in this current turn
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player	The turn player (1 if White, 0 if Black)
/// \param moves	The vector in which the generated moves will be stored into
void State::give_raw_move_rook(int& move_index, int row, int column, int player, std::vector<Move>& moves) const {

	// Up
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 0, 1);
}


// Simulate all the possible moves the knight could do to determine 
// where it can and can't move in this current turn
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player	The turn player (1 if White, 0 if Black)
/// \param moves	The vector in which the generated moves will be stored into
void State::give_raw_move_knight(int& move_index, int row, int column, int player, std::vector<Move>& moves) const {

	// Up then left
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -2, 1);

	// Up then right
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -2, -1);

	// Up then left
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 2, 1);

	// Down then right
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 2, -1);

	// Left then up
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -1, -2);

	// Left then down
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 1, -2);

	// Right then up
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -1, 2);

	// Right then down
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 1, 2);
}


// Simulate all the possible moves the bishop could do to determine 
// where it can and can't move in this current turn
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player	The turn player (1 if White, 0 if Black)
/// \param moves	The vector in which the generated moves will be stored into
void State::give_raw_move_bishop(int& move_index, int row, int column, int player, std::vector<Move>& moves) const {

	// Up-Left
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 1, 1);
}


// Simulate all the possible moves the king could do to determine 
// where it can and can't move in this current turn
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player	The turn player (1 if White, 0 if Black)
/// \param moves	The vector in which the generated moves will be stored into
void State::give_raw_move_queen(int& move_index, int row, int column, int player, std::vector<Move>& moves) const {

	// Up
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 0, 1);

	// Up-Left
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(move_index, row, column, player, 7, true, false, moves, 1, 1);
}


// Simulate all the possible moves the king could do to determine 
// where it can and can't move in this current turn
/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player	The turn player (1 if White, 0 if Black)
/// \param moves	The vector in which the generated moves will be stored into
void State::give_raw_move_king(int& move_index, int row, int column, int player, std::vector<Move>& moves) const {

	// Up
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -1, 0);

	// Down
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 1, 0);

	// Left
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 0, -1);

	// Right
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 0, 1);

	// Up-Left
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -1, -1);

	// Up-Right
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, -1, 1);

	// Down-Left
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 1, -1);

	// Down-Right
	raw_move_in_direction(move_index, row, column, player, 1, true, false, moves, 1, 1);
}

/// 
/// \param row		The starting row of the piece
/// \param column	The starting column of the piece
/// \param player	The turn player (1 if White, 0 if Black)
/// \param moves	The vector in which the generated moves will be stored into
void State::give_raw_move_pawn(int& move_index, int row, int column, int player, std::vector<Move>& moves) const {

	std::vector<Move> pawn_moves;

	if (player == WHITE) {

		if (row == 6) {
			// Left
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, -1, -1);
			// Right
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, -1, 1);
			// Up
			raw_move_in_direction(move_index, row, column, player, 2, false, false, pawn_moves, -1, 0);
		}
		else {
			// Up
			raw_move_in_direction(move_index, row, column, player, 1, false, false, pawn_moves, -1, 0);
			// Left
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, -1, -1);
			// Right
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, -1, 1);
		}
	}
	else {

		if (row == 1) {
			// Left
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, 1, -1);
			// Right
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, 1, 1);
			// Down
			raw_move_in_direction(move_index, row, column, player, 2, false, false, pawn_moves, 1, 0);
		}
		else {
			// Down
			raw_move_in_direction(move_index, row, column, player, 1, false, false, pawn_moves, 1, 0);
			// Left
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, 1, -1);
			// Right
			raw_move_in_direction(move_index, row, column, player, 1, true, true, pawn_moves, 1, 1);
		}
	}

	if (_doublestep_on_column != -1) {
		if (player == WHITE) {
			if (((column == _doublestep_on_column - 1) or (column == _doublestep_on_column + 1)) && row == 3) {
				moves.push_back(Move(row, column, 2, _doublestep_on_column, NA));
			}
		}
		else if (player == BLACK) {
			if (((column == _doublestep_on_column - 1) or (column == _doublestep_on_column + 1)) && row == 4) {
				moves.push_back(Move(row, column, 5, _doublestep_on_column, NA));
			}
		}
	}

	for (Move& m : pawn_moves)
	{
		// Check if the last row is the
		// last row on the board, if so, add the promotions
		// as possible moves
		if (m._e_r == 0) {
			// White's pawns
			/*moves.push_back(Move(row, column, m._e_r, m._e_c, wR));
			moves.push_back(Move(row, column, m._e_r, m._e_c, wN));
			moves.push_back(Move(row, column, m._e_r, m._e_c, wB));
			moves.push_back(Move(row, column, m._e_r, m._e_c, wQ)); */

			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, wR));
			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, wN));
			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, wB));
			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, wQ));
		}
		else if (m._e_r == 7) {
			// Black's pawns
			/*moves.push_back(Move(row, column, m._e_r, m._e_c, bR));
			moves.push_back(Move(row, column, m._e_r, m._e_c, bN));
			moves.push_back(Move(row, column, m._e_r, m._e_c, bB));
			moves.push_back(Move(row, column, m._e_r, m._e_c, bQ));*/

			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, bR));
			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, bN));
			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, bB));
			set_move_with_index(move_index, moves, Move(row, column, m._e_r, m._e_c, bQ));
		}
		else {
			// If the pawn cannot promote
			// itself in this move
			// moves.push_back(m);
			set_move_with_index(move_index, moves, m);

			//set_move_with_index(move_index, moves, row, column, m._e_r, m._e_r);
		}
	}
}


// Helper function to make adding moves to the legal moves list faster
//
// \param move_index	The current index in the "moves" vector we want to place the move into
// \param move	The move being put into the vector
// \param moves	The vector we keep all our legla moves in
void State::set_move_with_index(int& move_index, std::vector<Move>& moves, const Move& new_move) const {

	if (move_index < moves.size())
	{
		moves[move_index] = new_move;
	}
	else
	{
		moves.push_back(new_move);
	}

	move_index++;
}


// Print an ascii-graphic visualising the pieces and the board
void State::print_board() const {

	/*const std::string pieces[] = {"R", "N", "B", "Q", "K", "P", "r", "n", "b", "q", "k", "p", " "};

	std::cout << "  A   B   C   D   E   F   G   H\n";

	for (int row = 0; row < 8; row++) {
		std::cout << "+---+---+---+---+---+---+---+---+\n";
		std::cout << "|";
		for (int column = 0; column < 8; column++) {
			std::cout << " " << pieces[_board[row][column]] << " |";
		}
		std::cout << " " << 8 - row << "\n";
	}
	std::cout << "+---+---+---+---+---+---+---+---+\n";
	std::cout << "  A   B   C   D   E   F   G   H\n"; */


	// Switch to Unicode output
	(void)_setmode(_fileno(stdout), _O_U16TEXT);

	const std::wstring pieces[] =
	{ L"\u2656 ", L"\u2658 ", L"\u2657 ", L"\u2655 ", L"\u2654 ", L"\u2659 ", L"\u265C ", L"\u265E ", L"\u265D ", L"\u265B ", L"\u265A ", L"\u265F ", L"  " };

	int rows = 8;

	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			if ((column % 2 == 0 && row % 2 == 0) || (column % 2 == 1 && row % 2 == 1)) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY);
			}
			std::wcout << " " << pieces[_board[row][column]];
		}

		// Rest the text background color and number the row
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::wcout << " " << rows;
		rows--;
		std::wcout << "\n";
	}

	// Restore default text output to prevent Assertion Errors
	(void)_setmode(_fileno(stdout), _O_TEXT);
	// Restore text color to prevent a weird white background from appearing
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << " A  B  C  D  E  F  G  H\n";
}