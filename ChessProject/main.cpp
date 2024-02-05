#include <iostream>
#include "state.h"
#include "move.h"


// Checks if the provided move exists in the "moves" vector
bool isValidMove(const std::string& inputMove, const std::vector<Move>& moves) {
	for (const Move& move : moves) {
		if (move.toChessMove() == inputMove) {
			return true;  // Move is valid
		}
	}
	return false;  // Move is not valid
}


int main() {

	// Starting state
	State state;

	std::vector<Move> moves;

	state.give_all_raw_moves(state._current_turn, moves);


	while (moves.size() > 0)
	{
		state.print_board();

		// Print all possible moves in the following format
		// e2e4
		// e2e3
		// etc
		std::cout << "Available moves:" << "\n";
		for (const Move& move : moves) {
			std::string chessMove = move.toChessMove();
			std::cout << chessMove << "\n";
		}
 
		// Print the number of legal moves, fe 20
		std::cout << "Number of legal moves: " << moves.size() << "\n";

		// Ask the player for their next move
		std::string next_move;
		std::cout << "Give your next move: ";
		std::cin >> next_move;

		// Repeat the question until the user inputs a valid move
		while (!isValidMove(next_move, moves)) {
			std::cout << "Invalid move. Please enter a valid move: ";
			std::cin >> next_move;
		}

		state.make_move(next_move);

		moves.clear();
		state.give_all_raw_moves(state._current_turn, moves);
	}

	return 0;
}