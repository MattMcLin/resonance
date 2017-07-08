#include "checker.h"

Checker::Checker()
    : m_type(sq_empty)
{
}

Checker::Checker(CheckerType cType)
    : m_type(cType)
{
}

Checker::Checker(const Checker& other)
    : m_type(other.m_type)
{
}

Checker& Checker::operator=(const Checker& other)
{
    if (this != &other)
    {
        m_type = other.m_type;
    }

    return *this;
}

CheckerType Checker::type()
{
    return m_type;
}

bool Checker::isEmpty()
{
    return (m_type == sq_empty);
}

bool Checker::isWhite()
{
    if ((m_type == sq_white) || (m_type == sq_white_king))
    {
        return true;
    }
    return false;
}

bool Checker::isBlack()
{
    if ((m_type == sq_black) || (m_type == sq_black_king))
    {
        return true;
    }
    return false;
}

bool Checker::isKing()
{
    if ((m_type == sq_white_king) || (m_type == sq_black_king))
    {
        return true;
    }
    return false;
}
