#include "ProgramBddContext.hpp"

#include "glog/logging.h"
#include "buddy/fdd.h"

#include <cstdlib>
#include <ctime>

using namespace std;
using namespace  wali::domains::binrel;

// ////////////////////////////////////////
//     TRASH IN --- TRASH OUT            //
// ////////////////////////////////////////

using namespace wali::domains::binrel;

namespace wali
{
  namespace domains
  {
    namespace binrel
    {
      extern RevBddContext idx2Name;
    }
  }
}
// ////////////////////////////////////////////////////////////////////////////
// The interface assumes that the google logging library is initialized before
// calling any of its functions.

ProgramBddContext::~ProgramBddContext()
{
  regAInfo = regBInfo = NULL;
  BinRel::reset();
}

void ProgramBddContext::addBoolVar(std::string name)
{
  addIntVar(name,2);
}

void ProgramBddContext::addIntVar(std::string name, unsigned siz)
{
  BddContext::addIntVar(name,siz);
  if(siz > maxSize){
    if(maxSize == 0){
      //This is when we *create* the extra levels needed
      int domains[1] = {siz+1};
      int retSizeInfo = fdd_extdomain(domains,1);
      if(retSizeInfo < 0)
        LOG(FATAL) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(retSizeInfo) << "\"" << endl
          << "    Aborting." << endl;
      else
        sizeInfo = (unsigned) retSizeInfo;
      //To pretty print during testing, we add some names for this regsiter size fdd level.
      idx2Name[sizeInfo] = "__regSize";
      //Now create two extra bdd levels, and the corresponding BddInfo entries to be used for manipulation
      //inside ProgramBddContext.
      //We will create indices such that we get a default variable ordering where
      //baseLhs, baseRhs, baseExtra are mixed.
      {
        int domains2[3] = {siz, siz, siz};
        regAInfo = new BddInfo();
        //Create fdds for base
        int base = fdd_extdomain(domains2,3);
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;

        regAInfo->baseLhs = (unsigned) base;
        regAInfo->baseRhs = (unsigned) base + 1;
        regAInfo->baseExtra = (unsigned) base + 2;

        //To pretty print during testing, we add some names for this extra register
        //Currently, idx2Name is a global variable in wali::domains::binrel
        idx2Name[regAInfo->baseLhs] = "__regA";
        idx2Name[regAInfo->baseRhs] = "__regA'";
        idx2Name[regAInfo->baseExtra] = "__regA''";
      }
      {
        int domains2[3] = {siz, siz, siz};
        regBInfo = new BddInfo();
        //Create fdds for base
        int base = fdd_extdomain(domains2,3);
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        regBInfo->baseLhs = (unsigned) base;
        regBInfo->baseRhs = (unsigned )base + 1;
        regBInfo->baseExtra = (unsigned) base + 2;

        //To pretty print during testing, we add some names for this extra register
        //Currently, idx2Name is a global variable in wali::domains::binrel
        idx2Name[regBInfo->baseLhs] = "__regB";
        idx2Name[regBInfo->baseRhs] = "__regB'";
        idx2Name[regBInfo->baseExtra] = "__regB''";
      }
    }else{
      //This is when we *enlarge* the extra levels needed
      int maxVal = siz - maxSize;

      int domains[1] = {maxVal};
      int retSizeInfo = fdd_extdomain(domains,1);
      if(retSizeInfo < 0)
        LOG(FATAL) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(retSizeInfo) << "\"" << endl
          << "    Aborting." << endl;
      retSizeInfo = fdd_overlapdomain(sizeInfo,retSizeInfo);
      if(retSizeInfo < 0)
        LOG(FATAL) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(retSizeInfo) << "\"" << endl
          << "    Aborting." << endl;
      sizeInfo = retSizeInfo;

      //To pretty print during testing, we add some names for this regsiter size fdd level.
      idx2Name[sizeInfo] = "__regSize";
      //Now create two extra bdd levels, and the corresponding BddInfo entries to be used for manipulation
      //inside ProgramBddContext.
      //We will create indices such that we get a default variable ordering where
      //baseLhs, baseRhs, baseExtra are mixed.
      {
        int domains2[3] = {maxVal, maxVal, maxVal};
        //Create fdds for base
        int base = fdd_extdomain(domains2,3);
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        int retbase;
        retbase = fdd_overlapdomain(regAInfo->baseLhs,base);
        regAInfo->baseLhs = retbase;
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        retbase = fdd_overlapdomain(regAInfo->baseRhs,base+1);
        regAInfo->baseRhs = retbase;
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        retbase = fdd_overlapdomain(regAInfo->baseExtra,base+2);
        regAInfo->baseExtra = retbase;
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        //To pretty print during testing, we add some names for this extra register
        //Currently, idx2Name is a global variable in wali::domains::binrel
        idx2Name[regAInfo->baseLhs] = "__regA";
        idx2Name[regAInfo->baseRhs] = "__regA'";
        idx2Name[regAInfo->baseExtra] = "__regA''";
      }
      {
        int domains2[3] = {maxVal, maxVal, maxVal};
        //Create fdds for base
        int base = fdd_extdomain(domains2,3);
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        int retbase;
        retbase = fdd_overlapdomain(regBInfo->baseLhs,base);
        regBInfo->baseLhs = retbase;
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        retbase = fdd_overlapdomain(regBInfo->baseRhs,base+1);
        regBInfo->baseRhs = retbase;
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        retbase = fdd_overlapdomain(regBInfo->baseExtra,base+2);
        regBInfo->baseExtra = retbase;
        if (base < 0)
          LOG(ERROR) << "[ERROR-BuDDy initialization] \"" << bdd_errstring(base) << "\"" << endl
            << "    Aborting." << endl;
        //To pretty print during testing, we add some names for this extra register
        //Currently, idx2Name is a global variable in wali::domains::binrel
        idx2Name[regBInfo->baseLhs] = "__regB";
        idx2Name[regBInfo->baseRhs] = "__regB'";
        idx2Name[regBInfo->baseExtra] = "__regB''";
      }
    }
    maxSize = siz;
  }
}

ProgramBddContext::ProgramBddContext(int bddMemSize, int cacheSize) :
  BddContext(bddMemSize, cacheSize),
  sizeInfo(0),
  maxSize(0),
  regAInfo(NULL),
  regBInfo(NULL)
{}

ProgramBddContext::ProgramBddContext(const ProgramBddContext& other) :
  BddContext(other),
  sizeInfo(other.sizeInfo),
  maxSize(other.maxSize),
  regAInfo(other.regAInfo),
  regBInfo(other.regBInfo)
{}

ProgramBddContext& ProgramBddContext::operator = (const ProgramBddContext& other)
{
  if(this != &other){
    BddContext::operator=(other);
    sizeInfo=other.sizeInfo;
    maxSize=other.maxSize;
    regAInfo=other.regAInfo;
    regBInfo=other.regBInfo;
  }
  return *this;
}

std::ostream& ProgramBddContext::print(std::ostream& o)
{
  o << "ProgramBddContext dump:" << std::endl;
  o << "sizeInfo: " << sizeInfo << std::endl;
  o << "maxSize: " << maxSize << std::endl;
  regAInfo->print(o << "regAInfo: ") << std::endl;
  regBInfo->print(o << "regBInfo: ") << std::endl;
  return o;
}

bdd ProgramBddContext::From(std::string var)
{
  //TODO: This header should be placed in all functions below.
  bdd ret = bddfalse;
  ProgramBddContext::const_iterator iter = (this->find(var));
  if(iter == this->end())
    LOG(FATAL) << "Attempted From () on \"" << var << "\". I don't recognize this name\n";
  const bddinfo_t bi = iter->second;
  for(unsigned i = 0; i < bi->maxVal; ++i)
    ret = ret | (fdd_ithvar(bi->baseLhs, i) & fdd_ithvar(regAInfo->baseRhs, i));
  return ret & fdd_ithvar(sizeInfo, bi->maxVal);
}

bdd ProgramBddContext::True()
{
  return fdd_ithvar(regAInfo->baseRhs,1) & fdd_ithvar(sizeInfo, 2);
}

bdd ProgramBddContext::False()
{
  return fdd_ithvar(regAInfo->baseRhs,0) & fdd_ithvar(sizeInfo, 2);
}

bdd ProgramBddContext::Const(unsigned val)
{
  if(val >= maxSize){
    LOG(ERROR) << "[Const] Attempted to create a constant value larger "
      << "than maxVal";
    return bddfalse;
  }
  return fdd_ithvar(regAInfo->baseRhs, val) & fdd_ithvar(sizeInfo, maxSize);
}

bdd ProgramBddContext::NonDet()
{
  return bddtrue;
}

bdd ProgramBddContext::applyBinOp(bdd lexpr, bdd rexpr, bdd op)
{ 
  bddPair *regA2regB = bdd_newpair();
  fdd_setpair(regA2regB, regAInfo->baseRhs, regBInfo->baseRhs);
  rexpr = bdd_replace(rexpr, regA2regB);
  bdd_freepair(regA2regB);

  lexpr = lexpr & rexpr;

  //IMP: fddsets are unioned by & of the representing bdds (Not | as I had
  //earlier expected.
  lexpr = bdd_relprod(
      lexpr,
      op, 
      fdd_ithset(regAInfo->baseRhs) & fdd_ithset(regBInfo->baseRhs)
      );

  bddPair *regAExtra2Rhs = bdd_newpair();
  fdd_setpair(regAExtra2Rhs, regAInfo->baseExtra, regAInfo->baseRhs);
  lexpr = bdd_replace(lexpr, regAExtra2Rhs);
  bdd_freepair(regAExtra2Rhs);

  return lexpr;
}

bdd ProgramBddContext::applyUnOp(bdd expr, bdd op)
{
  expr = bdd_relprod(
      expr,
      op,
      fdd_ithset(regAInfo->baseRhs)
      );

  bddPair *regAExtra2Rhs = bdd_newpair();
  fdd_setpair(regAExtra2Rhs, regAInfo->baseExtra, regAInfo->baseRhs);
  expr = bdd_replace(expr, regAExtra2Rhs);
  bdd_freepair(regAExtra2Rhs);

  return expr;
}

bdd ProgramBddContext::And(bdd lexpr, bdd rexpr)
{
  return applyBinOp(lexpr, rexpr, bddAnd());      
}

bdd ProgramBddContext::Or(bdd lexpr, bdd rexpr)
{
  return applyBinOp(lexpr, rexpr, bddOr());
}

bdd ProgramBddContext::Not(bdd expr)
{
  return applyUnOp(expr, bddNot());      
}

bdd ProgramBddContext::Plus(bdd lexpr, bdd rexpr)
{
  unsigned in1 = getRegSize(lexpr);
  unsigned in2 = getRegSize(rexpr);
  return applyBinOp(lexpr, rexpr, bddPlus(in1,in2));
}

bdd ProgramBddContext::Minus(bdd lexpr, bdd rexpr)
{
  unsigned in1 = getRegSize(lexpr);
  unsigned in2 = getRegSize(rexpr);
  return applyBinOp(lexpr, rexpr, bddMinus(in1,in2));
}

bdd ProgramBddContext::Times(bdd lexpr, bdd rexpr)
{
  unsigned in1 = getRegSize(lexpr);
  unsigned in2 = getRegSize(rexpr);
  return applyBinOp(lexpr, rexpr, bddTimes(in1,in2));
}

bdd ProgramBddContext::Div(bdd lexpr, bdd rexpr)
{
  unsigned in1 = getRegSize(lexpr);
  unsigned in2 = getRegSize(rexpr);
  return applyBinOp(lexpr, rexpr, bddDiv(in1,in2));
}

bdd ProgramBddContext::bddAnd()
{
  bdd ret =
    (fdd_ithvar(regAInfo->baseRhs,1) & 
     fdd_ithvar(regBInfo->baseRhs,1) &
     fdd_ithvar(regAInfo->baseExtra,1))
    |
    (fdd_ithvar(regAInfo->baseRhs,1) & 
     fdd_ithvar(regBInfo->baseRhs,0) &
     fdd_ithvar(regAInfo->baseExtra,0))
    |
    (fdd_ithvar(regAInfo->baseRhs,0) & 
     fdd_ithvar(regBInfo->baseRhs,1) &
     fdd_ithvar(regAInfo->baseExtra,0))
    |
    (fdd_ithvar(regAInfo->baseRhs,0) & 
     fdd_ithvar(regBInfo->baseRhs,0) &
     fdd_ithvar(regAInfo->baseExtra,0));
  ret = ret & fdd_ithvar(sizeInfo, 2);
  return ret;
}

bdd ProgramBddContext::bddOr()
{
  bdd ret = 
    (fdd_ithvar(regAInfo->baseRhs,1) & 
     fdd_ithvar(regBInfo->baseRhs,1) &
     fdd_ithvar(regAInfo->baseExtra,1))
    |
    (fdd_ithvar(regAInfo->baseRhs,1) & 
     fdd_ithvar(regBInfo->baseRhs,0) &
     fdd_ithvar(regAInfo->baseExtra,1))
    |
    (fdd_ithvar(regAInfo->baseRhs,0) & 
     fdd_ithvar(regBInfo->baseRhs,1) &
     fdd_ithvar(regAInfo->baseExtra,1))
    |
    (fdd_ithvar(regAInfo->baseRhs,0) & 
     fdd_ithvar(regBInfo->baseRhs,0) &
     fdd_ithvar(regAInfo->baseExtra,0));
  ret = ret & fdd_ithvar(sizeInfo, 2);
  return ret;
}

bdd ProgramBddContext::bddNot()
{
  bdd ret =
    (fdd_ithvar(regAInfo->baseRhs,1) & 
     fdd_ithvar(regAInfo->baseExtra,0))
    |
    (fdd_ithvar(regAInfo->baseRhs,0) & 
     fdd_ithvar(regAInfo->baseExtra,1));
  ret = ret & fdd_ithvar(sizeInfo, 2);
  return ret;
}

bdd ProgramBddContext::bddPlus(unsigned in1Size, unsigned in2Size)
{
  if(in1Size != in2Size)
    LOG(ERROR) << "[ProgramBddContext::bddPlus] Addition of number of different bit widths is not allowed.\n";
  int outSize = in1Size;
  bdd ret = bddfalse;
  for(unsigned i=0; i<in1Size; ++i){
    for(unsigned j=0; j<in2Size; ++j){
      int k = (i + j) % outSize;
      ret = ret  |
        (fdd_ithvar(regAInfo->baseRhs,i) &
         fdd_ithvar(regBInfo->baseRhs,j) &
         fdd_ithvar(regAInfo->baseExtra,k));
    }
  }
  ret = ret & fdd_ithvar(sizeInfo, outSize);
  return ret;
}

bdd ProgramBddContext::bddMinus(unsigned in1Size, unsigned in2Size)
{
  if(in1Size != in2Size)
    LOG(ERROR) << "[ProgramBddContext::bddMinus] Subtraction of number of different bit widths is not allowed.\n";
  int outSize = in1Size;
  bdd ret = bddfalse;
  for(unsigned i=0; i<in1Size; ++i){
    for(unsigned j=0; j<in2Size; ++j){
      int k = (i - j + outSize) % outSize;
      ret = ret  |
        (fdd_ithvar(regAInfo->baseRhs,i) &
         fdd_ithvar(regBInfo->baseRhs,j) &
         fdd_ithvar(regAInfo->baseExtra,k));
    }
  }
  ret = ret & fdd_ithvar(sizeInfo, outSize);
  return ret;
}

bdd ProgramBddContext::bddTimes(unsigned in1Size, unsigned in2Size)
{
  if(in1Size != in2Size)
    LOG(ERROR) << "[ProgramBddContext::bddTimes] Multiplication of number of different bit widths is not allowed.\n";
  int outSize = in1Size;
  bdd ret = bddfalse;
  for(unsigned i=0; i<in1Size; ++i){
    for(unsigned j=0; j<in2Size; ++j){
      int k = (i * j) % outSize;
      ret = ret  |
        (fdd_ithvar(regAInfo->baseRhs,i) &
         fdd_ithvar(regBInfo->baseRhs,j) &
         fdd_ithvar(regAInfo->baseExtra,k));
    }
  }
  ret = ret & fdd_ithvar(sizeInfo, outSize);
  return ret;
}

bdd ProgramBddContext::bddDiv(unsigned in1Size, unsigned in2Size)
{
  if(in1Size != in2Size)
    LOG(ERROR) << "[ProgramBddContext::bddDiv] Division of number of different bit widths is not allowed.\n";
  int outSize = in1Size;
  bdd ret = bddfalse;
  for(unsigned i=0; i<in1Size; ++i){
    for(unsigned j=0; j<in2Size; ++j){
      int k;
      if(j == 0)
        //arbitrary
        k = 0;
      else
        k = (int) i / j;
      ret = ret |
        (fdd_ithvar(regAInfo->baseRhs,i) &
         fdd_ithvar(regBInfo->baseRhs,j) &
         fdd_ithvar(regAInfo->baseExtra,k));
    }
  }
  ret = ret & fdd_ithvar(sizeInfo, outSize);
  return ret;
}

unsigned ProgramBddContext::getRegSize(bdd forThis)
{
  //Inefficient!!!
  for(unsigned i = 0; i <= maxSize; ++i){
    bdd tmp = fdd_ithvar(sizeInfo, i);
    if((tmp & forThis) != bddfalse)
      return i;
  }
  LOG(FATAL) << "[ProgramBddContext::getRegSize] bdd does not have a recognizable regSize\n";
  return 0;
}

bdd ProgramBddContext::Assign(std::string var, bdd expr)
{
  bddinfo_t bi;
  if(this->find(var) == this->end()){
    LOG(WARNING) << "[ProgramBddContext::Assign] Unknown Variable";
    return bddfalse;
  }else{
    bi = this->find(var)->second;
  }

  //redundant?
  bddPair *regARhs2Extra = bdd_newpair();
  fdd_setpair(
      regARhs2Extra,
      regAInfo->baseRhs,
      regAInfo->baseExtra
      );
  expr = bdd_replace(expr,regARhs2Extra);
  bdd_freepair(regARhs2Extra);
  //up to here.

  bdd regA2var = bddfalse;
  for(unsigned i = 0; i < bi->maxVal; ++i)
    regA2var = regA2var |
      (fdd_ithvar(regAInfo->baseExtra,i) & fdd_ithvar(bi->baseRhs,i));
  expr = bdd_relprod(expr,regA2var,fdd_ithset(regAInfo->baseExtra));

  bdd c = bddtrue;
  for(
      BddContext::const_iterator iter = this->begin();
      iter != this->end();
      ++iter)
  {
    if(var != iter->first)
      c = c & fdd_equals((iter->second)->baseLhs,
          (iter->second)->baseRhs);
  }
  return bdd_exist(expr & c, fdd_ithset(sizeInfo));
}

bdd ProgramBddContext::Assume(bdd expr1, bdd expr2)
{
  bddPair *regARhs2Extra = bdd_newpair();
  fdd_setpair(
      regARhs2Extra,
      regAInfo->baseRhs,
      regAInfo->baseExtra
      );
  expr1 = bdd_replace(expr1, regARhs2Extra);
  bdd_freepair(regARhs2Extra);

  bddPair *regARhs2BExtra = bdd_newpair();
  fdd_setpair(
      regARhs2BExtra,
      regAInfo->baseRhs,
      regBInfo->baseExtra
      );
  expr2 = bdd_replace(expr2, regARhs2BExtra);
  bdd_freepair(regARhs2BExtra);

  bddPair *baseLhs2Rhs = bdd_newpair();
  for(BddContext::const_iterator iter = this->begin(); iter != this->end(); ++iter){
    fdd_setpair(
        baseLhs2Rhs,
        (iter->second)->baseLhs,
        (iter->second)->baseRhs
        );
  }
  expr2 = bdd_replace(expr2, baseLhs2Rhs);
  bdd_freepair(baseLhs2Rhs);

  bdd equate = bddtrue;
  for(BddContext::const_iterator iter = this->begin(); iter != this->end(); ++iter){
    equate = equate &
      fdd_equals(
          iter->second->baseLhs,
          iter->second->baseRhs
          );
  } 
  equate = equate &
    fdd_equals(
        regAInfo->baseExtra,
        regBInfo->baseExtra
        );

  bdd ret = expr1 & expr2 & equate;

  ret = bdd_exist(ret, fdd_ithset(regAInfo->baseExtra));
  ret = bdd_exist(ret, fdd_ithset(regBInfo->baseExtra));

  return bdd_exist(ret, fdd_ithset(sizeInfo));
}

bdd ProgramBddContext::tGetRandomTransformer(bool isTensored, unsigned seed)
{
  if(seed != 0)
    srand(seed);
  bdd ret = bddfalse;
  int numRounds = rand() % 10 + 1;
  for(int c=0; c < numRounds; ++c){
    bdd inbdd = rand()%2?bddfalse:bddtrue;
    for(BddContext::const_iterator iter = this->begin(); 
        iter != this->end();
        ++iter){
      int siz = iter->second->maxVal;
      int n;
      if(!isTensored){
        n = rand() % 4;
        if(n==0)
          inbdd = inbdd & fdd_ithvar(iter->second->baseLhs,rand()%siz);
        if(n==1)    
          inbdd = inbdd | fdd_ithvar(iter->second->baseLhs,rand()%siz);
        n = rand() % 4;
        if(n==0)
          inbdd = inbdd & fdd_ithvar(iter->second->baseRhs,rand()%siz);
        if(n==1)    
          inbdd = inbdd | fdd_ithvar(iter->second->baseRhs,rand()%siz);
      }else{
        n = rand() % 4;
        if(n==0)
          inbdd = inbdd & fdd_ithvar(iter->second->tensor1Lhs,rand()%siz);
        if(n==1)    
          inbdd = inbdd | fdd_ithvar(iter->second->tensor1Lhs,rand()%siz);
        n = rand() % 4;
        if(n==0)
          inbdd = inbdd & fdd_ithvar(iter->second->tensor1Rhs,rand()%siz);
        if(n==1)    
          inbdd = inbdd | fdd_ithvar(iter->second->tensor1Rhs,rand()%siz);
        n = rand() % 4;
        if(n==0)
          inbdd = inbdd & fdd_ithvar(iter->second->tensor2Lhs,rand()%siz);
        if(n==1)    
          inbdd = inbdd | fdd_ithvar(iter->second->tensor2Lhs,rand()%siz);
        n = rand() % 4;
        if(n==0)
          inbdd = inbdd & fdd_ithvar(iter->second->tensor2Rhs,rand()%siz);
        if(n==1)    
          inbdd = inbdd | fdd_ithvar(iter->second->tensor2Rhs,rand()%siz);
      }
    }
    ret = rand() % 2 ? ret & inbdd : ret | inbdd;
  }
  return ret;
}

// Yo, Emacs!
// Local Variables:
//   c-file-style: "ellemtel"
//   c-basic-offset: 2
// End:
