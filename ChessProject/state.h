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

	void raw_move_in_direction(int row, int collumn, int player, int max_steps, bool can_take, bool must_take, std::vector<Move>& moves, int row_delta, int collumn_delta);

	void give_raw_move_rook(int row, int collumn, int player, std::vector<Move>& moves);
	void give_raw_move_knight(int row, int collumn, int player, std::vector<Move>& moves);
	void give_raw_move_bishop(int row, int collumn, int player, std::vector<Move>& moves);
	void give_raw_move_queen(int row, int collumn, int player, std::vector<Move>& moves);
	void give_raw_move_king(int row, int collumn, int player, std::vector<Move>& moves);

private:

	// The pieces on the board, indexed as [row][collumn], for example:
	// [0][0] : upper-left corner ("a8")
	// [7][0] : lower-left corner ("a1")
	// [7][7] : lower-right corner ("h1")

	int _board[8][8] = {
		{bR, bN, bB, bQ, bK, bB, bN, bR},
		{bP, bP, bP, bP, bP, bP, bP, bP},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wP, wP, wP, wP, wP, wP, wP, wP},
		{wR, wN, wB, wQ, wK, wB, wN, wR}
	};

	int _current_turn = WHITE;
};