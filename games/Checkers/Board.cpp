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

bool Board::move(Cell OldPos, Cell NewPos)
{
    bool bRet = false;

    Cell pieceToRemove;
    if (moveIsValid(OldPos, NewPos, &pieceToRemove))
    {
        Checker c = pieceAt(OldPos);
        // Now check if we should make the piece a King.
        if (c.isWhite() && (NewPos.y == (NUM_SQUARES - 1)))
        {
            c = Checker(sq_white_king);
        }
        else if (c.isBlack() && (NewPos.y == 0))
        {
            c = Checker(sq_black_king);
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

bool Board::inBounds(const Cell& pos) const
{
	if ((pos.x >= 0) && (pos.x < NUM_SQUARES) && (pos.y >= 0) && (pos.y < NUM_SQUARES))
		return true;
	else
		return false;
}

Checker Board::pieceAt(const Cell& pos) const
{
    Checker c;
    if (inBounds(pos)) {
        c = m_BoardGrid[pos.y][pos.x];
    }
    return c;
}

Checker Board::pieceAt(const Cell& pos, int dx, int dy) const
{
    Cell newPos(pos.x + dx, pos.y + dy);
    return pieceAt(newPos);
}

bool Board::isEmpty(const Cell& pos) const
{
    return pieceAt(pos).isEmpty();
}

bool Board::moveIsValid(Cell OldPos, Cell NewPos, Cell* pRemovedPos)
{
    bool isValid = false;

    Checker piece = pieceAt(OldPos);

    const int dx = NewPos.x - OldPos.x;
    const int dy = NewPos.y - OldPos.y;
    const int adx = abs(dx);
    const int ady = abs(dy);
    if ((adx == ady)    // move must be diagonal 
        && (adx > 0)    // must be one or two squares away
        && (adx < 3)
        && (!piece.isEmpty())
        && (isEmpty(NewPos))    // must move a piece to an empty space
        )
    {
        if (validDirection(piece, dy))
        {
            // if (abs)dx == 2, confirm hopped over opponent piece
            if (adx == 2)   // in this case, we also know ady == 2
            {
                Cell middleCell(OldPos.x + (dx / 2), OldPos.y + (dy / 2));
                Checker middlePiece = pieceAt(middleCell);
                if (!middlePiece.isEmpty() && !matchesColor(piece, middlePiece))
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

bool Board::matchesColor(Checker piece, Checker color) const
{
    if ((piece.isWhite() && color.isWhite())
        || (piece.isBlack() && color.isBlack()))
    {
        return true;
    }
    return false;
}

bool Board::validDirection(Checker piece, int yDir) const
{
    if (piece.isKing() || (piece.isWhite() && (yDir > 0)) || (piece.isBlack() && (yDir < 0)))
    {
        return true;
    }
    return false;
}

bool Board::inDanger(const Cell& pos)
{
    bool canBeJumped = false;
    // A   B
    //   C
    // D   E
    Checker a = pieceAt(pos, -1, -1);
    Checker b = pieceAt(pos, 1, -1);
    Checker c = pieceAt(pos);
    Checker d = pieceAt(pos, -1, 1);
    Checker e = pieceAt(pos, 1, 1);

    // Consider 4 jumps: A->E, B->D, D->B, E->A
    if (!a.isEmpty() && e.isEmpty() && !matchesColor(a, c) && validDirection(a, 1)) {
        canBeJumped = true;
    }
    else if (!b.isEmpty() && d.isEmpty() && !matchesColor(b, c) && validDirection(b, 1)) {
        canBeJumped = true;
    }
    else if (!d.isEmpty() && b.isEmpty() && !matchesColor(d, c) && validDirection(d, -1)) {
        canBeJumped = true;
    }
    else if (!e.isEmpty() && a.isEmpty() && !matchesColor(e, c) && validDirection(e, -1)) {
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
            Cell pos(i, j);
            Checker c = pieceAt(pos);

            int p = -1;
            if (c.isWhite())
            {
                p = 0;
            }
            else if (c.isBlack())
            {
                p = 1;
            }

            if (p >= 0)
            {
                m_numPieces[p]++;
                if (c.isKing())
                {
                    m_numKings[p]++;
                }
                if (inDanger(pos))
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
