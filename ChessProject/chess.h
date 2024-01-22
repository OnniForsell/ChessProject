#pragma once

// Players
enum{WHITE, BLACK};

// Constant values for the different pieces
// R = Rook, N = Knight, B = Bishop, Q = Queen, K = King, P = Pawn, NA = Empty Space
enum { wR, wN, wB, wQ, wK, wP, bR, bN, bB, bQ, bK, bP, NA}; 

int get_piece_color(int piece);