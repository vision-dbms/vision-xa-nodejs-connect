#ifndef VA_Node_Isolate_Interface
#define VA_Node_Isolate_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_process.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Export;

        class Isolate : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Isolate, Vca::VRolePlayer);

            friend class Process;
            friend class Export;

        //  Aliases
        public:
            typedef v8::Isolate object_t;
            typedef object_t*   handle_t;
            typedef object_t*   global_t;

            typedef v8::NativeWeakMap              object_cache_t;
            typedef v8::Local<object_cache_t>      object_cache_handle_t;
            typedef v8::Persistent<object_cache_t> object_cache_global_t;

        //  Construction
        private:
            Isolate (Process *pProcess, v8::Isolate *pIsolate);

        //  Destruction
        private:
            ~Isolate ();

            bool onDeleteThis ();

        //  Instantiation
        public:
            static bool GetInstance (Reference &rpInstance, v8::Isolate *pIsolate);

        //  Access
        public:
            handle_t isolate () const {
                return m_hIsolate;
            }
            handle_t handle () const {
                return m_hIsolate;
            }
            operator handle_t () const {
                return m_hIsolate;
            }
            v8::Local<v8::Context> getCurrentContext () const {
                return m_hIsolate->GetCurrentContext ();
            }

        //  Handle Conversion
        public:
            template <typename S> v8::Local<S> Local (v8::PersistentBase<S> const &rThat) const {
                return v8::Local<S>::New (m_hIsolate, rThat);
            }

        //  Object Creation
        public:
            string_handle_t NewString (char const *pString) const {
                return v8::String::NewFromUtf8 (m_hIsolate, pString);
            }

        //  Exceptions
        public:
            void ThrowTypeError (char const *pMessage) const;

        //  Export Access
            bool GetExport (Vxa::export_return_t &rExport, value_handle_t hValue);

        //  Model Management
        public:
            bool Attach (ClassTraits<Export>::retaining_ptr_t &rpModelObject, v8::Local<v8::Object> hObject);
            bool Attach (ClassTraits<Export>::retaining_ptr_t &rpModelObject, v8::Local<v8::Value> hValue);
        private:
            bool Detach (Export *pModelObject);

        //  State
        private:
            Process::Reference const m_pProcess;
            global_t           const m_hIsolate;
            object_cache_global_t    m_hObjectCache;
        };

    } // namespace VA::Node

} // namespace VA


#endif
