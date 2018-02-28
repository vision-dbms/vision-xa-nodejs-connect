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
            typedef V8<promise_t>::local local_promise_t;

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
            local_promise_t promise () const {
                return resolver ()->GetPromise ();
            }
        private:
            local_resolver_t resolver () const {
                return GetLocal (m_hResolver);
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
            persistent_resolver_t m_hResolver;
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
