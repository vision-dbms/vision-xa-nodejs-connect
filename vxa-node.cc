#include <node.h>
#include <v8.h>
#include <uv.h>

#include "Vk.h"

#include "Vision_Evaluation_Gofer.h"

#include "Vca_VcaGofer.h"
#include "Vca_IDirectory.h"
#include "Vca_IPipeFactory.h"

#include <iostream>

// #include "Vision_Peer_NodeJS.h"

namespace {

    using v8::FunctionCallbackInfo;
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

    protected:
        Queueable (Isolate *pIsolate) : m_pIsolate (pIsolate) {
        }
        ~Queueable () {
        }

    protected:
        Isolate *isolate () const {
            return m_pIsolate;
        }

    public:
        void enqueue () {
            execute ();
        }
    private:
        virtual void execute () = 0;

    private:
        Isolate *const m_pIsolate;
    };

/************************
 *----  ResultSink  ----*
 ************************/

    class ResultSink : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ResultSink, Vca::VRolePlayer);

    //  Aliases
    public:
        typedef Persistent<Promise::Resolver> resolver_handle_t;

    //  class ResolveQueueable
    public:
        class ResolveQueueable : public Queueable {
            DECLARE_CONCRETE_RTTLITE (ResolveQueueable, Queueable);

        public:
            ResolveQueueable (
                Isolate *pIsolate, resolver_handle_t const &rhResolver, VE::Value iResult
            ) : Queueable (pIsolate), m_hResolver (pIsolate, rhResolver), m_iResult (iResult) {
            }
        private:
            ~ResolveQueueable () {
            }

        private:
            void execute () override {
                std::cerr << "Got Result: " << std::endl << m_iResult.output ();
            }

        //  State
        private:
            resolver_handle_t m_hResolver;
            VE::Value const   m_iResult;
        };

    //  class RejectQueueable
    public:
        class RejectQueueable : public Queueable {
            DECLARE_CONCRETE_RTTLITE (RejectQueueable, Queueable);

        public:
            RejectQueueable (
                Isolate *pIsolate, resolver_handle_t const &rhResolver, Vca::IError *pInterface, VString const &rMessage
            ) : Queueable (pIsolate), m_hResolver (pIsolate, rhResolver), m_pInterface (pInterface), m_iMessage (rMessage) {
            }
        private:
            ~RejectQueueable () {
            }

        private:
            void execute () override {
                std::cerr << "+++ Got Error: " << m_iMessage << std::endl;
            }

        //  State
        private:
            resolver_handle_t      m_hResolver;
            Vca::IError::Reference m_pInterface;
            VString const          m_iMessage;
        };
        
    public:
        ResultSink (
            Isolate *pIsolate,
            Local<Promise::Resolver> hResolver,
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
            Queueable::Reference const pQueueable (
                new ResolveQueueable (m_pIsolate, m_hResolver, iResult)
            );
            pQueueable->enqueue ();
        }
        void onError (Vca::IError *pInterface, VString const &rMessage) {
            Queueable::Reference const pQueueable (
                new RejectQueueable (m_pIsolate, m_hResolver, pInterface, rMessage)
            );
            pQueueable->enqueue ();
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

        Local<String> str = args[0]->ToString ();

        String::Utf8Value pExpression(str);

        MaybeLocal<Promise::Resolver> mResolver = Promise::Resolver::New (pIsolate->GetCurrentContext ());
        Local<Promise::Resolver> hResolver = mResolver.ToLocalChecked ();

        VE::Gofer::Reference const pGofer (new VE::Gofer (DefaultEvaluator (), *pExpression));
        ResultSink::Reference const pResultSink (new ResultSink (pIsolate, hResolver, pGofer));

//        Local<Object> obj = Object::New(pIsolate);
//        obj->Set(String::NewFromUtf8(pIsolate, "msg"), str);
//        args.GetReturnValue().Set(obj);
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
