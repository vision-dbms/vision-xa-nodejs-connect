#ifndef Vision_Node_Store_Interface
#define Vision_Node_Store_Interface

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

namespace Vision {
    namespace Node {
        using v8::Isolate;
        using v8::Local;
        using v8::Persistent;

        class Store : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Store, Vca::VRolePlayer);

        //  Index Type
        public:
            typedef int index_t;

        //  Isolate Store Access
        //  Construction
        public:
            static Store *GetStoreFor (Isolate *pIsolate);

        private:
            Store (Isolate *pIsolate);

        //  Destruction
        private:
            ~Store ();

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
        }; // class Store
    } // namespace Vision::Node
} // Vision


#endif
