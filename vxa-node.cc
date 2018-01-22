#include <node.h>
#include <v8.h>
#include <uv.h>

#include "Vk.h"

#include "V_VQueue.h"

#include "Vision_Evaluation_Gofer.h"

#include "Vca_VcaGofer.h"
#include "Vca_IDirectory.h"
#include "Vca_IPipeFactory.h"

#include <iostream>

// #include "Vision_Peer_NodeJS.h"

namespace {

    using v8::Context;
    using v8::Exception;
    using v8::FunctionCallbackInfo;
    using v8::HandleScope;
    using v8::Isolate;
    using v8::Local;
    using v8::MaybeLocal;
    using v8::Object;
    using v8::Persistent;
    using v8::Promise;
    using v8::String;
    using v8::Value;

/*********************
 *----  Helpers  ----*
 *********************/
    namespace VE = Vision::Evaluation;

    char const *ValueOrElse (char const *pName, char const *pElse) {
        char const *pValue = getenv (pName);
        return pValue ? pValue : pElse;
    }
    VE::evaluator_gofer_t *DefaultEvaluator () {
        static VE::evaluator_gofer_t::Reference const pGofer (
        //  new Vca::Gofer::Named<Vsa::IEvaluator,Vca::IDirectory> (ValueOrElse ("VxaEvaluator", "Default"))
            new Vca::Gofer::Named<Vsa::IEvaluator,Vca::IPipeFactory> (ValueOrElse ("VxaEvaluator", "8765"))
        );
        return pGofer;
    }

/***********************
 *----  Queueable  ----*
 ***********************/

    class Queueable : public VReferenceable {
        DECLARE_ABSTRACT_RTTLITE (Queueable, VReferenceable);

    //  Construction
    protected:
        Queueable (Isolate *pIsolate) : m_pIsolate (pIsolate) {
            assert (uv_async_init (uv_default_loop (), &m_iNodeMessage, &ThisClass::Run) == 0);
            m_iNodeMessage.data = this;
        }

    //  Destruction
        ~Queueable () {
        /*****************************************************************************************
         *  This destructor may be called from any Vca thread or, if we're lucky, from node's main
         *  thread.  Since the 'uv' library isn't thread safe, it's only safe to run the following
         *  line of code in node's main thread to avoid random crashes.  Make sure that happens!!!
         *****************************************************************************************/
            uv_close (reinterpret_cast<uv_handle_t*>(&m_iNodeMessage), NULL);
        }

    //  Access
    protected:
        Isolate *isolate () const {
            return m_pIsolate;
        }
        Local<Context> context () const {
            return m_pIsolate->GetCurrentContext ();
        }

    //  Scheduling
    protected:
        void schedule () {
            retain (); // ... reference removed below after this queueable has been 'Run'
            uv_async_send (&m_iNodeMessage);
        }

    //  Execution
    private:
        static void Run (uv_async_t *pHandle) {
            Reference const pQueueable(static_cast<ThisClass*>(pHandle->data));
            if (pQueueable) {
                pQueueable->Run ();
            }
        }
        void Run () {
            HandleScope iScope (isolate ());
            run ();
            release ();  // ... reference created by 'schedule' removed here.
        }
        virtual void run () = 0;

    //  State
    private:
        uv_async_t m_iNodeMessage;
        Isolate *const m_pIsolate;
    }; // class Queueable

/************************
 *----  ResultSink  ----*
 ************************/

    class ResultSink : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ResultSink, Vca::VRolePlayer);

    //  Aliases
    public:
        typedef Promise::Resolver resolver_t;
        typedef Persistent<resolver_t> resolver_handle_t;

    //  class Resolver
    public:
        class Resolver : public Queueable {
            DECLARE_ABSTRACT_RTTLITE (Resolver, Queueable);

        //  Construction
        protected:
            Resolver (
                Isolate *pIsolate, resolver_handle_t const &rhResolver
            ) : BaseClass (pIsolate), m_hResolver (pIsolate, rhResolver) {
            }

        //  Destruction
        protected:
            ~Resolver () {
            }

        //  Access
        protected:
            Local<resolver_t> resolver () const {
                return Local<resolver_t>::New (isolate (), m_hResolver);
            }

        //  State
        private:
            resolver_handle_t m_hResolver;
        };

    //  class Resolution
    public:
        class Resolution : public Resolver {
            DECLARE_CONCRETE_RTTLITE (Resolution, Resolver);

        //  Friends
            friend class ResultSink;

        //  Construction
        public:
            Resolution (
                Isolate *pIsolate, resolver_handle_t const &rhResolver, VE::Value iResult
            ) : Resolver (pIsolate, rhResolver), m_iResult (iResult) {
            }

        //  Destruction
        private:
            ~Resolution () {
            }

        //  Execution
        private:
            void run () override {
                resolver()->Resolve (String::NewFromUtf8 (isolate (), m_iResult.output ()));
            }

        //  State
        private:
            VE::Value const m_iResult;
        }; // class ResultSink::Resolution

    //  class Rejection
    public:
        class Rejection : public Resolver {
            DECLARE_CONCRETE_RTTLITE (Rejection, Resolver);

        //  Friends
            friend class ResultSink;

        //  Construction
        public:
            Rejection (
                Isolate *pIsolate,
                resolver_handle_t const &rhResolver,
                Vca::IError *pInterface,
                VString const &rMessage
            ) : Resolver (pIsolate, rhResolver), m_pInterface (pInterface), m_iMessage (rMessage) {
            }

        //  Destruction
        private:
            ~Rejection () {
            }

        //  Execution
        private:
            void run () override {
                resolver()->Reject (String::NewFromUtf8 (isolate (), m_iMessage));
            }

        //  State
        private:
            Vca::IError::Reference m_pInterface;
            VString const          m_iMessage;
        }; // class ResultSink::Rejection
        
    public:
        ResultSink (
            Isolate *pIsolate,
            Local<resolver_t> hResolver,
            VE::evaluation_result_gofer_t *pResultGofer
        ) : m_pIsolate (pIsolate), m_hResolver (pIsolate, hResolver) {
            retain (); {
                pResultGofer->supplyMembers (this, &ThisClass::onData, &ThisClass::onError);
            } untain ();
        }
    private:
        ~ResultSink () {
        }

    private:
        void onData (VE::Value iResult) {
            Resolution::Reference const pQueueable (
                new Resolution (m_pIsolate, m_hResolver, iResult)
            );
            pQueueable->schedule ();
        }
        void onError (Vca::IError *pInterface, VString const &rMessage) {
            Rejection::Reference const pQueueable (
                new Rejection (m_pIsolate, m_hResolver, pInterface, rMessage)
            );
            pQueueable->schedule ();
        }

    //  State
    private:
        Isolate *const    m_pIsolate;
        resolver_handle_t m_hResolver;
    };

/**********************
 *----  Evaluate  ----*
 **********************/
    void Evaluate(const FunctionCallbackInfo<Value>& args) {
        Vca::VCohortClaim cohortClaim;

        Isolate* const pIsolate = args.GetIsolate();

    //  Access the required expression to evaluate...
        if (args.Length () < 1) {
            pIsolate->ThrowException (
                Exception::TypeError (String::NewFromUtf8 (pIsolate, "Missing Expression"))
            );
            return;
        }
        MaybeLocal<String> str = args[0]->ToString (pIsolate);
        String::Utf8Value pExpression(str.ToLocalChecked ());

    //  Access the client context if supplied...
        if (args.Length () >= 2) {
        }

    //        Local<Object> obj = Object::New(pIsolate);
    //        obj->Set(String::NewFromUtf8(pIsolate, "context"), cc);
    //        obj->Set(String::NewFromUtf8(pIsolate, "command"), str);
    //        args.GetReturnValue().Set(obj);

    //  Set up the evaluation...
        VE::Gofer::Reference const pGofer (
            new VE::Gofer (DefaultEvaluator (), *pExpression, static_cast<VE::context_t*>(0))
        );

    //  Create the promise ...
        MaybeLocal<Promise::Resolver> mResolver = Promise::Resolver::New (pIsolate->GetCurrentContext ());
        Local<Promise::Resolver> hResolver = mResolver.ToLocalChecked ();

    //  Start the evaluation...
        ResultSink::Reference const pResultSink (new ResultSink (pIsolate, hResolver, pGofer));

    //  Return the promise
        args.GetReturnValue().Set(hResolver->GetPromise ());
    }

/**********************************
 *----  Module Initialization ----*
 **********************************/
    void Init(Local<Object> exports, Local<Object> module) {
        NODE_SET_METHOD(module, "exports", Evaluate);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
