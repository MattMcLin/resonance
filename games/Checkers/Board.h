#ifndef _BOARD_H_
#define _BOARD_H_

#include "checker.h"

const int NUM_SQUARES = 8;  // board width & height in squares
const int NUM_PIECES = 12;  // initial checkers per color

const int WHITE_PLAYER = 0;
const int BLACK_PLAYER = 1;

class Cell
{
public:
    Cell() :
        x(-1), y(-1) {}
    Cell(int nx, int ny) :
        x(nx), y(ny) {}
	int x,y;
};

/**
 * @brief The board itself, as well as "supervision" of the board.
 * This class acts as a referee and helper for the board.
 */
class Board
{
public:
	Board();

	bool move(Cell OldPos, Cell NewPos);
	bool moveIsValid(Cell OldPos, Cell NewPos, Cell* RemovedPos);
	bool inBounds(const Cell& pos) const;
    Checker pieceAt(const Cell& pos) const;
    Checker pieceAt(const Cell& pos, int dx, int dy) const;
    bool isEmpty(const Cell& pos) const;

    bool matchesColor(Checker piece, Checker color) const;
    bool validDirection(Checker piece, int yDir) const;

    bool inDanger(const Cell& pos);    // a piece at this position is in danger of being jumped

    bool cellIsWhite(int x, int y); // cell background is white

    void countPieces();
    bool gameOver() { return m_gameOver; }
    bool isWinner(int player);
    int score(int player);

private:
    void checkVictoryConditions();

	int m_ValidSquares[NUM_SQUARES][NUM_SQUARES];
	Checker m_BoardGrid[NUM_SQUARES][NUM_SQUARES];
    bool m_gameOver = false;
    int m_numPieces[2] = { 0 };
    int m_numKings[2] = { 0 };
    int m_numDangers[2] = { 0 };
};

#endif
