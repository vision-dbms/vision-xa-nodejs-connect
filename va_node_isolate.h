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
        class Isolated;

        class Isolate : public Vca::VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (Isolate, Vca::VRolePlayer);

            friend class Process::Primary;

            friend class Isolated;
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
            Isolate (v8::Isolate *pIsolate);

        //  Destruction
        private:
            ~Isolate ();

        //  Instantiation
        public:
            static bool GetInstance (Reference &rpInstance, v8::Isolate *pIsolate);

        //  Decommisioning
        protected:
            bool onDeleteThis ();                             // ... myself
            bool okToDecommision (Isolated *pIsolated) const; // ... others

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

        //  Handle Helpers
        public:
            template <typename S> v8::Local<S> Local (
                v8::PersistentBase<S> const &rThat
            ) const {
                return v8::Local<S>::New (m_hIsolate, rThat);
            }

        //  New Helpers
        public:
            string_handle_t NewString (char const *pString) const {
                return v8::String::NewFromUtf8 (m_hIsolate, pString);
            }
            resolver_handle_t NewResolver () const;

        //  Exception Helpers
        public:
            void ThrowTypeError (char const *pMessage) const;

        //  Export Access
            bool GetExport (Vxa::export_return_t &rExport, value_handle_t hValue);

        //  Model Management
        public:
            bool Attach (
                ClassTraits<Export>::retaining_ptr_t &rpModelObject, value_handle_t hValue
            );
        private:
            bool Detach (Export *pModelObject);

        //  State
        private:
            global_t        const m_hIsolate;
            object_cache_global_t m_hObjectCache;
        };

    } // namespace VA::Node

} // namespace VA


#endif
