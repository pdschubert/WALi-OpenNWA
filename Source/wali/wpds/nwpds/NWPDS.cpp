/**
 * @author Prathmesh Prabhu
 */

// ::wali::wpds::nwpds
#include "wali/wpds/nwpds/NWPDS.hpp"
// ::wali::wpds
#include "wali/wpds/RuleFunctor.hpp"
#include "wali/wpds/Rule.hpp"
// ::wali::wpds::ewpds
#include "wali/wpds/ewpds/ERule.hpp"
#include "wali/wpds/ewpds/ETrans.hpp"
// ::wali::Wfa
#include "wali/wfa/WFA.hpp"
#include "wali/wfa/Trans.hpp"
#include "wali/wfa/ITrans.hpp"
// ::wali
#include "wali/Key.hpp"
#include "wali/Common.hpp"
#include "wali/KeySource.hpp"
#include "wali/KeyPairSource.hpp"
#include "wali/Worklist.hpp"
#include "wali/DefaultWorklist.hpp"
//#include "wali/KeyOrderWorklist.hpp"

using namespace std;
using namespace wali;
using namespace wali::wpds;
using namespace wali::wpds::ewpds;
using namespace wali::wpds::fwpds;
using namespace wali::wpds::nwpds;

namespace wali 
{
  namespace wpds
  {
    namespace nwpds
    {
      const std::string NWPDS::XMLTag("NWPDS");
    } // namespace nwpds
  } //namespace wpds
} //namewpace wali

NWPDS::NWPDS(bool b) :
  EWPDS(),
  dbg(b)
{
  //XXX:HACK?
  //we disable wali::strict
  //We allow incoming transitions to query automata. 
  //Two newton steps are like a continued pre/post star query interjected
  //by other stuff. So we want to allow incoming transitions there (I think)
  wali::set_strict(false);
  worklist = new DefaultWorklist<wfa::ITrans>; //new KeyOrderWorklist;
}

NWPDS::NWPDS(ref_ptr<wpds::Wrapper> wrapper, bool b) :
  EWPDS(wrapper),
  dbg(b)
{
  //XXX:HACK?
  //we disable wali::strict
  //We allow incoming transitions to query automata. 
  //Two newton steps are like a continued pre/post star query interjected
  //by other stuff. So we want to allow incoming transitions there (I think)
  wali::set_strict(false);
}

NWPDS::NWPDS(const NWPDS& f) :
  EWPDS(f),
  newtonWl(f.newtonWl),
  dbg(f.dbg)
{
  //XXX:HACK?
  //we disable wali::strict
  //We allow incoming transitions to query automata. 
  //Two newton steps are like a continued pre/post star query interjected
  //by other stuff. So we want to allow incoming transitions there (I think)
  wali::set_strict(false);
}

NWPDS::~NWPDS()
{
  stack2ConstMap.clear();
  savedRules.clear();
}

Key NWPDS::getOldKey(Key newKey)
{
  if(stack2ConstMap.find(newKey) == stack2ConstMap.end()){
    string oldKeyStr = key2str(newKey) + "_NEWTON_OLDVAL";
    Key oldKey = getKey(oldKeyStr);
    stack2ConstMap[newKey] = oldKey;
  }
  return stack2ConstMap[newKey];
}

/*
   replace each delta_2 rule <p,y> -> <p',y'y''>
   by two rules:
     <p,y> -> <p',y't''> and <p,y> -> <p't'y''>
     here t' and t'' are new stack symbols that will hold on to the values of y',y'' from last Newton 
     iteration during saturation.
*/
void NWPDS::prestarSetupPds()
{
  if(dbg){
    *waliErr << "NWPDS::prestarSetupPds()\n";
    WpdsRules wr;
    this->for_each(wr);
    *waliErr << "Printing out rules before processing.\n";
    *waliErr << "Delta_1 rules: \n";
    for(std::set<Rule>::iterator it = wr.stepRules.begin();
        it != wr.stepRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_2 rules: \n";
    for(std::set<Rule>::iterator it = wr.pushRules.begin();
        it != wr.pushRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_0 rules: \n";
    for(std::set<Rule>::iterator it = wr.popRules.begin();
        it != wr.popRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
  }
  stack2ConstMap.clear();
  savedRules.clear();
  Delta2Rules dr;
  this->for_each(dr);
  for(std::vector<rule_t>::iterator it = dr.rules.begin();
      it != dr.rules.end();
      ++it){
    rule_t r = *it;
    if(dbg){
      *waliErr << "[SetupPDS] Processing Rule:" << std::endl;
      r->print(*waliErr);
      *waliErr << endl;
    }
    //backup the current rule
    savedRules.push_back(r);

    Key p = r->from_state();
    Key y = r->from_stack();
    Key p_prime = r->to_state();
    Key y_prime = r->to_stack1();
    Key y_prime_prime = r->to_stack2();

    assert(y_prime_prime != WALI_EPSILON);
    assert(y_prime != WALI_EPSILON);

    Key t_prime = getOldKey(y_prime);
    Key t_prime_prime = getOldKey(y_prime_prime);

    //erase rule from the WPDS
    erase_rule(p,y,p_prime,y_prime,y_prime_prime);
    ERule * er = dynamic_cast<ERule*>(r.get_ptr()); //change to static_cast ? 
    //<p,y> -> <p',y't''>
    add_rule(p,y,p_prime,y_prime,t_prime_prime,er->weight(),er->merge_fn());
    //<p,y> -> <p',t'y''>
    add_rule(p,y,p_prime,t_prime,y_prime_prime,er->weight(),er->merge_fn());
  }
  if(dbg){
    WpdsRules wr;
    this->for_each(wr);
    *waliErr << "Printing out rules after processing.\n";
    *waliErr << "Delta_1 rules: \n";
    for(std::set<Rule>::iterator it = wr.stepRules.begin();
        it != wr.stepRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_2 rules: \n";
    for(std::set<Rule>::iterator it = wr.pushRules.begin();
        it != wr.pushRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_0 rules: \n";
    for(std::set<Rule>::iterator it = wr.popRules.begin();
        it != wr.popRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
  }
}

/*
   replace each delta_2 rule <p,y> -> <p',y'y''>
   by two rules:
     <p,t> -> <p',y'y''> and <p,y> -> <p't'y''>
     here t and t' are new stack symbols that will hold on to the values of y,y' from last Newton 
     iteration during saturation.
*/
void NWPDS::poststarSetupPds()
{
  if(dbg){
    *waliErr << "NWPDS::prestarSetupPds()\n";
    WpdsRules wr;
    this->for_each(wr);
    *waliErr << "Printing out rules before processing.\n";
    *waliErr << "Delta_1 rules: \n";
    for(std::set<Rule>::iterator it = wr.stepRules.begin();
        it != wr.stepRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_2 rules: \n";
    for(std::set<Rule>::iterator it = wr.pushRules.begin();
        it != wr.pushRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_0 rules: \n";
    for(std::set<Rule>::iterator it = wr.popRules.begin();
        it != wr.popRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
  }
  stack2ConstMap.clear();
  state2ConstMap.clear();
  savedRules.clear();
  Delta2Rules dr;
  this->for_each(dr);
  for(std::vector<rule_t>::iterator it = dr.rules.begin();
      it != dr.rules.end();
      ++it){
    rule_t r = *it;
    if(dbg){
      *waliErr << "[SetupPDS] Processing Rule:" << std::endl;
      r->print(*waliErr);
      *waliErr << endl;
    }
    //backup the current rule
    savedRules.push_back(r);

    Key p = r->from_state();
    Key y = r->from_stack();
    Key p_prime = r->to_state();
    Key y_prime = r->to_stack1();
    Key y_prime_prime = r->to_stack2();

    assert(y_prime_prime != WALI_EPSILON);
    assert(y_prime != WALI_EPSILON);

    Key t = getOldKey(y);
    Key t_prime = getOldKey(y_prime);

    //erase rule from the WPDS
    erase_rule(p,y,p_prime,y_prime,y_prime_prime);
    ERule * er = dynamic_cast<ERule*>(r.get_ptr()); //change to static_cast ? 
    //<p,t> -> <p',y'y''>
    add_rule(p,t,p_prime,y_prime,y_prime_prime,er->weight(),er->merge_fn());
    //<p,y> -> <p',t'y''>
    add_rule(p,y,p_prime,t_prime,y_prime_prime,er->weight(),er->merge_fn());

    //poststar will generate mid states for (p',t').
    //We need to remember these to be able to clean the final WFA.
    genStates.push_back(
        std::pair<wali::Key, std::pair<wali::Key, wali::Key> > (p_prime,
          std::pair<wali::Key,wali::Key>(y_prime,t_prime)));
  }
  if(dbg){
    WpdsRules wr;
    this->for_each(wr);
    *waliErr << "Printing out rules after processing.\n";
    *waliErr << "Delta_1 rules: \n";
    for(std::set<Rule>::iterator it = wr.stepRules.begin();
        it != wr.stepRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_2 rules: \n";
    for(std::set<Rule>::iterator it = wr.pushRules.begin();
        it != wr.pushRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
    *waliErr << "\n\nDelta_0 rules: \n";
    for(std::set<Rule>::iterator it = wr.popRules.begin();
        it != wr.popRules.end();
        ++it)
      (*it).print(*waliErr) << std::endl;
  }
}


void NWPDS::restorePds()
{
  //erase all existing delta 2 rules
  WpdsRules wr;
  this->for_each(wr);
  for(std::set<Rule>::iterator it = wr.pushRules.begin();
      it != wr.pushRules.end();
      ++it){
    Rule r = *it;
    erase_rule(r.from_state(), r.from_stack(), r.to_state(),
        r.to_stack1(), r.to_stack2());
  }
  //Now restore the original rules
  for(std::vector<rule_t>::iterator it = savedRules.begin();
      it != savedRules.end();
      ++it){
    ERule * er = dynamic_cast<ERule*>(it->get_ptr());
    add_rule(er->from_state(), er->from_stack(), er->to_state(),
        er->to_stack1(), er->to_stack2(), er->weight(), er->merge_fn());
  }
}

void NWPDS::updateFa(wfa::WFA& fa)
{
  wali::wfa::ITrans *t;
  for(worklist_t::iterator it = newtonWl.begin();
      it != newtonWl.end();
      ++it){
    t = *it;

    //DEBUGGING
    //std::cout << ":" << t->from() << "-" << t->stack() << "->" << t->to() <<std::endl;

    if(dbg){
      *waliErr << "[UpdateFa]: " << std::endl << "Processing:" << std::endl;
      t->print(*waliErr) << std::endl;
    }
    bool oldToUpdate = false;
    wali::Key oldFrom, oldTo, oldStack;
    Key2KeyMap::const_iterator fromIt, toIt, stackIt;
    fromIt = toIt = state2ConstMap.end();
    stackIt = stack2ConstMap.end();
    //For a trans t, an old trans exists, if we find a match in the map stack2ConstMap
    //for
    // (a) The stack symbol, or
    // (b) either of the states are intermideate states and we find a match for them
    //stack symbol match -->
    stackIt = stack2ConstMap.find(t->stack());
    if(stackIt != stack2ConstMap.end()){
      oldStack = stackIt->second;
      oldToUpdate = true;
    }else{
      oldStack = t->stack();
    }
    //from state match -->
    fromIt = state2ConstMap.find(t->from());
    if(fromIt != state2ConstMap.end()){
      oldFrom = fromIt->second;
      oldToUpdate = true;
    }else{
      oldFrom = t->from();
    }
    //to state match -->
    toIt = state2ConstMap.find(t->to());
    Key2KeyMap::iterator match = state2StackMap.find(t->to());
    if(toIt != state2ConstMap.end() && match != state2StackMap.end() && match->second == t->to()){
      //to() is an intermideate state
      oldTo = toIt->second;
      oldToUpdate = true;
    }else{
      oldTo = t->to();
    }

    if(oldToUpdate){
      if(dbg){
        *waliErr << "Updating: [" 
          << wali::key2str(oldFrom) << ", "
          << wali::key2str(oldStack) << ", "
          << wali::key2str(oldTo) << "]"<< std::endl;
        wfa::Trans old;
        fa.find(oldFrom, oldStack, oldTo, old);
        if(old.weight() != NULL)
          (old.weight())->print(*waliErr << "NEWTON_CONST_WT" << std::endl) 
            << std::endl;
        if(t->weight() != NULL)
          (t->weight())->print(*waliErr << "NEWTON_VAR_WT" 
              << std::endl) << std::endl;
        if(t->weight() != NULL && old.weight() != NULL)
          *waliErr << "The weights are the same (" 
            << (t->weight()->equal(old.weight())) << ")" << std::endl;
      }
      //We want to track if anything was updated during the
      //functor operation.
      bool changed = false;
      //{
        wfa::Trans old;
        bool found = fa.find(oldFrom, oldStack, oldTo, old);
        wali::sem_elem_t newwt = t->weight();
        wali::sem_elem_t oldwt = found? old.weight() : NULL;
        if(newwt == NULL)
          changed = false;
        else if(oldwt == NULL)
          changed = true;
        else
          changed = !(newwt->equal(oldwt));
      //}
      //Now actually do the update, changing the from/stack/to as needed.
      if(changed){
        wali::wfa::ITrans * oldt = NULL;
        oldt = t->copy(oldFrom,oldStack,oldTo);
        assert(oldt != NULL);
        fa.addTrans(oldt);
        //DEBUGGING
        //oldt->print(*waliErr << "[Newton] Updated transition: ") << std::endl;
        //t->print(*waliErr << "from: ") << std::endl << std::endl;
        //DEBUGGING
        if(fromIt == state2ConstMap.end()){
          wali::Key f = oldt->from();
          wali::Key s = oldt->stack();
          Config * cfg = make_config(f,s); 
          oldt->setConfig(cfg);
          //Trans with generated from state do not go onto the worklist
          worklist->put(oldt);
        }
      }
    }
  }
  newtonWl.clear();
}

void NWPDS::prestar(wfa::WFA const & input, wfa::WFA & output)
{
  int newtonSteps = 0;
  genStates.clear();
  savedRules.clear();
  stack2ConstMap.clear();
  state2ConstMap.clear();
  state2StackMap.clear();

  prestarSetupPds();
  EWPDS::addEtrans = true;
  EWPDS::prestarSetupFixpoint(input, output);
  EWPDS::addEtrans = false;

  CreateInitialNewtonWl cinw(newtonWl);
  output.for_each(cinw);
  do{
    EWPDS::prestarComputeFixpoint(output);
    updateFa(output);
    newtonSteps++;
  }while(worklist->empty());
  //if(dbg)
  *waliErr << "Total Newton Steps: " << newtonSteps << std::endl;
  restorePds();
  EWPDS::unlinkOutput(output);
  EWPDS::currentOutputWFA = 0;
  cleanUpFa(output);
}

void NWPDS::poststar(wfa::WFA const & input, wfa::WFA & output)
{
  int newtonSteps = 0;
  genStates.clear();
  savedRules.clear();
  stack2ConstMap.clear();
  state2ConstMap.clear();
  state2StackMap.clear();

  poststarSetupPds();
  poststarSetupFixpoint(input,output);

  CreateInitialNewtonWl cinw(newtonWl);
  output.for_each(cinw);
  do{
    EWPDS::poststarComputeFixpoint(output);
    updateFa(output);
    newtonSteps++;
  }while(!worklist->empty());
  //if(dbg)
  *waliErr << "Total Newton Steps: " << newtonSteps << std::endl;
  restorePds();
  EWPDS::unlinkOutput(output);
  EWPDS::currentOutputWFA = 0;
  cleanUpFa(output);
}

void NWPDS::cleanUpFa(wfa::WFA& output)
{
  Key2KeyMap old2NewStackMap;
  Key2KeyMap old2NewStateMap;
  for(Key2KeyMap::iterator iter = stack2ConstMap.begin();
      iter != stack2ConstMap.end();
      ++iter)
    old2NewStackMap[iter->second] = iter->first;
  for(Key2KeyMap::iterator iter = state2ConstMap.begin();
      iter != state2ConstMap.end();
      ++iter)
    old2NewStateMap[iter->second] = iter->first;

  //DEBUGGING
  //*waliErr << "oldStates: ";
  //for(std::set<wali::Key>::iterator it = oldStates.begin();
  //    it != oldStates.end();
  //    ++it)
  //  *waliErr << wali::key2str(*it) << " ";
  //*waliErr << std::endl;
  //DEBUGGING

  RemoveOldTrans rot(old2NewStackMap,old2NewStateMap);
  output.for_each(rot);
}

void NWPDS::poststarSetupFixpoint(WFA const & input, WFA& fa)
{
  EWPDS::poststarSetupFixpoint(input,fa);
  for(GenStates::const_iterator it = genStates.begin();
      it != genStates.end();
      ++it){
    wali::Key a = gen_state((*it).first,(*it).second.first);
    wali::Key b =  gen_state((*it).first,(*it).second.second);
    state2ConstMap[a] = b;
    state2StackMap[a] = (*it).second.first;
    //DEBUGGING
    //*waliErr << "Added[state2ConstMap]" << wali::key2str(a) << " --> " << wali::key2str(b) << std::endl;
  }
}

void NWPDS::update(Key from, Key stack, Key to, sem_elem_t se, Config * cfg)
{

  wali::wfa::ITrans *t;
  if(addEtrans) {
    t = currentOutputWFA->insert(new wali::wpds::ewpds::ETrans(from, stack, to,
          0, se, 0));
  } else {
    t = currentOutputWFA->insert(new wali::wfa::Trans(from, stack, to, se));
  }

  t->setConfig(cfg);
  if (t->modified()) {
    worklist->put( t );
    newtonWl.push_back( t );
  }
}

wali::wfa::ITrans* NWPDS::update_prime( Key from, wali::wfa::ITrans* call, rule_t r, sem_elem_t delta, sem_elem_t wWithRule)
{
  //
  // !!NOTE!!
  // This code is copied in FWPDS::update_prime.
  // Changes here should be reflected there.
  //
  ERule* er = (ERule*)r.get_ptr();
  wali::wfa::ITrans* tmp = 
    new wali::wpds::ewpds::ETrans(
        from, r->to_stack2(), call->to(),
        delta, wWithRule, er);
  wali::wfa::ITrans* t = currentOutputWFA->insert(tmp);
  if(t->modified())
    newtonWl.push_back( t );
  return t;
}

#if 0
void NWPDS::update(Key from, Key stack, Key to, sem_elem_t se, Config * cfg)
{
  EWPDS::update(from,stack,to,se,cfg);
  wali::wfa::Trans t;
  currentOutputWFA->find(from,stack,to,t);
  if(t.modified()){
    newtonWl->put(t);
  }
}

wali::wfa::ITrans* NWPDS::update_prime(Key from, wfa::ITrans* call, rule_t r,  sem_elem_t delta, sem_elem_t wWithRule)
{
  wali::wfa::ITrans* ret = update_prime(from,call,r,delta,wWithRule);
  wali::wfa::ITrans t;
  currentOutputWFA->find(from,r->to_stack2(),call->to(),t);
  if(t.modified()){
    newtonWl->put(t);
  }
}
#endif


/////////////////////////////////////////////////////////////////
// class Dela2Rules
/////////////////////////////////////////////////////////////////

Delta2Rules::Delta2Rules() :
  RuleFunctor()
{}

Delta2Rules::~Delta2Rules() {}

void Delta2Rules::operator() (rule_t & r)
{
  if( r->is_rule2() )
    rules.push_back(r);
}

/////////////////////////////////////////////////////////////////
// class RemoveOldTrans
/////////////////////////////////////////////////////////////////
RemoveOldTrans::RemoveOldTrans(const NWPDS::Key2KeyMap& m, const NWPDS::Key2KeyMap& g) : oldStackMap(m), oldStateMap(g){}

void RemoveOldTrans::operator() (wali::wfa::ITrans* t)
{
  //is stack phony?
  if(oldStackMap.find(t->stack()) != oldStackMap.end())
    if(t->weight() != NULL)
      t->setWeight(t->weight()->zero());
  //is from phony?
  if(oldStateMap.find(t->from()) != oldStateMap.end())
    if(t->weight() != NULL)
      t->setWeight(t->weight()->zero());
  //is to phony?
  if(oldStateMap.find(t->to()) != oldStateMap.end())
    if(t->weight() != NULL)
      t->setWeight(t->weight()->zero());
}


/////////////////////////////////////////////////////////////////
// class CreateInitialNewtonWl
/////////////////////////////////////////////////////////////////
CreateInitialNewtonWl::CreateInitialNewtonWl(NWPDS::worklist_t& w): wl(w) {}
void CreateInitialNewtonWl::operator () (wali::wfa::ITrans* t)
{
  wl.push_back(t);
}