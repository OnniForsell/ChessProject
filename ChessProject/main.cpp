#include <iostream>
#include "state.h"
#include "move.h"

int main() {

	// Starting state
	State state;
	// state.erase_board();

	Move move(6, 4, 4, 4);
	// Move move("e2e4");
	state.make_move(move);

	state.print_board();


	std::vector<Move> moves;
	state.give_raw_move_rook(7, 0, WHITE, moves);
	state.give_raw_move_bishop(7, 5, WHITE, moves);
	state.give_raw_move_knight(7, 6, WHITE, moves);

	return 0;
}