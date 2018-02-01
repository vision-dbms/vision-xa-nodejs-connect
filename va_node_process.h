#ifndef VA_Node_Process_Interface
#define VA_Node_Process_Interface

/*********************
 *****  Context  *****
 *********************/

#include "va_node.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include <unordered_map>

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Isolate;

        class Process : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Process, Vca::VRolePlayer);

            friend class Isolate;

        //  Construction
        private:
            Process ();

        //  Destruction
        private:
            ~Process ();

        //  Instantiation
        public:
            static bool GetInstance (Reference &rpInstance);

        //  Access
        public:
            size_t isolateCacheSize () const {
                return m_iIsolateCache.size ();
            }

        //  Model Management
        public:
            bool Attach (ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate);
        private:
            bool Detach (Isolate *pModelObject);

        //  State
        private:
            std::unordered_map<v8::Isolate*,ClassTraits<Isolate>::notaining_ptr_t> m_iIsolateCache;
        };

    } // namespace VA::Node

} // namespace VA


#endif
