#ifndef wali_WEIGHT_HANDLER_GUARD
#define wali_WEIGHT_HANDLER_GUARD 1

/**
 * @author Nicholas Kidd
 */

#include "wali/Common.hpp"
#include "wali/SemElem.hpp"
#include "wali/MergeFn.hpp"

#include "wali/IWaliHandler.hpp"

namespace wali
{

  class IUserHandler : public IWaliHandler
  {

    public:
      IUserHandler();

      virtual ~IUserHandler();

      /**
       * @return true if [tag] is one of
       * <ul>
       *   <li>  wali::SemElem::XMLTag  </li>
       *   <li>  wali::MergeFn::XMLTag  </li>
       * </ul>
       *
       * Users who override this method should
       * retain the above behavior, and also
       * return true for user extensions.
       */
      virtual bool handlesElement( std::string tag );

      /**
       * @return the sem_elem_t for the parsed weight.
       */
      virtual sem_elem_t getWeight() = 0;

      /**
       * This method returns a parsed merge function.
       * It will *only* be invoked if the an EWPDS or FWPDS
       * is being parsed.
       *
       * @return the merge_fn_t for the parsed merge function.
       */
      virtual merge_fn_t getMergeFn() = 0;

  }; // class IUserHandler

} // namespace wali

#endif  // wali_WEIGHT_HANDLER_GUARD


