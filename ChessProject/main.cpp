#include <iostream>
#include "state.h"
#include "move.h"


// Checks if the provided move exists in the "moves" vector
/// 
/// \param inputMove		The given move that is being analyzed
/// \param moves			An array of the turn player's currently available legal moves
bool isValidMove(const std::string& inputMove, const std::vector<Move>& moves) {
	for (const Move& move : moves) {
		if (move.toChessMove() == inputMove) {
			return true;  // Move is valid
		}
	}
	return false;  // Move is not valid
}


// Helper function for handling a human player's moves
/// 
/// \param state		A State instance containing the current state of the board
/// \param move_history	An array of every move that's been made so far
/// \param moves		An array of the turn player's currently available legal moves
void handlePlayerMove(State& state, std::vector<State>& move_history, std::vector<Move>& moves) {
	std::cout << "Available moves:\n" << "+------+\n";
	for (const Move& move : moves) {
		std::string chessMove = move.toChessMove();
		std::cout << "| " << chessMove << " |\n";
		std::cout << "+------+" << "\n";
	}

	// Print the number of legal moves, fe 20
	std::cout << "Number of legal moves: " << moves.size() << "\n";

	// Ask the player for their next move
	std::string next_move;
	std::cout << "Give your next move: ";

	// Repeat the question until the user inputs a valid move
	std::cin >> next_move;
	if (next_move == "undo") {
		if (move_history.size() > 2) {
			// Remove the 2 latest states
			move_history.pop_back();
			move_history.pop_back();
			state = move_history.back(); // Update state to previous state
			moves.clear();
			state.give_moves(moves);
			return; // Return without making a move
		}
		else {
			std::cout << "Cannot undo further.\n";
			return; // Return without making a move
		}
	}
	else if (!isValidMove(next_move, moves)) {
		std::cout << "Invalid move. Please enter a valid move or type 'undo' to undo: " << "\n";
		handlePlayerMove(state, move_history, moves); // Recursively handle player move
		return;
	}

	state.make_move(next_move);
	move_history.push_back(state);
}

int main() {

	// Starting state
	State state;

	bool is_computer_black = false;
	bool is_computer_white = false;
	std::string black_answer;
	std::string white_answer;

	std::cout << "Is the computer black? ";
	std::cin >> black_answer;
	if (black_answer == "yes") {
		is_computer_black = true;
	}
	std::cout << "Is the computer white? ";
	std::cin >> white_answer;
	if (white_answer == "yes") {
		is_computer_white = true;
	}

	std::vector<State> move_history;
	std::vector<Move> moves;

	moves.reserve(60);


	state.give_moves(moves);
	move_history.push_back(state);

	while (moves.size() > 0)
	{
		state.print_board();

		// std::cout << state.evaluate() << "\n";
		if (state._current_turn == BLACK && is_computer_black) {
			MinMaxValue value = state.alphabeta(4, std::numeric_limits<float>::lowest(), -std::numeric_limits<float>::lowest());
			state.make_move(value._move);
			move_history.push_back(state);
		}
		else if (state._current_turn == WHITE && is_computer_white) {
			MinMaxValue value = state.alphabeta(4, std::numeric_limits<float>::lowest(), -std::numeric_limits<float>::lowest());
			state.make_move(value._move);
			move_history.push_back(state);
		}
		else {
			handlePlayerMove(state, move_history, moves);
		}

		moves.clear();
		state.give_moves(moves);
	}

	state.print_board();
	std::cout << "Game Over" << "\n";

	return 0;
}