/*!
 * $Id: Reach.cpp,v 1.2 2005/06/10 02:10:15 kidd Exp $
 *
 * @author Nick Kidd
 * @version $Revision: 1.2 $
 */

#include "Reach.hpp"

int Reach::numReaches = 0;

Reach::Reach( bool b ) :
    isreached(b)
{
    numReaches++;
    //std::cerr << "Reach(...) :" << numReaches << std::endl;
}

Reach::~Reach()
{
    numReaches--;
    //std::cerr << "~Reach()   :" << numReaches << std::endl;
}

sem_elem_t Reach::one() const
{
    static sem_elem_t O(new Reach(true));
    return O;
}

sem_elem_t Reach::zero() const
{
    static sem_elem_t Z(new Reach(false));
    return Z;
}

// zero is the annihilator for extend
sem_elem_t Reach::extend( SemElem* se )
{
    Reach* rhs = static_cast< Reach* >(se);
    return new Reach(isreached && rhs->isreached);
}

// zero is neutral for combine
sem_elem_t Reach::combine( SemElem* se )
{
    Reach* rhs = static_cast< Reach* >(se);
    return new Reach(isreached || rhs->isreached);
}

bool Reach::equal( SemElem* se ) const
{
    Reach* rhs = static_cast< Reach* >(se);
    return ( isreached == rhs->isreached );
}

std::ostream & Reach::print( std::ostream & o ) const
{
    return (isreached) ? o << "ONE" : o << "ZERO";
}

sem_elem_t Reach::from_string( const std::string& s ) const {
    return (s == "ONE") ? one() : zero();
}
/*
 * $Log $
 */

/* Yo, Emacs!
   ;;; Local Variables: ***
   ;;; tab-width: 4 ***
   ;;; End: ***
 */

