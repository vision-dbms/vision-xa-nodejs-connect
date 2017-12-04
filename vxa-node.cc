#include <node.h>
#include <v8.h>
#include <uv.h>

#include "Vk.h"

#include "Vision_Evaluation_Gofer.h"

namespace {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;

    void Evaluate(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        Local<Object> obj = Object::New(isolate);
        Local<String> str = args[0]->ToString ();
        obj->Set(String::NewFromUtf8(isolate, "msg"), str);

        String::Utf8Value pExpression(str);
        Vision::Evaluation::Gofer::Evaluate (*pExpression);

        args.GetReturnValue().Set(obj);
    }

    void Init(Local<Object> exports, Local<Object> module) {
        NODE_SET_METHOD(module, "exports", Evaluate);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
