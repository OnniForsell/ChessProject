#pragma once
#include <string>
#include <iostream>
#include <vector>

// Represent a change in the state of the board
class Move {
public:
	Move(int s_r, int s_c, int e_r, int e_c) :
		_s_r(s_r), _s_c(s_c), _e_r(e_r), _e_c(e_c)
	{}

	// Homework
	// Make a constructor that intializes the move based on a given string, fe. "e2e4" tai "g8f6"
	/* Move(const std::string& m)
	{
		_s_c = m[0] - 'a';
		_s_r = '8' - m[1];
		_e_c = m[2] - 'a';
		_e_r = '8' - m[3];
	}; */

private:

	int _s_r = 0; // The row of the starting square of the move
	int _s_c = 0; // The collumn starting square of the move

	int _e_r = 0; // The row of the ending square of the move
	int _e_c = 0; // The collumn of the ending collumn of the move

	friend class State;
};