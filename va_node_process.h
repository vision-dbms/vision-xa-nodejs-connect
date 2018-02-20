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
        class Isolated;

        class Process : public Vca::VRolePlayer {
            DECLARE_ABSTRACT_RTTLITE (Process, Vca::VRolePlayer);

            friend class Isolate;

        //  Implementation Classes
        public:
            class Primary;
            class Secondary;

        //  Construction
        protected:
            Process () {
            }

        //  Destruction
        protected:
            ~Process () {
            }

        //  Isolate Management ...
        //  ... Interface
        public:
            static bool Attach (
                ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate
            ) {
                return Implementation ()->attach (rpModelObject, pIsolate);
            }
        private:
            static bool Detach (Isolate *pModelObject) {
                return Implementation ()->detach (pModelObject);
            }
        public:
            static size_t IsolateCacheSize () {
                return Implementation ()->isolateCacheSize ();
            }

        //  ... Implementation
        private:
            virtual bool attach (
                ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate
            ) = 0;
            virtual bool detach (Isolate *pModelObject) = 0;
            virtual size_t isolateCacheSize () const = 0;

        //  Object Management ...
        //  ... Interface
        private:
            static bool OkToDecommision (Isolated *pIsolated) {
                return Implementation ()->okToDecommision (pIsolated);
            }

        //  ... Implementation
        private:
            virtual bool okToDecommision (Isolated *pIsolated) = 0;
            
        //  Implementation
        public:
            static Process *Implementation ();
        };

    } // namespace VA::Node

} // namespace VA


#endif
