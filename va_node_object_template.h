#ifndef VA_Node_Object_Template_Interface
#define VA_Node_Object_Template_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_template.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_function_template.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class ObjectTemplate : public Template {
            DECLARE_CONCRETE_RTTLITE (ObjectTemplate, Template);

        //  Construction
        public:
            ObjectTemplate (Isolate *pIsolate);

        //  Destruction
        private:
            ~ObjectTemplate () {
            }

        //  Access
        public:
            local_object_template_t templateHandle () const {
                return LocalFor (m_hTemplate);
            }

        //  Initialization
        public:
            bool MaybeSetMemberFunction (
                VString const &rMemberName, v8::FunctionCallback pCallback
            );

        //  Instantiation
        public:
            bool MaybeGetInstance (local_object_t &rResult, void *pIFD) const;

        //  Internal Field Data Access/Update
        public:
            static void *GetObjectIFD (v8::FunctionCallbackInfo<value_t> args) {
                return GetObjectIFD (args.Holder ());
            }
            static void *GetObjectIFD (local_object_t hObject) {
                return local_external_t::Cast(hObject->GetInternalField(0))->Value();
            }

        //  State
        private:
            persistent_object_template_t const m_hTemplate;
        };

    } // namespace VA::Node

} // namespace VA


#endif
