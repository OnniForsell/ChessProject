#pragma once
#include <limits>
#include <map>
#include <omp.h>
#include <future>
#include <thread>
#include "chess.h"
#include "move.h"


class MinMaxValue {

public:
	MinMaxValue(float value, Move move) :
		_value(value), _move(move) {}

	float _value;
	Move _move;
};

class State
{
public:
	void erase_board();
	void make_move(const Move& m);

	int _doublestep_on_column = -1;

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

	// Scores the final result of the game
	// in the following manner:
	// White mate			1000000
	// Stalemate			0
	// Black mate			-1000000
	// 
	// The function is called once we run out of legal moves
	float score_final_result() const {
		// std::cout << ":DDDDDDD" << "\n";

		if (_current_turn == WHITE) {

			// Find the king
			int row, column;
			search_for_piece(wK, row, column);

			// Is the king threatened?
			if (is_square_threatened(row, column, BLACK))
				return -1000000;
			else
				return 0;
		}
		else {
			// Same for black
			int row, column;
			search_for_piece(bK, row, column);

			if (is_square_threatened(row, column, WHITE))
				return 1000000;
			else
				return 0;
		}
	}

	// Heuristically score the state of the game
	float evaluate() const {
		return 10.0f * material() + 0.30f * mobility() + 1.0f * king_safety() +
			1 * bishop_left() + 1 * knight_left() + 1 * pawn_left() + 0.2f * centralization() +
			0.5f * pawn_structure();
	}

	#undef max
	#undef min
	/// Asynchronous version of the alphabeta function, which will split the workload among the machine's cores
	/// 
	/// \param depth		How deep the algorithm will go with it's calculations
	/// \param alpha		The alpha of the alphabeta
	/// \param beta			The beta of the alphabeta
	MinMaxValue parallel_alphabeta(int depth, float alpha, float beta)
	{
		std::vector<Move> moves;
		give_moves(moves);

		if (_current_turn == WHITE) {
			float best_value = _current_turn == WHITE ?
				std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
			Move best_move;
			std::vector<std::future<MinMaxValue>> futures;
			for (Move& m : moves) {
				State new_state = *this;
				new_state.make_move(m);
				futures.push_back(std::async(&State::alphabeta, new_state, depth - 1, alpha, beta));
			}
			for (int i = 0; i < moves.size(); ++i) {
				Move m = moves[i];
				MinMaxValue value = futures[i].get();
				if (value._value > best_value) {
					best_value = value._value;
					best_move = m;
				}
			}
			return MinMaxValue(best_value, best_move);
		}
		else {
			float best_value = _current_turn == WHITE ?
				std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
			Move best_move;
			std::vector<std::future<MinMaxValue>> futures;
			for (Move& m : moves) {
				State new_state = *this;
				new_state.make_move(m);
				futures.push_back(std::async(&State::alphabeta, new_state, depth - 1, alpha, beta));
			}
			for (int i = 0; i < moves.size(); ++i) {
				Move m = moves[i];
				MinMaxValue value = futures[i].get();
				if (value._value < best_value) {
					best_value = value._value;
					best_move = m;
				}
			}
			return MinMaxValue(best_value, best_move);
		}
	}

	#undef max
	#undef min
	/// 
	/// \param depth		How deep the algorithm will go with it's calculations
	/// \param alpha		The alpha of the alphabeta
	/// \param beta			The beta of the alphabeta
	MinMaxValue alphabeta(int depth, float alpha, float beta)
	{
		std::vector<Move> moves;
		give_moves(moves);
		if (moves.size() == 0) {
			return MinMaxValue(score_final_result(), Move());
		}
		if (depth == 0) {
			return MinMaxValue(evaluate(), Move());
		}
		if (_current_turn == WHITE) {
			float best_value = _current_turn == WHITE ?
				std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();

			Move best_move;

			for (Move& m : moves) {
				State new_state = *this;
				new_state.make_move(m);
				MinMaxValue value = new_state.alphabeta(depth - 1, alpha, beta);
				if (value._value > best_value) {
					best_value = value._value;
					best_move = m;
				}
				if (best_value > beta) {
					break;
				}
				alpha = std::max(alpha, best_value);
			}
			return MinMaxValue(best_value, best_move);
		}
		else {
			float best_value = _current_turn == WHITE ?
				std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
			Move best_move;
			for (Move& m : moves) {
				State new_state = *this;
				new_state.make_move(m);
				MinMaxValue value = new_state.alphabeta(depth - 1, alpha, beta);
				if (value._value < best_value) {
					best_value = value._value;
					best_move = m;
				}
				if (best_value < alpha) {
					break;
				}
				beta = std::min(beta, best_value);
			}
			return MinMaxValue(best_value, best_move);
		}
	}

	/// Asynchronous version of the alphabeta function, which will split the workload among the machine's cores
	/// 
	/// \param depth	How many moves ahead do we want the bot to think
	MinMaxValue parallel_minimax(int depth)
	{
		std::vector<Move> moves;
		give_moves(moves);
		float best_value = _current_turn == WHITE ?
			std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
		Move best_move;
		std::vector<std::future<MinMaxValue> > futures;
		for (Move& m : moves) {
			State new_state = *this;
			new_state.make_move(m);

			futures.push_back(std::async(&State::minmax, new_state, depth - 1));
		}

		for (int i = 0; i < moves.size(); ++i)
		{
			Move m = moves[i];
			MinMaxValue value = futures[i].get();

			if (_current_turn == WHITE && value._value > best_value) {
				best_value = value._value;
				best_move = m;
			}
			else if (_current_turn == BLACK && value._value < best_value) {
				best_value = value._value;
				best_move = m;
			}
		}
		return MinMaxValue(best_value, best_move);
	}

	// Gives the minimax-value of the board. Depth defines
	// how many more steps deep we'll go in the game tree
	//
	// Testing fe. in the main program
	//
	// State state;
	// float value = minimax(state, 2);
	///
	/// \param depth	How deep the calculation will go
	MinMaxValue minmax(int depth) {

		// Generate the moves of the game state
		std::vector<Move> moves;
		give_moves(moves);


		if (moves.size() == 0) {

			// Recurssion edge case 1:
			// Game ended (no more legal moves left)
			return MinMaxValue(score_final_result(), Move());
		}

		if (depth == 0) {
			// Recursion edge case 2:
			// We're at the cutoff depth
			return MinMaxValue(evaluate(), Move());
		}

		// We have moves left and we're not at the cutoff depth,
		// So we'll try each possible move and call minimax for
		// each state, then we pick the best one
		float best_value = _current_turn == WHITE ? std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
		Move best_move;

		for (Move& m : moves) {
			State new_state = *this;
			new_state.make_move(m);

			// Recursion step, let's call Minimax for the next step
			MinMaxValue value = new_state.minmax(depth - 1);

			// If we got the best value, then let's store it
			if (_current_turn == WHITE && value._value > best_value) {
				best_value = value._value;
				best_move = m;
			}
			else if (_current_turn == BLACK && value._value < best_value) {
				best_value = value._value;
				best_move = m;
			}
		}

		return MinMaxValue(best_value, best_move);
	}

	// Calculates the value of the pieces (white piece value - black piece value)
	//
	// Pawn = 1
	// Knight = 3
	// Bishop = 3
	// Rook = 5
	// Queen = 9
	float material() const {
		std::map<int, float> piece_values = {
			{wP, 1}, {wN, 3}, {wB, 3}, {wR, 5}, {wQ, 9},
			{bP, -1}, {bN, -3}, {bB, -3}, {bR, -5}, {bQ, -9},
			{NA, 0}
		};

		float value = 0;

		for (int row = 0; row < 8; row++) {
			for (int column = 0; column < 8; column++) {
				int piece = _board[row][column];
				value += piece_values[piece];
			}
		}

		return value;
	}


	// Returns the difference between white's and black's available moves
	float mobility() const {
		std::vector<Move> white_moves;
		std::vector<Move> black_moves;

		give_all_raw_moves(WHITE, white_moves);
		give_all_raw_moves(BLACK, black_moves);

		return (white_moves.size() - black_moves.size());
	}

	float pawn_structure() const {
		float sum = 0.0f;
		const float increment = 0.1f;
		for (int row = 1; row < 7; ++row) {
			for (int column = 1; column < 7; ++column) {
				if (_board[row][column] == wP || _board[row][column] == bP) {
					if (_board[row + 1][column + 1] == _board[row][column]) {
						sum += (_board[row][column] == wP) ? increment : -increment;
					}
					if (_board[row - 1][column + 1] == _board[row][column]) {
						sum += (_board[row][column] == wP) ? increment : -increment;
					}
					if (_board[row - 1][column - 1] == _board[row][column]) {
						sum += (_board[row][column] == wP) ? increment : -increment;
					}
					if (_board[row + 1][column - 1] == _board[row][column]) {
						sum += (_board[row][column] == wP) ? increment : -increment;
					}
					if (_board[row][column - 1] == _board[row][column]) {
						sum += (_board[row][column] == wP) ? increment : -increment;
					}
					if (_board[row][column + 1] == _board[row][column]) {
						sum += (_board[row][column] == wP) ? increment : -increment;
					}
				}
			}
		}
		return sum;
	}

	// Returns the centralization value of white's and black's pieces
	float centralization() const {
		float pointsB[8][8] = {
		{   0,   0,   0,   0,   0,   0,   0,  0 },
		{   0,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,  0 },
		{0.1f,0.1f,0.2f,0.2f,0.2f,0.2f,0.1f,0.1f},
		{0.1f,0.1f,0.2f,0.3f,0.3f,0.2f,0.1f,0.1f},
		{0.1f,0.1f,0.2f,0.3f,0.3f,0.2f,0.1f,0.1f},
		{0.1f,0.1f,0.2f,0.2f,0.2f,0.2f,0.1f,0.1f},
		{   0,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,  0 },
		{   0,   0,   0,   0,   0,   0,   0,  0 }
		};

		float pointsW[8][8] = {
		{   0,   0,   0,   0,   0,   0,   0,  0 },
		{   0,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,  0 },
		{0.1f,0.1f,0.2f,0.2f,0.2f,0.2f,0.1f,0.1f},
		{0.1f,0.1f,0.2f,0.3f,0.3f,0.2f,0.1f,0.1f},
		{0.1f,0.1f,0.2f,0.4f,0.4f,0.2f,0.1f,0.1f},
		{0.2f,0.2f,0.3f,0.5f,0.5f,0.3f,0.2f,0.2f},
		{0.2f,0.3f,0.4f,0.6f,0.6f,0.4f,0.3f,0.2f},
		{0.2f,0.3f,0.4f,0.6f,0.6f,0.4f,0.3f,0.2f}
		};


		float sum = 0;
		for (int row = 0; row < 8; ++row) {
			for (int column = 0; column < 8; ++column) {
				if (_board[row][column] != NA) {
					if (get_piece_color(_board[row][column]) == WHITE) {
						sum += pointsW[7 - row][column];
					}
					else {
						sum -= pointsW[row][column];
					}
				}
			}
		}
		return sum;
	}

	// Check if all the pieces blocking a
	// possible castling are out of the turn 
	// player's king's way. Reward plays that 
	// let the king castle

	float bishop_left() const {
		if (_current_turn == WHITE) {
			if (_board[7][5] != wB) {
				return 100;
			}
			else {
				return 0;
			}
		}
		else {
			if (_board[0][5] != bB) {
				return -100;
			}
			else {
				return 0;
			}
		}
	}

	float pawn_left() const {
		if (_current_turn == WHITE) {
			if (_board[6][4] != wP) {
				return 1000;
			}
			else {
				return 0;
			}
		}
		else {
			if (_board[1][4] != bP) {
				return -1000;
			}
			else {
				return 0;
			}
		}
	}

	float knight_left() const {
		if (_current_turn == WHITE) {
			if (_board[7][6] != wN) {
				return 100;
			}
			else {
				return 0;
			}
		}
		else {
			if (_board[0][6] != bN) {
				return -100;
			}
			else {
				return 0;
			}
		}
	}

	// Returns a value based on the turn player's king's safety
	float king_safety() const
	{
		int row, column;
		int king = _current_turn == WHITE ? wK : bK;
		search_for_piece(king, row, column);
		if ((king == wK) && (row == 7) && (column != 0) && (column != 7)) {
			if (((_board[row][column + 1] == wR) || (_board[row][column - 1] == wR)) && (_board[row - 1][column - 1] == wP) &&
				(_board[row - 1][column] == wP) && (_board[row - 1][column + 1] == wP)) {
				return 100000;
			}
			else { return 0; }
		}
		if ((king == bK) && (row == 0) && (column != 0) && (column != 7)) {
			if (((_board[row][column + 1] == bR) || (_board[row][column - 1] == bR)) && (_board[row + 1][column - 1] == bP) &&
				(_board[row + 1][column] == bP) && (_board[row + 1][column + 1] == bP)) {
				return -100000;
			}
			else { return 0; }
		}
		return 0;
	}


	// Check if it's possible for the turn player to castle
	/// 
	/// \param player	Which player's ability to castle we're checking
	/// \param moves	A vector of all legal moves thus far, all possible castles are added to this
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

	void raw_move_in_direction(int& move_index, int row, int column, int player, int max_steps, bool can_take, bool must_take, std::vector<Move>& moves, int row_delta, int column_delta) const;

	void give_raw_move_rook(int& move_index, int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_knight(int& move_index, int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_bishop(int& move_index, int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_queen(int& move_index, int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_king(int& move_index, int row, int column, int player, std::vector<Move>& moves) const;
	void give_raw_move_pawn(int& move_index, int row, int column, int player, std::vector<Move>& moves) const;

	void set_move_with_index(int& move_index, std::vector<Move>& moves, const Move& new_move) const;

	// Finds a given piece's (for example wK) location on the board
	void search_for_piece(int piece, int& row, int& column) const;

	// Check if the piece in the current square is under threat of capture
	/// 
	/// \param row		The row of the king
	/// \param column	The column of the king
	/// \param threatening_player	A boolean to check if the king should worry about white or black pieces
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
		{bR, bN, bB, bQ, bK, bB, bN, bR},
		{bP, bP, bP, bP, bP, bP, bP, bP},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wP, wP, wP, wP, wP, wP, wP, wP},
		{wR, wN, wB, wQ, wK, wB, wN, wR}
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
	};*/

	/*int _board[8][8] = {
		{NA, NA, NA, bR, NA, NA, bK, NA},
		{bP, NA, NA, NA, NA, bP, bP, NA},
		{wP, bP, NA, NA, NA, bQ, NA, bP},
		{bN, NA, NA, NA, bP, NA, NA, NA},
		{NA, NA, NA, NA, wP, NA, NA, NA},
		{NA, NA, NA, bP, NA, NA, wP, NA},
		{NA, NA, NA, NA, wK, NA, NA, wP},
		{wR, NA, bR, NA, NA, bB, wN, wR}
	};*/

	/*int _board[8][8] = {
		{NA, NA, NA, NA, NA, NA, NA, bK},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{wN, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA},
		{NA, NA, NA, NA, NA, NA, NA, NA}
	};*/

	// Keep track of if white or black is allowed to castle
	// in either direction. True by default
	bool white_short_castling_allowed = true;
	bool white_long_castling_allowed = true;
	bool black_short_castling_allowed = true;
	bool black_long_castling_allowed = true;

	// En passant

};