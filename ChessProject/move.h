#pragma once
#include <string>
#include <iostream>
#include <vector>

// Represent a change in the state of the board
class Move {
public:
	Move()
	{}

	Move(int s_r, int s_c, int e_r, int e_c, int promotion = NA) :
		_s_r(s_r), _s_c(s_c), _e_r(e_r), _e_c(e_c), _piece_to_promote(promotion)
	{}

	// Homework
	// Make a constructor that intializes the move based on a given string, fe. "e2e4" tai "g8f6"
	Move(const std::string& m)
	{
		_s_c = m[0] - 'a';
		_s_r = '8' - m[1];
		_e_c = m[2] - 'a';
		_e_r = '8' - m[3];

		if (m.size() > 4)
		{
			if (_e_r == 0) {

				switch (m[4])
				{
				case 'Q':
					_piece_to_promote = wQ;
					break;
				case 'R':
					_piece_to_promote = wR;
					break;
				case 'B':
					_piece_to_promote = wB;
					break;
				case 'N':
					_piece_to_promote = wN;
					break;
				}
			}
			else {
				switch (m[4])
				{
				case 'Q':
					_piece_to_promote = bQ;
					break;
				case 'R':
					_piece_to_promote = bR;
					break;
				case 'B':
					_piece_to_promote = bB;
					break;
				case 'N':
					_piece_to_promote = bN;
					break;
				}
			}
		}
	};

	std::string toChessMove() const
	{
		char startFile = 'a' + _s_c;
		char endFile = 'a' + _e_c;

		std::string chessMove = "";
		chessMove += startFile;
		chessMove += std::to_string(8 - _s_r);
		chessMove += endFile;
		chessMove += std::to_string(8 - _e_r);


		if (_piece_to_promote != NA)
		{
			switch (_piece_to_promote)
			{
			case wQ:
				chessMove += 'Q';
				break;
			case wR:
				chessMove += 'R';
				break;
			case wB:
				chessMove += 'B';
				break;
			case wN:
				chessMove += 'N';
				break;
			case bQ:
				chessMove += 'Q';
				break;
			case bR:
				chessMove += 'R';
				break;
			case bB:
				chessMove += 'B';
				break;
			case bN:
				chessMove += 'N';
				break;
			}
		}

		return chessMove;
	}


private:

	int _s_r = 0; // The row of the starting square of the move
	int _s_c = 0; // The column starting square of the move

	int _e_r = 0; // The row of the ending square of the move
	int _e_c = 0; // The column of the ending collumn of the move

	int _piece_to_promote = NA; // fe. wQ, wR, bN etc.

	friend class State;
};