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
	// state.erase_board();

	// Move move(6, 4, 4, 4);
	//Move move("e2e4");
	//state.make_move(move);

	state.print_board();

	std::vector<Move> moves;
	//state.give_raw_move_rook(7, 0, WHITE, moves);
	//state.give_raw_move_bishop(7, 5, WHITE, moves);
	//state.give_raw_move_knight(7, 6, WHITE, moves);


	state.give_all_raw_moves(state._current_turn, moves);


	while (moves.size() > 0)
	{
		state.print_board();

		// TODO 1
		// Print all possible moves in the following format
		// e2e4
		// e2e3
		// etc
		std::cout << "Available moves:" << "\n";
		for (const Move& move : moves) {
			std::string chessMove = move.toChessMove();
			std::cout << chessMove << "\n";
		}
 
		// TODO 2
		// Print the number of legal moves, fe 20
		std::cout << "Number of legal moves: " << moves.size() << "\n";

		// TODO 3
		// Ask the player for a move, for example "e2e4" (If the move is a promotion, then it should be something like "a7a8Q")
		// Make a Move class instance and call the move function
		// Refuse any moves not listed in the moves list

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