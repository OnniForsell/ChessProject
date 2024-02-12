#pragma once
#include "chess.h"
#include "move.h"

class State 
{
public:
	void erase_board();
	void make_move(const Move& m);

	// Homework
	// Print the board as an ascii-graphic
	void print_board() const;

	// MOVE GENERATOR
	// Provides all *legal* moves of the current game state
	void give_moves(std::vector<Move>& moves) {
		int king = _current_turn == WHITE ? wK : bK;
		int player = _current_turn;
		int opponent = get_opponent(player);

		// Create the turn player's raw moves. Some might
		// leave the king under attack
		std::vector<Move> raw_moves;
		give_all_raw_moves(player, raw_moves);

		give_castles(player, raw_moves);

		// Test every raw move

		for (Move& rm : raw_moves) {

			// Create a copy of the current state to prevent
			// destruction to the actual game state
			State test_state = *this;

			// Play a raw move in the test state
			test_state.make_move(rm);

			// Find the king
			int row, column;
			test_state.search_for_piece(king, row, column);

			// Did this move put the king in check?
			if (!test_state.is_square_threatened(row, column, opponent)) {
				moves.push_back(rm);
			}
		}
	}

	void give_castles(int player, std::vector<Move>& moves) const {
		if (player == WHITE) {
			if (white_short_castling_allowed && _board[7][5] == NA && _board[7][6] == NA && 
				!is_square_threatened(7, 4, BLACK) && !is_square_threatened(7, 5, BLACK)) {
				moves.push_back(Move(7, 4, 7, 6));
			}
			if (white_long_castling_allowed && _board[7][1] == NA && _board[7][2] == NA && _board[7][3] == NA &&
				!is_square_threatened(7, 4, BLACK) && !is_square_threatened(7, 5, BLACK)) {
				moves.push_back(Move(7, 4, 7, 2));
			}
		}
		else {
			if (black_short_castling_allowed && _board[0][5] == NA && _board[0][6] == NA &&
				!is_square_threatened(0, 4, WHITE) && !is_square_threatened(0, 5, WHITE)) {
				moves.push_back(Move(0, 4, 0, 6));
			}
			if (black_long_castling_allowed && _board[0][1] == NA && _board[0][2] == NA && _board[0][3] == NA &&
				!is_square_threatened(0, 2, WHITE) && !is_square_threatened(0, 3, WHITE)) {
				moves.push_back(Move(0, 4, 0, 2));
			}
		}
	}

	void give_all_raw_moves(int player, std::vector<Move>& moves) const;

	void raw_move_in_direction(int row, int column, int player, int max_steps, bool can_take, bool must_take, std::vector<Move>& moves, int row_delta, int column_delta) const;

	void give_raw_move_rook(int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_knight(int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_bishop(int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_queen(int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_king(int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_pawn(int row, int column, int player, std::vector<Move>& moves) const;

	// Finds a given piece's (for example wK) location on the board
	void search_for_piece(int piece, int& row, int& column) const;

	// Check if the piece in the current square is under threat of capture
	bool is_square_threatened(int row, int column, int threatening_player) const {
		// Generate raw moves for the threatening player
		std::vector<Move> threatening_moves;
		give_all_raw_moves(threatening_player, threatening_moves);

		// Check if any raw moves have coordinates that equal the square being checked.
		// If it does, return true (otherwise false)

		for (const Move& move : threatening_moves) {
			if (move._e_r == row && move._e_c == column) {
				return true;
			}
		}

		return false;
	}

	int _current_turn = WHITE;

private:

	// The pieces on the board, indexed as [row][column], for example:
	// [0][0] : upper-left corner ("a8")
	// [7][0] : lower-left corner ("a1")
	// [7][7] : lower-right corner ("h1")

	int _board[8][8] = {
		{bR, NA, NA, NA, bK, NA, NA, bR},
		{bP, bP, bP, bP, bP, bP, bP, bP},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wP, wP, wP, wP, wP, wP, wP, wP},
		{wR, NA, NA, NA, wK, NA, NA, wR}
	};

	/*int _board[8][8] = {
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, bR},
		{NA, NA, wK, wP, NA, NA, NA, bQ},
		{NA, NA, NA, NA, NA, NA, NA, bR},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA}
	}; */

	// Keep track of if white or black is allowed to castle
	// in either direction. True by default
	bool white_short_castling_allowed = true;
	bool white_long_castling_allowed = true;
	bool black_short_castling_allowed = true;
	bool black_long_castling_allowed = true;

	// En passant

};