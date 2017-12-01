#ifndef Vision_Evaluation_Gofer_Interface
#define Vision_Evaluation_Gofer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInstance.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vision_Evaluation_Value.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vision {
    namespace Evaluation {
        class Gofer : public Vca::VGoferInstance<Value> {
            DECLARE_CONCRETE_RTTLITE (Gofer, Vca::VGoferInstance<Value>);

        //  Construction
        public:
            Gofer () {
            }
        //  Destruction
        private:
            ~Gofer () {
            };
        };
    } // namespace Evaluation
} // namespace Vision

#endif
