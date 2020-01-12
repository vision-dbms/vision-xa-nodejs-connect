#ifndef VA_Node_Function_Template_Interface
#define VA_Node_Function_Template_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_template.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class FunctionTemplate : public Template {
            DECLARE_CONCRETE_RTTLITE (FunctionTemplate, Template);

        //  Construction
        public:
            FunctionTemplate (Isolate *pIsolate, v8::FunctionCallback pCallback);

        //  Destruction
        private:
            ~FunctionTemplate () {
            }

        //  Access
        public:
            local_function_template_t templateHandle () const {
                return LocalFor (m_hTemplate);
            }

        //  Instantiation
        public:
            bool MaybeGetFunction (local_function_t &rResult) const;

        //  State
        private:
            persistent_function_template_t const m_hTemplate;
        };

    } // namespace VA::Node

} // namespace VA


#endif
