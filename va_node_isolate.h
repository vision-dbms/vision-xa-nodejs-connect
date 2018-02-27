#ifndef VA_Node_Isolate_Interface
#define VA_Node_Isolate_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

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

        class Isolate : public Entity {
            DECLARE_CONCRETE_RTTLITE (Isolate, Entity);

            friend class Process::Primary;

            friend class Isolated;
            friend class Export;

        //  Aliases
        public:
            typedef v8::Isolate object_t;
            typedef object_t*   handle_t;
            typedef object_t*   global_t;

            typedef v8::NativeWeakMap              object_cache_t;
            typedef V8<object_cache_t>::local      object_cache_handle_t;
            typedef V8<object_cache_t>::persistent object_cache_global_t;

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

        //  Access Helpers
        public:
            template <typename handle_t> bool GetString (VString &rString, handle_t hString) const {
                return GetString (rString, hString->ToString (handle ()));
            }
            bool GetString (VString &rString, maybe_string_t hString) const;
            bool GetString (VString &rString, local_string_t hString) const;

        //  Creation Helpers
        public:
            local_resolver_t NewResolver () const;

            local_string_t NewString (char const *pString) const;

        //  Exception Helpers
        public:
            void ThrowTypeError (char const *pMessage) const;

        //  Export Access
            bool GetExport (Vxa::export_return_t &rExport, local_value_t hValue);

        //  Model Management
        public:
            bool Attach (
                ClassTraits<Export>::retaining_ptr_t &rpModelObject, maybe_value_t hValue
            );
            bool Attach (
                ClassTraits<Export>::retaining_ptr_t &rpModelObject, local_value_t hValue
            );
        private:
            bool Detach (Export *pModelObject);

        //  State
        private:
            global_t        const m_hIsolate;
            object_cache_global_t m_hValueCache;
        };

    } // namespace VA::Node

} // namespace VA


#endif
