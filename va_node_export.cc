/*****  VA_Node_Export Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "va_node_export.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_handle_scope.h"

#include "Vxa_VAny.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"


/******************************
 ******************************
 *****                    *****
 *****  VA::Node::Export  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Export::Export (
    Isolate *pIsolate, local_value_t hValue
) : BaseClass (pIsolate), m_hValue (*pIsolate, hValue) {
    std::cerr << "VA::Node::Export::Export: " << this << std::endl;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Export::~Export () {
    std::cerr << "VA::Node::Export::~Export: " << this << std::endl;
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Export::decommision () {
    std::cerr << "VA::Node::Export::decommision: " << this << std::endl;
    return isolate ()->Detach (this) && BaseClass::decommision ();
}

/****************************
 ****************************
 *****  Return Helpers  *****
 ****************************
 ****************************/

void VA::Node::Export::ReturnUnwrapped (Vxa::VResultBuilder &rRB, maybe_value_t hValue) const {
    Reference pValue;
    if (Attach (pValue, hValue))
        rRB = pValue;
    else
        rRB = false;
}

void VA::Node::Export::ReturnUnwrapped (Vxa::VResultBuilder &rRB, local_value_t hValue) const {
    Reference pValue;
    if (Attach (pValue, hValue))
        rRB = pValue;
    else
        rRB = false;
}


/*********************
 *********************
 *****  Methods  *****
 *********************
 *********************/

/*************************
 *----  loopbackAny  ----*
 *************************/

namespace {
    class LoopbackSink : public Vxa::VAny::Client {
    public:
        LoopbackSink (Vxa::VResultBuilder &rRB) : m_rRB (rRB) {
        }
        ~LoopbackSink () {
        }
    private:
        template <typename value_t> void onImpl (value_t iValue) {
            m_rRB = iValue;
        }
    public:
        virtual void on (int iValue) override {
            onImpl (iValue);
        }
        virtual void on (double iValue) override {
            onImpl (iValue);
        }
        virtual void on (VString const &iValue) override {
            onImpl (iValue);
        }
    private:
        Vxa::VResultBuilder &m_rRB;
    };
}

void VA::Node::Export::loopbackAny (
    Vxa::VResultBuilder &rRB, Vxa::VAny::value_t rAny
) {
    LoopbackSink iAnySink (rRB);
    rAny.supply (iAnySink);
}

/*************************
 *----  interceptor  ----*
 *************************/

namespace {
    class InterceptorSink : public Vxa::VAny::Client {
    public:
        InterceptorSink (VString &rMessage) : m_rMessage (rMessage) {
        }
        ~InterceptorSink () {
        }
    private:
        template <typename value_t> void onImpl (value_t iValue) {
            m_rMessage << iValue;
        }
    public:
        virtual void on (int iValue) override {
            onImpl (iValue);
        }
        virtual void on (double iValue) override {
            onImpl (iValue);
        }
        virtual void on (VString const &iValue) override {
            onImpl (iValue);
        }
    private:
        VString &m_rMessage;
    };
}

void VA::Node::Export::interceptor (
    Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t rPack
) {
    VString iMessage;
    iMessage
	<< "Called as '"
        << (rRB.invokedIntensionally () ? ":" : "")
	<< rRB.selectorName ()
	<< "' with parameter count of "
	<< rRB.parameterCount ()
	<< " and task size of "
	<< rRB.taskCardinality ();

    InterceptorSink iAnySink (iMessage);
    Vxa::cardinality_t const cParameters = rPack.parameterCount ();
    for (Vxa::cardinality_t xParameter = 0; xParameter < cParameters; xParameter++) {
        iMessage.printf (
            "\n %2u: %10s: ", xParameter, rRB.selectorComponent (xParameter).content ()
        );
        rPack.parameterValue (xParameter).supply (iAnySink);
    }
    iMessage << "\n";
    rRB = iMessage;
}

void VA::Node::Export::adder (Vxa::VResultBuilder &rRB, Vxa::VPack<double>::value_t pack_o_ds) {
    double result = 0.0;
    for (unsigned int xD = 0; xD < pack_o_ds.parameterCount (); xD++)
        result += pack_o_ds[xD];
    rRB = result;
}


/***************************
 *****  JS Operations  *****
 ***************************/

/**********************
 *----  Callback  ----*
 **********************/

namespace {
    using namespace VA::Node;

    class JSCallbackSink : public Vxa::VAny::Client {
    public:
        JSCallbackSink (
            maybe_value_t &rResult, Isolate *pIsolate
        ) : m_rResult (rResult), m_pIsolate (pIsolate) {
        }
        ~JSCallbackSink () {
        }
    private:
        template <typename value_t> void onImpl (value_t iValue) {
        }
    public:
        virtual void on (int iValue) override {
            onImpl (iValue);
        }
        virtual void on (double iValue) override {
            onImpl (iValue);
        }
        virtual void on (VString const &iValue) override {
            onImpl (iValue);
        }
    private:
        maybe_value_t      m_rResult;
        Isolate::Reference m_pIsolate;
    };
}

void VA::Node::Export::JSCallback (Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t) {
    HandleScope iHS (this);

    rRB = false;
    local_object_t hThis;
    if (GetLocal (hThis)) {
        maybe_value_t const hPropertyValue = hThis->Get (
            context (), NewString (rRB.selectorComponent (0))
        );
        ReturnUnwrapped (rRB, hPropertyValue);
    }
}


/*************************
 *----  Conversions  ----*
 *************************/

void VA::Node::Export::JSToString (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    VString iResult;
    UnwrapString (iResult, value(), false);
    rRB = iResult;
}

void VA::Node::Export::JSToDetail (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    VString iResult;
    UnwrapString (iResult, value(), true);
    rRB = iResult;
}

/****************************
 *----  Return Helpers  ----*
 ****************************/

void VA::Node::Export::JSUnwrap (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    ReturnUnwrapped (rRB, value ());
}


/****************************
 *----  Property Query  ----*
 ****************************/

void VA::Node::Export::JSHasProperty (Vxa::VResultBuilder &rRB, VString const &rPropertyName) {
    HandleScope iHS (this);
    local_object_t hThis;
    rRB = GetLocal (hThis) && hThis->Has (
        context (), NewString (rPropertyName)
    ).FromMaybe (false);
}


/************************
 *----  Type Query  ----*
 ************************/

void VA::Node::Export::JSIsUndefined (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsUndefined ();
}
void VA::Node::Export::JSIsNull (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsNull ();
}
void VA::Node::Export::JSIsNullOrUndefined (Vxa::VResultBuilder &rRB) {
#if 0
    HandleScope iHS (this);
    rRB = value()->IsNullOrUndefined ();
#else
    rRB = false;
#endif
}
void VA::Node::Export::JSIsTrue (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsTrue ();
}
void VA::Node::Export::JSIsFalse (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsFalse ();
}
void VA::Node::Export::JSIsName (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsName ();
}
void VA::Node::Export::JSIsString (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsString ();
}
void VA::Node::Export::JSIsSymbol (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsSymbol ();
}
void VA::Node::Export::JSIsFunction (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsFunction ();
}
void VA::Node::Export::JSIsArray (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsArray ();
}
void VA::Node::Export::JSIsObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsObject ();
}
void VA::Node::Export::JSIsBoolean (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsBoolean ();
}
void VA::Node::Export::JSIsNumber (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsNumber ();
}
void VA::Node::Export::JSIsExternal (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsExternal ();
}
void VA::Node::Export::JSIsInt32 (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsInt32 ();
}
void VA::Node::Export::JSIsUint32 (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsUint32 ();
}
void VA::Node::Export::JSIsDate (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsDate ();
}
void VA::Node::Export::JSIsArgumentsObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsArgumentsObject ();
}
void VA::Node::Export::JSIsBooleanObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsBooleanObject ();
}
void VA::Node::Export::JSIsNumberObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsNumberObject ();
}
void VA::Node::Export::JSIsStringObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsStringObject ();
}
void VA::Node::Export::JSIsSymbolObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsSymbolObject ();
}
void VA::Node::Export::JSIsNativeError (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsNativeError ();
}
void VA::Node::Export::JSIsRegExp (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsRegExp ();
}
void VA::Node::Export::JSIsAsyncFunction (Vxa::VResultBuilder &rRB) {
#if 0
    HandleScope iHS (this);
    rRB = value()->IsAsyncFunction ();
#else
    rRB = false;
#endif
}
void VA::Node::Export::JSIsGeneratorFunction (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsGeneratorFunction ();
}
void VA::Node::Export::JSIsGeneratorObject (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsGeneratorObject ();
}
void VA::Node::Export::JSIsPromise (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsPromise ();
}
void VA::Node::Export::JSIsMap (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsMap ();
}
void VA::Node::Export::JSIsSet (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsSet ();
}
void VA::Node::Export::JSIsMapIterator (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsMapIterator ();
}
void VA::Node::Export::JSIsSetIterator (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsSetIterator ();
}
void VA::Node::Export::JSIsWeakMap (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsWeakMap ();
}
void VA::Node::Export::JSIsWeakSet (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsWeakSet ();
}
void VA::Node::Export::JSIsArrayBuffer (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsArrayBuffer ();
}
void VA::Node::Export::JSIsArrayBufferView (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsArrayBufferView ();
}
void VA::Node::Export::JSIsTypedArray (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsTypedArray ();
}
void VA::Node::Export::JSIsUint8Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsUint8Array ();
}
void VA::Node::Export::JSIsUint8ClampedArray (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsUint8ClampedArray ();
}
void VA::Node::Export::JSIsInt8Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsInt8Array ();
}
void VA::Node::Export::JSIsUint16Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsUint16Array ();
}
void VA::Node::Export::JSIsInt16Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsInt16Array ();
}
void VA::Node::Export::JSIsUint32Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsUint32Array ();
}
void VA::Node::Export::JSIsInt32Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsInt32Array ();
}
void VA::Node::Export::JSIsFloat32Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsFloat32Array ();
}
void VA::Node::Export::JSIsFloat64Array (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsFloat64Array ();
}
void VA::Node::Export::JSIsDataView (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsDataView ();
}
void VA::Node::Export::JSIsSharedArrayBuffer (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsSharedArrayBuffer ();
}
void VA::Node::Export::JSIsProxy (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    rRB = value()->IsProxy ();
}
void VA::Node::Export::JSIsWebAssemblyCompiledModule (Vxa::VResultBuilder &rRB) {
#if 0
    HandleScope iHS (this);
    rRB = value()->IsWebAssemblyCompiledModule ();
#else
    rRB = false;
#endif
}


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Export::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Vxa::Object::ClassBuilder (pClass) {
    defineMethod ("loopbackAny:", &Export::loopbackAny);

    defineMethod ("add"		, &Export::adder);
    defineMethod ("add:"	, &Export::adder);
    defineMethod ("add:a:"	, &Export::adder);
    defineMethod ("add:a:a:"	, &Export::adder);
    defineMethod ("add:a:a:a:"	, &Export::adder);
    defineMethod ("add:a:a:a:a:", &Export::adder);

    defineMethod (".toString"                   , &Export::JSToString);
    defineMethod (".toDetail"                   , &Export::JSToDetail);

    defineMethod (".unwrap"                     , &Export::JSUnwrap);

    defineMethod (".hasProperty:"               , &Export::JSHasProperty);

    defineMethod (".isUndefined"                , &Export::JSIsUndefined);
    defineMethod (".isNull"                     , &Export::JSIsNull);
    defineMethod (".isNullOrUndefined"          , &Export::JSIsNullOrUndefined);
    defineMethod (".isTrue"                     , &Export::JSIsTrue);
    defineMethod (".isFalse"                    , &Export::JSIsFalse);
    defineMethod (".isName"                     , &Export::JSIsName);
    defineMethod (".isString"                   , &Export::JSIsString);
    defineMethod (".isSymbol"                   , &Export::JSIsSymbol);
    defineMethod (".isFunction"                 , &Export::JSIsFunction);
    defineMethod (".isArray"                    , &Export::JSIsArray);
    defineMethod (".isObject"                   , &Export::JSIsObject);
    defineMethod (".isBoolean"                  , &Export::JSIsBoolean);
    defineMethod (".isNumber"                   , &Export::JSIsNumber);
    defineMethod (".isExternal"                 , &Export::JSIsExternal);
    defineMethod (".isInt32"                    , &Export::JSIsInt32);
    defineMethod (".isUint32"                   , &Export::JSIsUint32);
    defineMethod (".isDate"                     , &Export::JSIsDate);
    defineMethod (".isArgumentsObject"          , &Export::JSIsArgumentsObject);
    defineMethod (".isBooleanObject"            , &Export::JSIsBooleanObject);
    defineMethod (".isNumberObject"             , &Export::JSIsNumberObject);
    defineMethod (".isStringObject"             , &Export::JSIsStringObject);
    defineMethod (".isSymbolObject"             , &Export::JSIsSymbolObject);
    defineMethod (".isNativeError"              , &Export::JSIsNativeError);
    defineMethod (".isRegExp"                   , &Export::JSIsRegExp);
    defineMethod (".isAsyncFunction"            , &Export::JSIsAsyncFunction);
    defineMethod (".isGeneratorFunction"        , &Export::JSIsGeneratorFunction);
    defineMethod (".isGeneratorObject"          , &Export::JSIsGeneratorObject);
    defineMethod (".isPromise"                  , &Export::JSIsPromise);
    defineMethod (".isMap"                      , &Export::JSIsMap);
    defineMethod (".isSet"                      , &Export::JSIsSet);
    defineMethod (".isMapIterator"              , &Export::JSIsMapIterator);
    defineMethod (".isSetIterator"              , &Export::JSIsSetIterator);
    defineMethod (".isWeakMap"                  , &Export::JSIsWeakMap);
    defineMethod (".isWeakSet"                  , &Export::JSIsWeakSet);
    defineMethod (".isArrayBuffer"              , &Export::JSIsArrayBuffer);
    defineMethod (".isArrayBufferView"          , &Export::JSIsArrayBufferView);
    defineMethod (".isTypedArray"               , &Export::JSIsTypedArray);
    defineMethod (".isUint8Array"               , &Export::JSIsUint8Array);
    defineMethod (".isUint8ClampedArray"        , &Export::JSIsUint8ClampedArray);
    defineMethod (".isInt8Array"                , &Export::JSIsInt8Array);
    defineMethod (".isUint16Array"              , &Export::JSIsUint16Array);
    defineMethod (".isInt16Array"               , &Export::JSIsInt16Array);
    defineMethod (".isUint32Array"              , &Export::JSIsUint32Array);
    defineMethod (".isInt32Array"               , &Export::JSIsInt32Array);
    defineMethod (".isFloat32Array"             , &Export::JSIsFloat32Array);
    defineMethod (".isFloat64Array"             , &Export::JSIsFloat64Array);
    defineMethod (".isDataView"                 , &Export::JSIsDataView);
    defineMethod (".isSharedArrayBuffer"        , &Export::JSIsSharedArrayBuffer);
    defineMethod (".isProxy"                    , &Export::JSIsProxy);
    defineMethod (".isWebAssemblyCompiledModule", &Export::JSIsWebAssemblyCompiledModule);
    
#if 1
    defineDefault (&Export::JSCallback);
#else
    defineDefault (&Export::interceptor);
#endif
}

namespace {
    Vxa::VCollectable<VA::Node::Export> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(VA::Node::Export);
