#ifndef VA_Node_Template_Interface
#define VA_Node_Template_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_isolated.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Template : public Isolated {
            DECLARE_ABSTRACT_RTTLITE (Template, Isolated);

        //  Construction
        protected:
            Template (Isolate *pIsolate);

        //  Destruction
        protected:
            ~Template () {
            }

        //  State
        private:
        };

    } // namespace VA::Node

} // namespace VA


#endif
