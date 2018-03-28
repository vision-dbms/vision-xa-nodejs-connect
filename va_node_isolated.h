#ifndef VA_Node_Isolated_Interface
#define VA_Node_Isolated_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

#include "va_node_isolate.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Isolated : public Entity {
            DECLARE_ABSTRACT_RTTLITE (Isolated, Entity);

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
            Isolated (Isolate *pIsolate);

        //  Destruction
        protected:
            ~Isolated ();

        //  Decommisioning
/************************************************************************************
 *  An 'Isolated's destructor may be called from any thread.  As of this writing,
 *  it is not safe to destroy 'node', 'v8' or 'uv' objects (e.g., 'Reset' a
 *  'v8::Persistent', 'uv_close' a 'uv_async_t' handle, etc.) on any thread other
 *  than the node/uv main thread.  While these resources are not currently subject
 *  to automatic destruction ("uv.h" structs are C objects, 'v8::Persistent' handles
 *  are not automatically reset on destruction (see the comments assocated with
 *  'kResetInDestructor' in "v8.h"), the apparent quiet 'success' of destructors
 *  that should be releasing resources comes at the cost of leaked memory.  To
 *  address these issues, this class provides support for a two phase destruction
 *  process (* SEE BELOW *) that arranges for the reclamation of resources in the
 *  correct thread. To take advantage of that process, every derived class with
 *  resources to reclaim must define a 'protected' ('private' is ok for 'final'
 *  classes) virtual override of this class' 'decommision' member that releases
 *  the resources allocated by this class and calls the base class version of
 *  'decommision':
 *  
 *    bool VA::Node::DerivedClass::decommission () {
 *    //  Free the resources held by this instance ...
 *        m_hSomething.Reset ();
 *        somethingMoreComplicated ();
 *        etc ();
 *
 *    //  Call the base class' version of 'decommision':
 *        return BaseClass::decommision ();
 *    }
 *
 *--------------------------------------------------------------------------------
 *
 *  (*) IMPORTANT IMPLEMENTATION NOTE (*)
 *
 *  The decommisioning process is implemented using the 'onDeleteThis' hook provided
 *  by the object lifetime management infrastructure inherited from 'Isolated's base
 *  class(es).  That hook gives derived classes an opportunity to approve an object's
 *  destruction (by returning 'true') or to defer or cancel that destruction (by
 *  returning 'false' after presumably making alternative arrangements) as is done
 *  here:
 *
 *    bool VA::Node::Isolated::onDeleteThis () {
 *        return m_pIsolate->okToDecommision (this) && decommision ();
 *    }
 *
 ************************************************************************************/
        protected:
            bool onDeleteThis ();
            virtual bool decommision ();

        //  Access
        public:
            Isolate *isolate () const {
                return m_pIsolate;
            }
            v8::Isolate *isolateHandle () const {
                return m_pIsolate->handle ();
            }
            local_context_t context () const {
                return m_pIsolate->context ();
            }

        //  Model Management
        protected:
            template <typename handle_t> bool Attach (
                ClassTraits<Export>::retaining_ptr_t &rpModelObject, handle_t hValue
            ) const {
                return m_pIsolate->Attach (rpModelObject, hValue);
            }

        //  Local Access
        protected:
        //  ... source handle -> local handle
            template <typename source_t> typename V8<source_t>::local LocalFor (source_t const &rHandle) const {
                return m_pIsolate->LocalFor (rHandle);
            }

        //  ... source handle -> local handle (maybe)
            template <typename local_t, typename source_t> bool GetLocalFrom (
                local_t &rhLocal, source_t const &rhSource
            ) const {
                return m_pIsolate->GetLocalFrom (rhLocal, rhSource);
            }

        //  Object Access
        protected:
            template <typename unwrapped_t, typename handle_t> bool GetUnwrapped (
                unwrapped_t &rUnwrapped, handle_t hValue
            ) const {
                return m_pIsolate->GetUnwrapped (rUnwrapped, hValue);
            }
            template <typename handle_t> bool UnwrapString (
                VString &rString, handle_t hString, bool bDetail = false
            ) const {
                return m_pIsolate->UnwrapString (rString, hString, bDetail);
            }

        //  Object Creation
        protected:
            local_string_t NewString (char const *pString) const {
                return m_pIsolate->NewString (pString);
            }

        //  Result Return
        public:
        /*----------------------*
         *----  Maybe Call  ----*
         *----------------------*/
            template <typename handle_t> bool MaybeSetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, handle_t hCallable, vxa_pack_t rPack
            ) {
                return m_pIsolate->MaybeSetResultToCall (rResult, hReceiver, hCallable, rPack);
            }

        /*-----------------------*
         *----  Maybe Value  ----*
         *-----------------------*/
            template <typename handle_t> bool MaybeSetResultToValue (
                vxa_result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToValue (rResult, hValue);
            }
            template <typename handle_t> bool MaybeSetResultToInt32 (
                vxa_result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToInt32 (rResult, hValue);
            }
            template <typename handle_t> bool MaybeSetResultToDouble (
                vxa_result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToDouble (rResult, hValue);
            }
            template <typename handle_t> bool MaybeSetResultToString (
                vxa_result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToString (rResult, hValue);
            }
            template <typename handle_t> bool MaybeSetResultToObject (
                vxa_result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->MaybeSetResultToObject (rResult, hValue);
            }

        /****************************
         *----  SetResultTo...  ----*
         ****************************/
            template <typename handle_t> bool SetResultToCall (
                vxa_result_t &rResult, local_value_t hReceiver, handle_t hCallable, vxa_pack_t rPack
            ) {
                return m_pIsolate->SetResultToCall (rResult, hReceiver, hCallable, rPack);
            }
            template <typename handle_t> bool SetResultToValue (
                vxa_result_t &rResult, handle_t hValue
            ) const {
                return m_pIsolate->SetResultToValue (rResult, hValue);
            }

            bool SetResultToGlobal (vxa_result_t &rResult) {
                return m_pIsolate->SetResultToGlobal (rResult);
            }
            bool SetResultToUndefined (vxa_result_t &rResult) {
                return m_pIsolate->SetResultToUndefined (rResult);
            }

        //  Task Launcher
        public:
            static bool launchTask (Vxa::VTask *pTask);

        //  State
        private:
            Isolate::Reference const m_pIsolate;
        };
    } // namespace VA::Node
} // namespace VA


#endif
