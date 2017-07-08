#ifndef _CHECKER_H_
#define _CHECKER_H_

typedef enum
{
    sq_empty = 0,
    sq_white,
    sq_white_king,
    sq_black,
    sq_black_king,
} CheckerType;

class Checker
{
public:
    Checker();
    Checker(CheckerType cType);
    Checker(const Checker& other);
    Checker& operator =(const Checker& other);

    CheckerType type();

    bool isEmpty();
    bool isWhite();
    bool isBlack();
    bool isKing();

private:
    CheckerType m_type;
};

#endif
