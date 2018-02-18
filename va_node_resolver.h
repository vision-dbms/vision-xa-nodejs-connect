#ifndef VA_Node_Resolver_Interface
#define VA_Node_Resolver_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_isolated.h"
#include "va_node_triggerable.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
    /*****************************
     *----  class Resolver  -----*
     *****************************/
        class Resolver : public Isolated {
            DECLARE_CONCRETE_RTTLITE (Resolver, Isolated);

        //  Aliases
        public:
            typedef v8::Promise promise_t;
            typedef v8::Local<promise_t> promise_handle_t;

            typedef promise_t::Resolver resolver_t;
            typedef v8::Local<resolver_t>      handle_t;
            typedef v8::Persistent<resolver_t> global_t;

        //  Resolution
        public:
            class Resolution;
            friend class Resolution;

        //  Construction
        public:
            Resolver (Isolate *pIsolate);

        //  Destruction
        private:
            ~Resolver ();

        //  Decommissioning
        protected:
            virtual bool decommision () override;

        //  Access
        public:
            promise_handle_t promise () const {
                return resolver ()->GetPromise ();
            }
        private:
            handle_t resolver () const {
                return GetLocal<resolver_t> (m_hResolver);
            }

        //  Outcome
        private:
            template <typename R> void Resolve (R iR) const {
                resolver ()->Resolve (iR);
            }
            template <typename R> void Reject (R iR) const {
                resolver ()->Reject (iR);
            }

        //  State
        private:
            global_t m_hResolver;
        };


    /*****************************************
     *----  class Resolver::Resolution  -----*
     *****************************************/

        class Resolver::Resolution : public Triggerable {
            DECLARE_ABSTRACT_RTTLITE (Resolution, Triggerable);

        //  Construction
        protected:
            Resolution (Resolver *pResolver);

        //  Destruction
        protected:
            ~Resolution ();

        //  Outcome
        protected:
            template <typename R> void Resolve (R iR) const {
                m_pResolver->Resolve (iR);
            }
            template <typename R> void Reject (R iR) const {
                m_pResolver->Reject (iR);
            }

        //  State
        private:
            Resolver::Reference const m_pResolver;
        };

    } // namespace VA::Node

} // namespace VA


#endif
