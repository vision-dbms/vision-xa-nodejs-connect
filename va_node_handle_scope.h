#ifndef VA_Node_HandleScope_Interface
#define VA_Node_HandleScope_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_isolated.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        template <typename Wrapped_T> class HandleScopeWrapper {
        //  Aliases
        public:
            typedef Wrapped_T wrapped_t;

            typedef Isolated::isolate_handle_t isolate_handle_t;

        //  Construction
        public:
            HandleScopeWrapper (Isolated *pIsolated) : HandleScopeWrapper (pIsolated->isolate ()) {
            }
            HandleScopeWrapper (Isolate *pIsolate) : HandleScopeWrapper (pIsolate->handle ()) {
            }
            HandleScopeWrapper (isolate_handle_t hIsolate) : m_iWrappedScope (hIsolate) {
            }

        //  Destruction
        public:
            ~HandleScopeWrapper () {
            }

        //  Cast
        public:
            operator wrapped_t const& () const {
                return m_iWrappedScope;
            }
            operator wrapped_t& () {
                return m_iWrappedScope;
            }

        //  State
        private:
            wrapped_t m_iWrappedScope;
        };

        typedef HandleScopeWrapper<v8::HandleScope> HandleScope;
        typedef HandleScopeWrapper<v8::EscapableHandleScope> EscapableHandleScope;

    } // namespace VA::Node

} // namespace VA


#endif
