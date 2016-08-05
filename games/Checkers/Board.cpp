// Board.cpp

#include <Windows.h>
#include "Board.h"

Board::Board()
{
    // set the game pieces
    ZeroMemory(&m_BoardGrid, sizeof(m_BoardGrid));
    for (int j = 0; j < NUM_SQUARES; j++)
    {
        for (int i = 0; i < NUM_SQUARES; i++)
        {
            if ((j % 2) == 0)
            {
                if ((i % 2) == 1)
                {
                    if (j < 3)
                        m_BoardGrid[j][i] = sq_white;
                    if (j >= 5)
                        m_BoardGrid[j][i] = sq_black;
                }
            }
            else
            {
                if ((i % 2) == 0)
                {
                    if (j < 3)
                        m_BoardGrid[j][i] = sq_white;
                    if (j >= 5)
                        m_BoardGrid[j][i] = sq_black;
                }
            }
        }
    }
}

bool Board::Move(Cell OldPos, Cell NewPos)
{
    bool bRet = false;

    Cell pieceToRemove;
    if (MoveIsValid(OldPos, NewPos, &pieceToRemove))
    {
        Checker c = pieceAt(OldPos.x, OldPos.y);
        // Now check if we should make the piece a King.
        if (isWhite(c) && (NewPos.y == (NUM_SQUARES - 1)))
        {
            c = sq_white_king;
        }
        else if (isBlack(c) && (NewPos.y == 0))
        {
            c = sq_black_king;
        }

        m_BoardGrid[OldPos.y][OldPos.x] = sq_empty;
        m_BoardGrid[NewPos.y][NewPos.x] = c;

        // Now remove any pieces we may have jumped over.
        if ((pieceToRemove.x >= 0) && (pieceToRemove.y >= 0))
        {
            m_BoardGrid[pieceToRemove.y][pieceToRemove.x] = sq_empty;
        }

        checkVictoryConditions();

        bRet = true;
	}

	return bRet;
}

bool Board::InBounds(Cell Pos)
{
	if ((Pos.x >= 0) && (Pos.x < NUM_SQUARES) && (Pos.y >= 0) && (Pos.y < NUM_SQUARES))
		return true;
	else
		return false;
}

Checker Board::pieceAt(int x, int y)
{
    Checker c = sq_empty;
    Cell pos(x, y);

    if (InBounds(pos)) {
        c = m_BoardGrid[y][x];
    }
    return c;
}

bool Board::MoveIsValid(Cell OldPos, Cell NewPos, Cell* pRemovedPos)
{
    bool isValid = false;

    Checker piece = pieceAt(OldPos.x, OldPos.y);

    const int dx = NewPos.x - OldPos.x;
    const int dy = NewPos.y - OldPos.y;
    const int adx = abs(dx);
    const int ady = abs(dy);
    if ((adx == ady)    // move must be diagonal 
        && (adx > 0)    // must be one or two squares away
        && (adx < 3)
        && (piece != sq_empty)                           // must move a piece to an empty space
        && (isEmpty(NewPos.x, NewPos.y))
        )
    {
        if (validDirection(piece, dy))
        {
            // if (abs)dx == 2, confirm hopped over opponent piece
            if (adx == 2)   // in this case, we also know ady == 2
            {
                Cell middleCell(OldPos.x + (dx / 2), OldPos.y + (dy / 2));
                Checker middlePiece = pieceAt(middleCell.x, middleCell.y);
                if ((middlePiece != sq_empty) && !matchesColor(piece, middlePiece))
                {
                    if (pRemovedPos)
                    {
                        // mark the piece that was jumped over to be removed
                        pRemovedPos->x = middleCell.x;
                        pRemovedPos->y = middleCell.y;
                    }
                    isValid = true;
                }
            }
            else
            {
                isValid = true;
            }
        }
    }

    return isValid;
}

bool Board::isEmpty(int x, int y)
{
    return (pieceAt(x,y) == sq_empty);
}

bool Board::isEmpty(Cell Pos)
{
    return isEmpty(Pos.x, Pos.y);
}

bool Board::isEmpty(Checker c)
{
    return (c == sq_empty);
}

bool Board::isWhite(Checker c)
{
    if ((c == sq_white) || (c == sq_white_king))
    {
        return true;
    }
    return false;
}

bool Board::isBlack(Checker c)
{
    if ((c == sq_black) || (c == sq_black_king))
    {
        return true;
    }
    return false;
}

bool Board::isKing(Checker c)
{
    if ((c == sq_white_king) || (c == sq_black_king))
    {
        return true;
    }
    return false;
}

bool Board::isWhite(Cell Pos)
{
    return isWhite(Pos.x, Pos.y);
}

bool Board::isWhite(int x, int y)
{
    return isWhite(pieceAt(x, y));
}

bool Board::isKing(Cell Pos)
{
    return isKing(Pos.x, Pos.y);
}

bool Board::isKing(int x, int y)
{
    return isKing(pieceAt(x,y));
}

bool Board::matchesColor(Checker piece, Checker color)
{
    if (   (isWhite(piece) && isWhite(color))
        || (isBlack(piece) && isBlack(color)))
    {
        return true;
    }
    return false;
}

bool Board::validDirection(Checker piece, int yDir)
{
    if (isKing(piece) || (isWhite(piece) && (yDir > 0)) || (isBlack(piece) && (yDir < 0))) 
    {
        return true;
    }
    return false;
}

bool Board::inDanger(int x, int y)
{
    bool canBeJumped = false;
    // A   B
    //   C
    // D   E
    Checker a = pieceAt(x-1, y-1);
    Checker b = pieceAt(x+1, y-1);
    Checker c = pieceAt(x, y);
    Checker d = pieceAt(x-1, y+1);
    Checker e = pieceAt(x+1, y+1);

    // Consider 4 jumps: A->E, B->D, D->B, E->A
    if        ((a != sq_empty) && (e == sq_empty) && !matchesColor(a, c) && validDirection(a, 1)) {
        canBeJumped = true;
    } else if ((b != sq_empty) && (d == sq_empty) && !matchesColor(b, c) && validDirection(b, 1)) {
        canBeJumped = true;
    } else if ((d != sq_empty) && (b == sq_empty) && !matchesColor(d, c) && validDirection(d, -1)) {
        canBeJumped = true;
    } else if ((e != sq_empty) && (a == sq_empty) && !matchesColor(e, c) && validDirection(e, -1)) {
        canBeJumped = true;
    }
 
    return canBeJumped;
}

bool Board::cellIsWhite(int x, int y)
{
    return ((x % 2) == (y % 2));
}

void Board::countPieces()
{
    for (int p = 0; p < 2; ++p)
    {
        m_numPieces[p] = 0;
        m_numKings[p] = 0;
        m_numDangers[p] = 0;
    }
    for (int j = 0; j < NUM_SQUARES; j++)
    {
        for (int i = 0; i < NUM_SQUARES; i++)
        {
            Checker c = pieceAt(i, j);

            int p = -1;
            if (isWhite(c))
            {
                p = 0;
            }
            else if (isBlack(c))
            {
                p = 1;
            }

            if (p >= 0)
            {
                m_numPieces[p]++;
                if (isKing(c))
                {
                    m_numKings[p]++;
                }
                if (inDanger(i, j))
                {
                    m_numDangers[p]++;
                }
            }
        }
    }
}

bool Board::isWinner(int player)
{
    return (m_numPieces[!player] == 0);
}

int Board::score(int player)
{
    int score = m_numPieces[player] - m_numPieces[!player] 
                + (3 * m_numKings[player]) - (2 * m_numKings[!player])
                + (6 * m_numDangers[!player]) - (6 * m_numDangers[player]);
    if (m_numPieces[!player] == 0)
    {
        score += 100;
    }
    if (m_numPieces[player] == 0)
    {
        score -= 100;
    }
    return score;
}

void Board::checkVictoryConditions()
{
    countPieces();

    if (isWinner(WHITE_PLAYER) || isWinner(BLACK_PLAYER))
    {
        m_gameOver = true;
    }
}
