#include <sstream>

#include "gtest/gtest.h"

#include "opennwa/Nwa.hpp"
#include "opennwa/NwaParser.hpp"

#include "Tests/nwa/Source/fixtures.hpp"
#include "Tests/nwa/Source/class-NWA/supporting.hpp"

using namespace opennwa;

#define NUM_ELEMENTS(array)  (sizeof(array)/sizeof((array)[0]))

static Nwa const nwas[] = {
    Nwa(),
    AcceptsBalancedOnly().nwa,
    AcceptsStrictlyUnbalancedLeft().nwa,
    AcceptsPossiblyUnbalancedLeft().nwa,
    AcceptsStrictlyUnbalancedRight().nwa,
    AcceptsPossiblyUnbalancedRight().nwa,
    AcceptsPositionallyConsistentString().nwa
};

static const unsigned num_nwas = NUM_ELEMENTS(nwas);


namespace opennwa {

        void
        expect_nwa_is_idempotent(Nwa const & nwa)
        {
            std::stringstream output;
            nwa.print(output);

            NwaRefPtr again_nwa = read_nwa(output);

            std::stringstream again_output;
            again_nwa->print(again_output);

            EXPECT_EQ(nwa, *again_nwa);
            EXPECT_EQ(output.str(), again_output.str());
        }

        TEST(opennwa$$print$and$read_nwa$, printAndread_nwaAreIdempotent)
        {
            for (unsigned nwa = 0; nwa < num_nwas; ++nwa) {
                std::stringstream ss;
                ss << "Testing NWA " << nwa;
                SCOPED_TRACE(ss.str());

                expect_nwa_is_idempotent(nwas[nwa]);
            }
        }
        
}

