#ifndef VA_Node_Import_Interface
#define VA_Node_Import_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_isolated.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollectable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Import : public Isolated {
            DECLARE_CONCRETE_RTTLITE (Import, Isolated);

            friend class Isolate;

        //  Class Builder
        public:
            class ClassBuilder
                : public BaseClass::ClassBuilder
            {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };
            friend class ClassBuilder;

        //  Construction
        protected:
            Import (Isolate *pIsolate);

        //  Destruction
        protected:
            ~Import ();

        //  Decommissioning
        private:
            virtual bool decommission () override;

        //  External Management
        public:
            local_external_t getExternal ();
        private:
            static void WeakCallbackFinalizer (v8::WeakCallbackInfo<ThisClass> const &rCallbackInfo);
            void weakCallbackFinalizer (v8::WeakCallbackInfo<ThisClass> const &rCallbackInfo);

        //  State
        private:
            persistent_external_t m_hExternal;
        };
    } // namespace Node

} // namespace VA


#endif
