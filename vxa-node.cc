#include <node.h>
#include <v8.h>
#include <uv.h>

#include "Vk.h"

#include "V_VQueue.h"

#include "Vision_Evaluation_Gofer.h"

#include "va_node_process.h"
#include "va_node_isolate.h"
#include "va_node_export.h"

#include "Vca_VcaGofer.h"
#include "Vca_IDirectory.h"
#include "Vca_IPipeFactory.h"

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
    namespace VN = VA::Node;

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

    //  Aliases
    public:
        typedef VN::Isolate Isolate;
        typedef VN::value_handle_t value_handle_t;
        typedef VN::string_handle_t string_handle_t;

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
        v8::Isolate *isolateHandle () const {
            return m_pIsolate->handle ();
        }
        Local<Context> context () const {
            return m_pIsolate->getCurrentContext ();
        }

    //  Handle Cast
    protected:
        template <typename S> v8::Local<S> GetLocal (v8::PersistentBase<S> const &rThat) const {
            return m_pIsolate->Local (rThat);
        }

    //  Value Creation Helpers
    protected:
        string_handle_t NewString (char const *pString) const {
            return m_pIsolate->NewString (pString);
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
            HandleScope iScope (isolateHandle ());
            run ();
            release ();  // ... reference created by 'schedule' removed here.
        }
        virtual void run () = 0;

    //  State
    private:
        Isolate::Reference const m_pIsolate;
        uv_async_t m_iNodeMessage;
    }; // class Queueable

/************************
 *----  ResultSink  ----*
 ************************/

    class ResultSink : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ResultSink, Vca::VRolePlayer);

    //  Aliases
    public:
        typedef VN::Isolate Isolate;
        typedef Promise::Resolver resolver_t;

        typedef Local<resolver_t>      resolver_handle_t;
        typedef Persistent<resolver_t> resolver_global_t;

    //  class Resolver
    public:
        class Resolver : public Queueable {
            DECLARE_ABSTRACT_RTTLITE (Resolver, Queueable);

        //  Construction
        protected:
            Resolver (
                Isolate *pIsolate, resolver_global_t const &rhResolver
            ) : BaseClass (pIsolate), m_hResolver (*pIsolate, rhResolver) {
            }

        //  Destruction
        protected:
            ~Resolver () {
            }

        //  Access
        private:
            Local<resolver_t> resolver () const {
                return GetLocal<resolver_t> (m_hResolver);
            }

        //  Resolution
        protected:
            template <typename R> void Resolve (R iR) const {
                resolver ()->Resolve (iR);
            }
            template <typename R> void Reject (R iR) const {
                resolver ()->Reject (iR);
            }

        //  State
        private:
            resolver_global_t m_hResolver;
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
                Isolate *pIsolate, resolver_global_t const &rhResolver, VE::Value iResult
            ) : Resolver (pIsolate, rhResolver), m_iResult (iResult) {
            }

        //  Destruction
        private:
            ~Resolution () {
            }

        //  Execution
        private:
            void run () override {
                Resolve (NewString (m_iResult.output ()));
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
                resolver_global_t const &rhResolver,
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
                Reject (NewString (m_iMessage));
            }

        //  State
        private:
            Vca::IError::Reference m_pInterface;
            VString const          m_iMessage;
        }; // class ResultSink::Rejection
        
    public:
        ResultSink (
            Isolate *pIsolate,
            resolver_handle_t hResolver,
            VE::evaluation_result_gofer_t *pResultGofer
        ) : m_pIsolate (pIsolate), m_hResolver (*pIsolate, hResolver) {
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
        VN::Isolate::Reference m_pIsolate;
        resolver_global_t m_hResolver;
    };

/**********************
 *----  Evaluate  ----*
 **********************/
    void Evaluate(const FunctionCallbackInfo<Value>& args) {
        Vca::VCohortClaim cohortClaim;

        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate());

    //  Access the required expression to evaluate...
        if (args.Length () < 1) {
            pIsolate->ThrowTypeError ("Missing Expression");
            return;
        }
        MaybeLocal<String> str = args[0]->ToString (pIsolate->isolate ());
        String::Utf8Value pExpression(str.ToLocalChecked ());

    //  Access the client context if supplied...
        Vxa::export_return_t iExport;
        if (args.Length () >= 2) {
            pIsolate->GetExport (iExport, args[1]);
        }

    //  Set up the evaluation...
        VE::Gofer::Reference const pGofer (
            new VE::Gofer (DefaultEvaluator (), *pExpression, iExport)
        );

    //  Create the promise ...
        MaybeLocal<Promise::Resolver> mResolver = Promise::Resolver::New (pIsolate->getCurrentContext ());
        Local<Promise::Resolver> hResolver = mResolver.ToLocalChecked ();

    //  Start the evaluation...
        ResultSink::Reference const pResultSink (new ResultSink (pIsolate, hResolver, pGofer));

    //  Return the promise
        args.GetReturnValue().Set(hResolver->GetPromise ());
    }

/**************************
 *----  State Access  ----*
 **************************/

    void CachedIsolateCount (const FunctionCallbackInfo<Value>& args) {
        VA::Node::Process::Reference pNodeProcess;
        args.GetReturnValue ().Set (
            static_cast<uint32_t>(
                VA::Node::Process::GetInstance (pNodeProcess) ? pNodeProcess->isolateCacheSize () : 0
            )
        );
    }

/**********************************
 *----  Module Initialization ----*
 **********************************/
    void Init(Local<Object> exports, Local<Object> module) {
        NODE_SET_METHOD(exports, "v", Evaluate);
        NODE_SET_METHOD(exports, "cachedIsolateCount", CachedIsolateCount);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
