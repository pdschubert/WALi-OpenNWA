#ifndef wali_wpds_fwpds_FWPDS_GUARD
#define wali_wpds_fwpds_FWPDS_GUARD 1

/*!
 * @author Nick Kidd
 * @author Akash Lal
 *
 * $Id$
 */

#include "wali/Common.hpp"
#include "wali/wpds/WPDS.hpp"
#include "wali/wpds/Wrapper.hpp"

#include "wali/wpds/ewpds/EWPDS.hpp"
#include "wali/wpds/ewpds/MergeFunction.hpp"

#include "wali/graph/GraphCommon.hpp"
#include "wali/graph/InterGraph.hpp"

namespace wali {

    namespace wfa {
        class WFA;
        class Trans;
    }
    namespace graph {
        class InterGraph;
    }

    namespace wpds {

        class Wrapper;
        class rule_t;

        namespace fwpds {


            class FWPDS : public ewpds::EWPDS {
                public:
                    typedef ewpds::merge_fn_t mfun_t;

                public:
                    FWPDS();
                    FWPDS(Wrapper* wrapper);
                    FWPDS( Worklist<wfa::Trans> * worklist );
                    FWPDS( Wrapper * wrapper , Worklist<wfa::Trans> * worklist );

                    ////////////
                    // add rules
                    ////////////

                    // Inherited from EWPDS

                    ///////////
                    // pre*
                    ///////////
                    virtual void prestar( wfa::WFA & input, wfa::WFA & output );


                    ///////////
                    // post*
                    ///////////
                    virtual void poststar( wfa::WFA & input, wfa::WFA & output );

                    ///////////////////////
                    // FWPDS Settings
                    //////////////////////

                    /*! @brief Sets evaluation strategy for RegExp after InterGraph
                     * saturation is complete, i.e., during the time when
                     * transition weights are calculated. It is true by default,
                     * but setting it to false seems to be far more efficient
                     * while using BDD-based weight domain (Moped)
                     */
                    static void topDownEval(bool f);

              private:
                    void prestar_handle_call(
                            wfa::Trans *t1,
                            wfa::Trans *t2,
                            rule_t &r,
                            sem_elem_t delta
                            );

                    void prestar_handle_trans(
                            wfa::Trans * t,
                            wfa::WFA & ca  ,
                            rule_t & r,
                            sem_elem_t delta );

                    void poststar_handle_eps_trans(
                            wfa::Trans *teps, 
                            wfa::Trans *tprime,
                            sem_elem_t delta);

                    void poststar_handle_trans(
                            wfa::Trans * t ,
                            wfa::WFA & ca   ,
                            rule_t & r,
                            sem_elem_t delta
                            );

                    void update(
                                wali::Key from
                                , wali::Key stack
                                , wali::Key to
                                , sem_elem_t se
                                , Config * cfg
                                );

                    wfa::Trans * update_prime(
                            wali::Key from
                            , wali::Key stack
                            , wali::Key to
                            , sem_elem_t se
                            );
              
                    void operator()( wali::wfa::Trans * orig );
                    ///////////
                    // helpers
                    ///////////
                    bool checkResults( wfa::WFA& input, bool poststar );

                protected:
                    sem_elem_t wghtOne;
                    ref_ptr<wali::graph::InterGraph> interGr;
                    bool checkingPhase;

            }; // class FWPDS

        } // namespace fwpds

    } // namespace wpds

} // namespace wali

#endif  // wali_wpds_fwpds_FWPDS_GUARD

/* Yo, Emacs!
   ;;; Local Variables: ***
   ;;; tab-width: 4 ***
   ;;; End: ***
*/

