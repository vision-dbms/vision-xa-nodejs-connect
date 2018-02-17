#include <node.h>
#include <v8.h>

#include "Vk.h"

#include "V_VQueue.h"

#include "Vision_Evaluation_Gofer.h"

#include "va_node_export.h"
#include "va_node_resolver.h"
#include "va_node_triggerable.h"

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

/************************
 *----  ResultSink  ----*
 ************************/

    class ResultSink : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ResultSink, Vca::VRolePlayer);

    //  Aliases
    public:
        typedef VN::Resolver Resolver;

    //  class Resolution
    public:
        class Resolution : public Resolver::Resolution {
            DECLARE_CONCRETE_RTTLITE (Resolution, Resolver::Resolution);

        //  Friends
            friend class ResultSink;

        //  Construction
        public:
            Resolution (
                Resolver *pResolver, VE::Value iResult
            ) : BaseClass (pResolver), m_iResult (iResult) {
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
        class Rejection : public Resolver::Resolution {
            DECLARE_CONCRETE_RTTLITE (Rejection, Resolver::Resolution);

        //  Friends
            friend class ResultSink;

        //  Construction
        public:
            Rejection (
                Resolver *pResolver, Vca::IError *pInterface, VString const &rMessage
            ) : BaseClass (pResolver), m_pInterface (pInterface), m_iMessage (rMessage) {
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

    //  Construction
    public:
        ResultSink (
            Resolver *pResolver, VE::evaluation_result_gofer_t *pResultGofer
        ) : m_pResolver (pResolver) {
            retain (); {
                pResultGofer->supplyMembers (this, &ThisClass::onData, &ThisClass::onError);
            } untain ();
        }

    //  Destruction
    private:
        ~ResultSink () {
        }

    //  Access
    public:
        Resolver::promise_handle_t GetPromise () const {
            return m_pResolver->promise ();
        }

    //  Outcomes
    private:
        void onData (VE::Value iResult) {
            Resolution::Reference const pOutcome (
                new Resolution (m_pResolver, iResult)
            );
            pOutcome->trigger ();
        }
        void onError (Vca::IError *pInterface, VString const &rMessage) {
            Rejection::Reference const pOutcome (
                new Rejection (m_pResolver, pInterface, rMessage)
            );
            pOutcome->trigger ();
        }

    //  State
    private:
        Resolver::Reference m_pResolver;
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

    //  Start the evaluation...
        ResultSink::Reference const pResultSink (
            new ResultSink (new VN::Resolver (pIsolate), pGofer)
        );

    //  And return the promise:
        args.GetReturnValue().Set (pResultSink->GetPromise ());
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
