#ifndef VA_Node_Context_Interface
#define VA_Node_Context_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_isolate.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Context : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Context, Vca::VRolePlayer);

            friend class Isolate;

        //  Construction
        private:
            Context (Isolate *pIsolate);

        //  Destruction
        private:
            ~Context ();

        //  Access
        public:
            Isolate *isolateWrapper () const {
                return m_pIsolate;
            }
            v8::Isolate *isolate () const {
                return isolateWrapper()->isolate();
            }

        //  State
        private:
            Isolate::Reference const m_pIsolate;
        };

    } // namespace Node

} // namespace VA


#endif
