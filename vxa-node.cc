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
    using v8::Object;
    using v8::String;
    using v8::Value;

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

    class ResultSink : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ResultSink, Vca::VRolePlayer);

    public:
        ResultSink (VE::evaluation_result_gofer_t *pResultGofer) {
            retain (); {
                pResultGofer->supplyMembers (this, &ThisClass::onData, &ThisClass::onError);
            } untain ();
        }
    private:
        ~ResultSink () {
        }

    private:
        void onData (VE::Value iResult) {
            std::cerr << "Got Result: " << std::endl << iResult.output ();
        }
        void onError (Vca::IError *pInterface, VString const &rMessage) {
            std::cerr << "+++ Got Error: " << rMessage << std::endl;
        }
    };

    void Evaluate(const FunctionCallbackInfo<Value>& args) {
        Vca::VCohortClaim cohortClaim;

        Isolate* isolate = args.GetIsolate();

        Local<Object> obj = Object::New(isolate);
        Local<String> str = args[0]->ToString ();
        obj->Set(String::NewFromUtf8(isolate, "msg"), str);

        String::Utf8Value pExpression(str);
        VE::Gofer::Reference const pGofer (new VE::Gofer (DefaultEvaluator (), *pExpression));

        ResultSink::Reference const pResultSink (new ResultSink (pGofer));

        args.GetReturnValue().Set(obj);
    }

    void Init(Local<Object> exports, Local<Object> module) {
        NODE_SET_METHOD(module, "exports", Evaluate);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
