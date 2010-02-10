#ifndef wali_nwa_Symbol_GUARD
#define wali_nwa_Symbol_GUARD 1

/**
 * @author Amanda Burton
 */

// ::wali
#include "wali/Printable.hpp"
#include "wali/Common.hpp"
#include "wali/Key.hpp"

// ::std
#include <assert.h>

namespace wali
{
  namespace nwa
  {  
    /**
     *
     *  This class is used to label the transitions of an NWA.
     *
     */
    template<typename T = Key>
    class Symbol : public Printable
    {    
      //The types of symbols that are possible:
      //  Ordinary: most will by of this type
      //  Wild: represents 'any' Symbol
      //  Epsilon:  represents 'no' Symbol
      enum Type { Ordinary, Wild, Epsilon };
    
      /**
       *  TODO
       * @brief constructs a wild symbol 
       *
       */                                                                                                            
      //static Symbol& wild( ) 
      //{                              
      //  static Symbol * wild = new Symbol(true);                   
      //  return *wild;                                          
      //}                                                                  
      
      /**
       *  TODO
       * @brief constructs an epsilon symbol
       *
       */
      //static Symbol& epsilon( )
      //{
      //  static Symbol * epsilon = new Symbol(false);
      //  return *epsilon;
      //}
    
      //
      // Methods
      //

      public:
      
      //Constructors and Destructor
      Symbol( );
      Symbol( bool wild );
      Symbol( T lbl );
      Symbol( const Symbol & other );
      Symbol & operator=( const Symbol & other );
  
      virtual ~Symbol( ) { }


      //Accessors
      
      /**
       *
       * @brief access the epsilon symbol
       * 
       * This method returns the epsilon symbol.
       *
       * @return the epsilon symbol
       *
       */
      static Symbol<T> getEpsilon( );
      
      /**
       *  
       * @brief test whether this is the epsilon symbol
       * 
       * This method determines whether this is the epsilon symbol.
       *
       * @return true if this is the epsilon symbol, false otherwise
       *
       */
      bool isEpsilon( ) const;  
      
      /**
       * 
       * @brief access the wild symbol
       *
       * This method returns the wild symbol.
       *
       * @return the wild symbol
       *
       */
      static Symbol<T> getWild( );
      
      /**
       *  
       * @brief test whether this is the wild symbol
       *
       * This method determines whether this is the wild symbol.
       *
       * @return true if this is the wild symbol, false otherwise
       *
       */
      bool isWild() const;      
      
      /** 
       *  TODO: what should wild and epsilon return from this?
       * @brief access the label associated with this symbol
       *
       * This method provides access to the label associated with
       * this symbol.
       * Note: This method should never be called on a wild or epsilon symbol.
       *
       * @return the label associated with this symbol
       *
       */
      T getLabel( ) const;
      
      //Intersection of symbol labels
      /**
       *  TODO: move edgeIntersection back to here!
       * @brief creates the symbol that is the intersection of this symbol with the 
       *        given symbol 'other'
       *
       * This method checks that this symbol and the given symbol 'other' can be 
       * intersected and creates the resulting symbol.  If these two symbols can 
       * be intersected, true is returned and the resulting symbol is passed 
       * back via the address 'result'.  Otherwise, false is returned and no
       * new symbol is created.
       * Note: If some symbol-matching metric other than strict label equality
       *       is desired, this method will need to be overridden.
       * 
       * @param - other: the symbol to intersect with this symbol
       * @param - result: the  address to use in passing back the joint symbol created
       *                when intersection is possible 
       * @result true if the two symbols can be intersected, false otherwise
       *
       */
      /*virtual bool intersect( Symbol * other, Symbol & result )
      {
        //Note: When overriding this method your metric must determine an
        // appropriate label, create a symbol with that label, and set result
        // to the symbol just created.
      
        if( isWild() )  //If we have a wild symbol, whatever the other symbol is survives (even if it is also wild).
        {
          result = *other;
          return true;
        }
        else if( other->isWild() ) //If the other symbol is wild, whatever this symbol is survives. 
        {
          result = *this;
          return true;
        }
        else if( isEpsilon() )  //If we have an epsilon, only another epsilon can match.
        {
          return other->isEpsilon();
        }
        else if( lbl == other->lbl )  //For a general symbol, the labels must be equal for the symbols to match. 
        {
          result = *this;
          return true;
        }
        else
          return false;
      }*/
      
      //Utilities
      
      /** 
       *
       * @brief print the Symbol
       *
       * This method prints out the Symbol to the output stream provided.
       * Note: T must have a print function.
       *
       * @param - o: the output stream to which to print the Symbol
       * @return the output stream to which the Symbol was printed
       *
       */
      std::ostream & print( std::ostream & o ) const;
      
      /** 
       *
       * @brief tests whether this Symbol is equivalent to the Symbol 'other'.
       *
       * This method tests the equivalence of this Symbol and the Symbol
       * 'other'.
       *
       * @param - other: the Symbol to compare this Symbol to
       * @return true if this Symbol is equivalent to the Symbol 'other', false otherwise
       *
       */
      bool operator==( const Symbol & other ) const;
      
      /**
       *
       * @brief tests the relationship between this Symbol and the Symbol 
       *        'other'.
       *
       * This method tests whether this Symbol is 'less than' the Symbol
       * 'other' in some way.  The default is to order the Symbols based
       * on the ordering of their labels.
       * Note: Wild is less than all symbols, Epsilon is greater than all symbols.
       *
       * @param - rhs: the Symbol to compare this Symbol to
       * @return true if this Symbol is 'less than' the Symbol 'other', false otherwise
       *
       */
      bool operator<( const Symbol & rhs ) const;

      //
      // Variables
      //
      
      protected:
      
      T lbl;
      Type symbolType;
    };

    
    //Constructors
    template <typename T>
    Symbol<T>::Symbol( )
    {
      lbl = T(); 
      symbolType = Wild;
    }
    
    template <typename T>
    Symbol<T>::Symbol( bool wild )
    {
      lbl = T();
      if( wild )
        symbolType = Wild;
      else
        symbolType = Epsilon; 
    }
    
    template <typename T>
    Symbol<T>::Symbol( T lbl )
    {
      this->lbl = lbl;
      symbolType = Ordinary; 
    }

    /* TODO */
    template <typename T>
    Symbol<T>::Symbol( const Symbol & other )
    {
      //if( other.isWild() )
      //  *this = wild;
      //else if( other.isEpsilon() )
      //  *this = epsilon;
      //else
      //{
        lbl = other.lbl;
        symbolType = other.symbolType;
      //}
    }

    /* TODO */
    template <typename T>
    Symbol<T> & Symbol<T>::operator=( const Symbol & other )
    {
      if( this == &other )     
        return *this;
      //if( other.isWild() )
      //  *this = wild;
      //else if( other.isEpsilon() )
      //  *this = epsilon;
      //else
      //{
        lbl = other.lbl;
        symbolType = other.symbolType;
      //}
      return *this;
    }
    
    /**
     *  TODO
     * @brief access the epsilon symbol
     * 
     * @return the epsilon symbol
     *
     */
    template <typename T>
    Symbol<T> Symbol<T>::getEpsilon( )
    {
      return Symbol(false);
      //return &epsilon;
    }
    
    /**
     *
     * @brief test whether this is the epsilon symbol
     *
     * @return true if this is the epsilon symbol, false otherwise
     *
     */
    template <typename T>
    bool Symbol<T>::isEpsilon( ) const
    {
      return symbolType == Epsilon;
    }  
    
    /**
     * 
     * @brief access the wild symbol
     *
     * @return the wild symbol
     *
     */
    template <typename T>
    Symbol<T> Symbol<T>::getWild( )
    {
      return Symbol();
    }
    
    /**
     *  
     * @brief test whether this is the wild symbol
     *
     * @return true if this is the wild symbol, false otherwise
     *
     */
    template <typename T>
    bool Symbol<T>::isWild( ) const
    {
      return symbolType == Wild;
    }
    
    /** 
     *
     * @brief access the label associated with this symbol
     *
     * @return the label associated with this symbol
     *
     */
    template <typename T>
    typename T Symbol<T>::getLabel( ) const
    {
      assert(!isWild() && !isEpsilon());
      return lbl;
    }
    
    /** 
     *
     * @brief print the Symbol
     *
     * @param - o: the output stream to which to print the Symbol
     * @return the output stream to which the Symbol was printed
     *
     */
    template <typename T>
    std::ostream & Symbol<T>::print( std::ostream & o ) const
    {
      if( isWild() )
        o << "wild";
      else if( isEpsilon() )
        o << "epsilon";
      else
        lbl.print(o); 
        //o << lbl; //TODO: restore this!
      
      return o;
    }
    
    /** 
     *
     * @brief tests whether this Symbol is equivalent to the Symbol 'other'.
     *
     * @param - other: the Symbol to compare this Symbol to
     * @return true if this Symbol is equivalent to the Symbol 'other', false otherwise
     *
     */
    template <typename T>
    bool Symbol<T>::operator==( const Symbol & other ) const
    {
      if( isWild() )
        return other.isWild();
      else if( other.isWild() )
        return false;  
      else if( isEpsilon() )
        return other.isEpsilon();
      else if( other.isEpsilon() )
        return false;
      else 
        return ( lbl == other.lbl );
    }
    
    /**
     *
     * @brief tests the relationship between this Symbol and the Symbol 'other'.
     *
     * @param - rhs: the Symbol to compare this Symbol to
     * @return true if this Symbol is 'less than' the Symbol 'other', false otherwise
     *
     */
    template <typename T>
    bool Symbol<T>::operator<( const Symbol & rhs ) const
    {
      if( operator==(rhs) )
        return false;
        
      // invariant: not equal
      
      if( isWild() ) 
        return true;
      if( rhs.isWild() ) 
        return false;      
         
      if( rhs.isEpsilon() )
        return true;
      if( isEpsilon() )
        return false;
        
      return lbl<rhs.lbl;
    }
  }
}
#endif