#ifndef VA_Node_Process_Interface
#define VA_Node_Process_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Callback;
        class Isolate;
        class Isolated;

        class Process : public Entity {
            DECLARE_ABSTRACT_RTTLITE (Process, Entity);

            friend class Callback;
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

        //  Lifetime Management ...
        //  ... Interface
        private:
            static bool OkToDecommission (Isolated *pIsolated) {
                return Implementation ()->okToDecommission (pIsolated);
            }

        //  ... Implementation
        private:
            virtual bool okToDecommission (Isolated *pIsolated) = 0;

        //  Callback Management ...
        //  ... Interface
        private:
            static void ScheduleCallback (Callback *pCallback) {
                return Implementation ()->scheduleCallback (pCallback);
            }

        //  ... Implementation
        private:
            virtual void scheduleCallback (Callback *pCallback) = 0;

        //  Shutdown...
        //  ... Interface
        private:
            static bool OnShutdown (Isolate *pIsolate) {
                return Implementation ()->onShutdown (pIsolate);
            }

        //  ... Implementation
        private:
            virtual bool onShutdown (Isolate *pIsolate) = 0;

        private:
        //  Implementation
        public:
            static Process *Implementation ();
        };

    } // namespace VA::Node

} // namespace VA


#endif
