#ifndef VA_Node_Gateway_Interface
#define VA_Node_Gateway_Interface

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
        using v8::Isolate;
        using v8::Local;
        using v8::Persistent;

        class Gateway : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Gateway, Vca::VRolePlayer);

        //  Index Type
        public:
            typedef int index_t;

        //  Isolate Gateway Access
        //  Construction
        public:
            static Gateway *GetGatewayFor (Isolate *pIsolate);

        private:
            Gateway (Isolate *pIsolate);

        //  Destruction
        private:
            ~Gateway ();

        //  Access
        public:
            Isolate *isolate () const {
                return m_pIsolate;
            }

        //  Update
        public:
            template <typename V> index_t insert (V v) {
                return -1;
            }

        //  State
        private:
            Isolate *const m_pIsolate;
        }; // class Gateway
    } // namespace VA::Node
} // VA


#endif
