#ifndef VA_Node_Isolate_Interface
#define VA_Node_Isolate_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

#include "V_VCounter.h"

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
            typedef V::counter32nil_t       call_counter_t;
            typedef call_counter_t::value_t call_count_t;

            typedef v8::Isolate isolate_t;
            typedef isolate_t*  isolate_handle_t;
            typedef isolate_t*  isolate_global_t;

            typedef v8::Map                        object_cache_t;
            typedef V8<object_cache_t>::local      local_object_cache_t;
            typedef V8<object_cache_t>::maybe      maybe_object_cache_t;
            typedef V8<object_cache_t>::persistent persistent_object_cache_t;

            typedef ClassTraits<Export>::retaining_ptr_t ExportReference;
            typedef ClassTraits<Export>::notaining_ptr_t ExportPointer;

        //  class ArgSink
        public:
            class ArgSink : public Vxa::VAny::Client {
            public:
                ArgSink (local_value_t &rResult, Isolate *pIsolate);
                ~ArgSink ();
            public:
                virtual bool on (int iValue) override;
                virtual bool on (double iValue) override;
                virtual bool on (VString const &iValue) override;
                virtual bool on (VCollectableObject *pObject) override;

            private:
                local_value_t& m_rResult;
                Isolate* const m_pIsolate;
            };

        //  class ArgPack
        public:
            class ArgPack {
            //  Construction
            public:
                template <typename ...Ts> ArgPack (
                    Isolate *pIsolate, Ts ...args
                ) : m_aArgs (sizeof... (Ts)) {
                    PunPackRest (pIsolate, m_aArgs.elementArray (), args...);
                }
                ArgPack (Isolate *pIsolate, vxa_pack_t rPack);

            //  PunPack
            private:
                template <typename T, typename... Ts> static void PunPackRest (
                    Isolate *pIsolate, local_value_t *pResult, T tHere, Ts ...tRest
                ) {
                    PunPackHere (pIsolate, *pResult, tHere);
                    PunPackRest (pIsolate, pResult+1,tRest...);
                }
                static void PunPackRest (Isolate*, local_value_t*) {
                }

                static void PunPackHere (Isolate *pIsolate, local_value_t &rResult, local_value_t iT) {
                    rResult = iT;
                }
                static void PunPackHere (Isolate *pIsolate, local_value_t &rResult, int32_t iT) {
                    rResult = pIsolate->NewNumber(iT);
                }
                static void PunPackHere (Isolate *pIsolate, local_value_t &rResult, uint32_t iT) {
                    rResult = pIsolate->NewNumber(iT);
                }
                static void PunPackHere (Isolate *pIsolate, local_value_t &rResult, double iT) {
                    rResult = pIsolate->NewNumber(iT);
                }
                static void PunPackHere (Isolate *pIsolate, local_value_t &rResult, char const *iT) {
                    rResult = pIsolate->NewString(iT);
                }

            //  Destruction
            public:
                ~ArgPack ();

            //  Access
            public:
                int argc () const {
                    return m_aArgs.elementCount ();
                }
                local_value_t *argv () const {
                    return const_cast<local_value_t*>(m_aArgs.elementArray ());
                }

            //  State
            private:
                VkDynamicArrayOf<local_value_t> m_aArgs;
            };

        //  Task Launcher
        public:
            class TaskLauncher;
            bool launchTask (Vxa::VTask *pTask);

        //  Construction
        private:
            Isolate (v8::Isolate *pIsolate);

        //  Destruction
        private:
            ~Isolate ();

        //  Instantiation
        public:
            static bool GetInstance (Reference &rpInstance, v8::Isolate *pIsolate);

        //  Decommissioning
        public:
            bool onShutdown ();
        protected:
            bool onDeleteThis ();                              // ... myself
            bool okToDecommission (Isolated *pIsolated) const; // ... others

        //  Access
        public:
            call_count_t callCount () const {
                return m_iCallCount;
            }

            isolate_handle_t isolate () const {
                return m_hIsolate;
            }
            isolate_handle_t handle () const {
                return m_hIsolate;
            }
            operator isolate_handle_t () const {
                return m_hIsolate;
            }
            local_context_t context () const {
                return m_hIsolate->GetCurrentContext ();
            }

        //  V8 Deprecation Workarounds
        protected:
#if V8_MAJOR_VERSION >= 7 && V8_MINOR_VERSION >= 4
            isolate_handle_t ToBooleanContext () const {
                return isolate ();
            }
#else
            local_context_t ToBooleanContext () const {
                return context ();
            }
#endif

        //  Local Access
        public:
        //  ... isolate constants
            local_primitive_t LocalUndefined () const {
                return v8::Undefined (m_hIsolate);
            }
            local_primitive_t LocalNull () const {
                return v8::Null (m_hIsolate);
            }
            local_boolean_t LocalTrue () const {
                return v8::True (m_hIsolate);
            }
            local_boolean_t LocalFalse () const {
                return v8::False (m_hIsolate);
            }
            bool GetLocalUndefined (local_value_t &rhLocal) const {
                rhLocal = LocalUndefined ();
                return true;
            }
            bool GetLocalNull (local_value_t &rhLocal) const {
                rhLocal = LocalNull ();
                return true;
            }
            bool GetLocalTrue (local_value_t &rhLocal) const {
                rhLocal = LocalTrue ();
                return true;
            }
            bool GetLocalFalse (local_value_t &rhLocal) const {
                rhLocal = LocalFalse ();
                return true;
            }

        //  ... handle -> local handle
            template <typename source_t> typename V8<source_t>::local LocalFor (
                source_t const &rhSource
            ) const {
                return V8<source_t>::local::New (m_hIsolate, rhSource);
            }

        //  ... handle -> local handle (maybe)
            template <typename local_t> bool GetLocalFrom (
                local_t &rhLocal, persistent_value_t const &hValue
            ) const {
                return GetLocalFrom (rhLocal, LocalFor (hValue));
            }
            template <typename local_t> bool GetLocalFrom (
                local_t &rhLocal, local_value_t const &hValue
            ) const {
                return false;
            }

        //  ... handle -> local handle (maybe) implementations
            bool GetLocalFrom (local_value_t &rhLocal, local_value_t const &hValue) const {
                rhLocal = hValue;
                return true;
            }

            bool GetLocalFrom (local_boolean_t &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToBoolean (ToBooleanContext ()));
            }
            bool GetLocalFrom (local_function_t &rhLocal, local_value_t hValue) const {
                if (hValue->IsFunction ()) {
                    rhLocal = local_function_t::Cast (hValue);
                    return true;
                }
                return false;
            }
            bool GetLocalFrom (local_integer_t &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToInteger (context ()));
            }
            bool GetLocalFrom (V8<v8::Int32>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToInt32 (context ()));
            }
            bool GetLocalFrom (local_number_t &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToNumber (context ()));
            }
            bool GetLocalFrom (local_object_t &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToObject (context ()));
            }
            bool GetLocalFrom (local_string_t &rhLocal, local_value_t hValue, bool bDetailed) const {
                return GetLocalFor (
                    rhLocal,
                    bDetailed ? hValue->ToDetailString (context ()) : hValue->ToString (context ())
                );
            }
            bool GetLocalFrom (V8<v8::Uint32>::local &rhLocal, local_value_t hValue) const {
                return GetLocalFor (rhLocal, hValue->ToUint32 (context ()));
            }

        //  Access Helpers
        public:
            template <typename unwrapped_t, typename handle_t> bool GetUnwrapped (
                unwrapped_t &rUnwrapped, handle_t hValue
            ) const {
                local_value_t hLocalValue;
                return GetLocalFor (hLocalValue, hValue) && GetUnwrapped (
                    rUnwrapped, hLocalValue
                );
            }

            bool GetUnwrapped (bool    &rUnwrapped, local_value_t hValue) const;
            bool GetUnwrapped (double  &rUnwrapped, local_value_t hValue) const;
            bool GetUnwrapped (int32_t &rUnwrapped, local_value_t hValue) const;

            template <typename handle_t> bool UnwrapString (
                VString &rUnwrapped, handle_t hValue, bool bDetail = false
            ) const {
                local_value_t hLocalValue;
                return GetLocalFor (hLocalValue, hValue) && UnwrapString (
                    rUnwrapped, hLocalValue, bDetail
                );
            }
            bool UnwrapString (
                VString &rUnwrapped, local_value_t hValue, bool bDetail = false
            ) const;
            bool UnwrapString (VString &rUnwrapped, maybe_string_t hString) const;
            bool UnwrapString (VString &rUnwrapped, local_string_t hString) const;

        //  Creation Helpers
        public:
            bool NewString (local_string_t &rResult, VString const &rString) const;
            local_string_t NewString (VString const &rString) const;

            local_integer_t NewNumber (int iNumber) const {
                return integer_t::New (handle (), iNumber);
            }
            local_integer_t NewNumber (unsigned int iNumber) const {
                return integer_t::NewFromUnsigned (handle (), iNumber);
            }
            local_number_t  NewNumber (double iNumber) const {
                return number_t::New (handle (), iNumber);
            }
            local_object_t NewObject () const {
                return object_t::New (handle ());
            }
            local_external_t NewExternal (void *pData) const {
                return external_t::New (handle (), pData);
            }

        //  Exception Helpers
        public:
            void ThrowTypeError (VString const &rMessage) const;

        //  Export Access
            bool GetExport (Vxa::export_return_t &rExport, local_value_t hValue);

        //  Model Management
        public:
            bool Attach (
                ExportReference &rpModelObject, maybe_value_t hValue
            );
            bool Attach (
                ExportReference &rpModelObject, local_value_t hValue
            );
        private:
            bool Detach (Export *pModelObject);

        //  Result Return
        public:
        /*--------------------------*
         *----  Maybe Callback  ----*
         *--------------------------*/
            template<typename result_t,typename callable_t,typename... arg_ts> bool MaybeSetResultToCall (
                result_t &rResult,
                node::async_context aContext,
                local_object_t hReceiver,
                callable_t hCallable,
                arg_ts ...args
            ) {
                return MaybeSetResultToApply (
                    rResult, aContext, hReceiver, hCallable, ArgPack (this, args...)
                );
            }
            template <typename result_t, typename callable_t> bool MaybeSetResultToApply (
                result_t &rResult,
                node::async_context aContext,
                local_object_t hReceiver,
                callable_t hCallable,
                ArgPack const &rArgs
            ) {
                local_function_t hFunction;
                return GetLocalFrom (hFunction, hCallable) && MaybeSetResultToApply (
                    rResult, aContext, hReceiver, hFunction, rArgs
                );
            }
            template <typename result_t> bool MaybeSetResultToApply (
                result_t &rResult,
                node::async_context aContext,
                local_object_t hReceiver,
                local_function_t hFunction,
                ArgPack const &rArgs
            ) {
                v8::TryCatch iCatcher (*this);
                return MaybeSetResultToValue (
                    rResult, node::MakeCallback (
                        handle (), hReceiver, hFunction, rArgs.argc (), rArgs.argv (), aContext
                    )
                );
            }

        /*----------------------*
         *----  Maybe Call  ----*
         *----------------------*/
            template <typename result_t, typename callable_t, typename... arg_ts> bool MaybeSetResultToCall (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, arg_ts ...args
            ) {
                return MaybeSetResultToApply (rResult, hReceiver, hCallable, ArgPack (this, args...));
            }

        public:
        /*-----------------------*
         *----  Maybe Apply  ----*
         *-----------------------*/
            template <typename result_t, typename callable_t> bool MaybeSetResultToApply (
                result_t &rResult,
                local_value_t hReceiver,
                callable_t hCallable,
                vxa_pack_t rPack,
                bool bConstructorCall = false
            ) {
                return MaybeSetResultToApply (
                    rResult, hReceiver, hCallable, ArgPack (this, rPack), bConstructorCall
                );
            }

            template <typename result_t, typename callable_t> bool MaybeSetResultToApply (
                result_t &rResult,
                local_value_t hReceiver,
                callable_t hCallable,
                ArgPack const &rArgs,
                bool bConstructorCall = false
            ) {
                local_value_t hLocalCallable;
                return GetLocalFor (hLocalCallable, hCallable)
                    && MaybeSetResultToApply (rResult, hReceiver, hLocalCallable, rArgs, bConstructorCall);
            }

            template <typename result_t> bool MaybeSetResultToApply (
                result_t &rResult,
                local_value_t hReceiver,
                local_value_t hCallable,
                ArgPack const &rArgs,
                bool bConstructorCall = false
            ) {
                return MaybeSetResultToApplyOf<result_t,local_function_t> (
                    rResult, hReceiver, hCallable, rArgs, bConstructorCall
                ) || MaybeSetResultToApplyOf<result_t,local_object_t> (
                    rResult, hReceiver, hCallable, rArgs, bConstructorCall
                );
            }

            template <typename result_t> bool MaybeSetResultToApply (
                result_t &rResult,
                local_value_t hReceiver,
                local_function_t hCallable,
                ArgPack const &rArgs,
                bool bConstructorCall = false
            ) {
                m_iCallCount++;
                v8::TryCatch iCatcher (*this);
                return (
                    bConstructorCall ? MaybeSetResultToNewInstance (
                        rResult, hCallable, rArgs
                    ) : MaybeSetResultToValue (
                        rResult, hCallable->Call (context (), hReceiver, rArgs.argc (), rArgs.argv ())
                    )
                ) || MaybeSetResultToError (rResult, iCatcher);
            }
            template <typename result_t> bool MaybeSetResultToNewInstance (
                result_t &rResult, local_function_t hCallable, ArgPack const &rArgs
            ) {
                local_object_t hNewInstance;
                return GetLocalFor (
                    hNewInstance, hCallable->NewInstance (context (), rArgs.argc (), rArgs.argv ())
                ) && MaybeSetResultToValue (rResult, v8::Local<value_t> (hNewInstance));
            }

            template <typename result_t> bool MaybeSetResultToApply (
                result_t &rResult,
                local_value_t hReceiver,
                local_object_t hCallable,
                ArgPack const &rArgs,
                bool bConstructorCall = false
            ) {
                m_iCallCount++;
                v8::TryCatch iCatcher (*this);
                return hCallable->IsCallable () && (
                    MaybeSetResultToValue (
                        rResult, bConstructorCall ? hCallable->CallAsConstructor (
                            context (), rArgs.argc (), rArgs.argv ()
                        ) : hCallable->CallAsFunction (
                            context (), hReceiver, rArgs.argc (), rArgs.argv ()
                        )
                    ) || MaybeSetResultToError (rResult, iCatcher)
                );
            }

            template <typename result_t, typename cast_callable_t> bool MaybeSetResultToApplyOf (
                result_t &rResult,
                local_value_t hReceiver,
                local_value_t hCallable,
                ArgPack const &rArgs,
                bool bConstructorCall = false
            ) {
                cast_callable_t hCastCallable;
                return GetLocalFrom (hCastCallable, hCallable)
                    && MaybeSetResultToApply (rResult, hReceiver, hCastCallable, rArgs, bConstructorCall);
            }

        /*---------------------*
         *----  Maybe New  ----*
         *---------------------*/
            template <typename result_t, typename constructable_t, typename... arg_ts> bool MaybeSetResultToNewInstance (
                result_t &rResult, constructable_t hConstructable, arg_ts ...args
            ) {
                return MaybeSetResultToNewInstance (rResult, hConstructable, ArgPack (this, args...));
            }

            template <typename result_t, typename constructable_t> bool MaybeSetResultToNewInstance (
                result_t &rResult, constructable_t hConstructable, vxa_pack_t rPack
            ) {
                return MaybeSetResultToNewInstance (rResult, hConstructable, ArgPack (this, rPack));
            }

            template <typename result_t, typename constructable_t> bool MaybeSetResultToNewInstance (
                result_t &rResult, constructable_t hConstructable, ArgPack const &rArgs
            ) {
                local_value_t hUndefined;
                return GetLocalUndefined (hUndefined) && MaybeSetResultToApply (
                    rResult, hUndefined, hConstructable, rArgs, true
                );
            }

        /*-----------------------*
         *----  Maybe Error  ----*
         *-----------------------*/
            template <typename result_t> bool MaybeSetResultToError (
                result_t &rResult, v8::TryCatch &rCatcher
            ) {
                return rCatcher.HasCaught () && MaybeSetResultToValue (
                    rResult, rCatcher.Exception ()
                );
            }

        /*-----------------------*
         *----  Maybe Value  ----*
         *-----------------------*/
            template <typename result_t, typename handle_t> bool MaybeSetResultToValue (
                result_t &rResult, handle_t hValue
            ) {
                local_value_t hLocalValue;
                return GetLocalFor (hLocalValue, hValue)
                    && MaybeSetResultToValue (rResult, hLocalValue);
            }

            bool MaybeSetResultToValue (
                local_value_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToValue (
                vxa_result_t &rResult, local_value_t hValue
            );

            bool MaybeSetResultToInt32 (
                vxa_result_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToDouble (
                vxa_result_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToString (
                vxa_result_t &rResult, local_value_t hValue
            );
            bool MaybeSetResultToObject (
                vxa_result_t &rResult, local_value_t hValue
            );

        /*--------------------------*
         *----  SetResultTo...  ----*
         *--------------------------*/
            template <typename result_t, typename callable_t, typename... arg_ts> bool SetResultToCall (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, arg_ts ...args
            ) {
                return MaybeSetResultToCall (rResult, hReceiver, hCallable, args...)
                    || SetResultToUndefined (rResult);
            }
            template <typename result_t, typename callable_t, typename pack_t> bool SetResultToApply (
                result_t &rResult, local_value_t hReceiver, callable_t hCallable, pack_t &rPack
            ) {
                return MaybeSetResultToApply (rResult, hReceiver, hCallable, rPack)
                    || SetResultToUndefined (rResult);
            }

            template <typename result_t, typename constructable_t, typename... arg_ts> bool SetResultToNewInstance (
                result_t &rResult, constructable_t hConstructable, arg_ts ...args
            ) {
                return MaybeSetResultToNewInstance (rResult, hConstructable, args...)
                    || SetResultToUndefined (rResult);
            }
            template <typename result_t, typename constructable_t, typename pack_t> bool SetResultToNewInstance (
                result_t &rResult, constructable_t hConstructable, pack_t &rPack
            ) {
                return MaybeSetResultToNewInstance (rResult, hConstructable, rPack)
                    || SetResultToUndefined (rResult);
            }

            template <typename result_t, typename handle_t> bool SetResultToValue (
                result_t &rResult, handle_t hValue
            ) {
                return MaybeSetResultToValue (rResult, hValue) || SetResultToUndefined (rResult);
            }

            template <typename result_t> bool SetResultToGlobal (result_t &rResult) {
                local_value_t hGlobal = context()->Global ();
                return SetResultToValue (rResult, hGlobal);
            }

            bool SetResultToUndefined (local_value_t &rResult);
            bool SetResultToUndefined (vxa_result_t &rResult);

        /****************************************
         *----  Caching Function Factories  ----*
         ****************************************/
        public:
            bool GetTaskLaunchFunction (local_function_t &rResult, v8::FunctionCallback callback) {
                return GetCachedFunction (rResult, m_hTaskLaunchFT, callback);
            }

            bool GetCachedFunction (
                local_function_t               &rResult,
                persistent_function_template_t &rCached,
                v8::FunctionCallback            callback
            );
            bool GetCachedFunctionTemplate (
                local_function_template_t      &rResult,
                persistent_function_template_t &rCached,
                v8::FunctionCallback            callback
            );

        //  State
        private:
            isolate_handle_t const         m_hIsolate;
            persistent_object_cache_t      m_hValueCache;
            call_counter_t                 m_iCallCount;
            persistent_function_template_t m_hTaskLaunchFT;
        };

    } // namespace VA::Node

} // namespace VA


#endif
