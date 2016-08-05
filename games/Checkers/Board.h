// Board.h

const int NUM_SQUARES = 8;  // board width & height in squares
const int NUM_PIECES = 12;  // initial checkers per color

const int WHITE_PLAYER = 0;
const int BLACK_PLAYER = 1;

typedef enum
{
	sq_empty = 0,
	sq_white,
	sq_white_king,
	sq_black,
	sq_black_king,
} Checker;

class Cell
{
public:
    Cell() :
        x(-1), y(-1) {}
    Cell(int nx, int ny) :
        x(nx), y(ny) {}
	int x,y;
};

class Board
{
public:
	Board();

	bool Move(Cell OldPos, Cell NewPos);
	bool MoveIsValid(Cell OldPos, Cell NewPos, Cell* RemovedPos);
	bool InBounds(Cell Pos);
    Checker pieceAt(int x, int y);

    bool isEmpty(Checker c);
    bool isWhite(Checker c);
    bool isBlack(Checker c);
    bool isKing(Checker c);

    bool isEmpty(int x, int y);
    bool isEmpty(Cell Pos);
    bool isWhite(Cell Pos);         // checker piece is white
    bool isWhite(int x, int y);
    bool isKing(Cell Pos);
    bool isKing(int x, int y);

    bool matchesColor(Checker piece, Checker color);
    bool validDirection(Checker piece, int yDir);

    bool inDanger(int x, int y);    // a piece at this position is in danger of being jumped

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
