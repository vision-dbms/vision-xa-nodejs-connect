#ifndef VA_Node_Isolate_Interface
#define VA_Node_Isolate_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "v8.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Isolate : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Isolate, Vca::VRolePlayer);

        //  Construction
        private:
            Isolate (v8::Isolate *pIsolate);

        //  Destruction
        private:
            ~Isolate ();

        //  Access
        public:
            v8::Isolate *isolate () const {
                return m_pIsolate;
            }

        //  State
        private:
            v8::Isolate *const m_pIsolate;
        };

    } // namespace VA::Node

} // namespace VA


#endif
