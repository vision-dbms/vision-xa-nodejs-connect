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

#include "Vxa_VCollectable.h"


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
//    std::cerr << "VA::Node::Export::Export: " << this << std::endl;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Export::~Export () {
//    std::cerr << "VA::Node::Export::~Export: " << this << std::endl;
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Export::decommission () {
//    std::cerr << "VA::Node::Export::decommission: " << this << std::endl;
    return isolate ()->Detach (this) && BaseClass::decommission ();
}


/************************
 ************************
 *****  JS Methods  *****
 ************************
 ************************/

/***************************
 *****  JS Operations  *****
 ***************************/

/**********************
 *----  Callback  ----*
 **********************/

void VA::Node::Export::JSCallback (vxa_result_t &rResult, vxa_pack_t rPack) {
    HandleScope iHS (this);

    local_object_t hObject;
    if (!GetLocal (hObject))
        SetResultToUndefined (rResult);
    else {
    /*
        std::cerr
            << "VA::Node::Export ["
            << std::setw(14) << this
            << ","
            << std::setw(6) << callCount () + 1
            << "] Calling   "
            << rResult.selectorComponent (0)
            << std::endl;
    */
        local_string_t hSelector; local_value_t hApplicable; (
            NewString (
                hSelector, rResult.selectorComponent (0)
            ) && GetLocalFor (
                hApplicable, hObject->Get (context (), hSelector)
            ) && rResult.invokedIntensionally () ? (
                SetResultToValue (rResult, hApplicable)
            ) : (
                MaybeSetResultToApply (rResult, value (), hApplicable, rPack) ||
                SetResultToValue (rResult, hApplicable)
            )
        ) || SetResultToUndefined (rResult);
    }
}


/*********************
 *----  Context  ----*
 *********************/

void VA::Node::Export::JSGlobal (vxa_result_t &rResult) {
    HandleScope iHS (this);
/*
    std::cerr
        << "VA::Node::Export ["
        << std::setw(14) << this
        << ","
        << std::setw(6) << callCount ()
        << "] Getting   .global"
        << std::endl;
*/
    SetResultToGlobal (rResult);
}

/************************
 *----  Comparison  ----*
 ************************/

void VA::Node::Export::JSStrictEquals (vxa_result_t &rResult, ThisClass *pThat) {
    HandleScope iHS (this);
    rResult = value ()->StrictEquals (pThat->value ());
}

/************************
 *----  Conversion  ----*
 ************************/

void VA::Node::Export::JSToString (vxa_result_t &rResult) {
    HandleScope iHS (this);
    VString iResult;
    UnwrapString (iResult, value(), false);
    rResult = iResult;
}


/*****************
 *----  New  ----*
 *****************/

void VA::Node::Export::JSNew (vxa_result_t &rResult, vxa_pack_t rPack) {
    HandleScope iHS (this);

    SetResultToNewInstance (rResult, value (), rPack);
}


/******************************
 *----  Structure Access  ----*
 ******************************/

void VA::Node::Export::JSAt (vxa_result_t &rResult, vxa_any_t rKey) {
    HandleScope iHS (this);

    local_object_t hObject;
    local_value_t hKey, hValue;
    Isolate::ArgSink iKeySink (hKey, isolate ());
    rKey.supply (iKeySink);
    (
        GetLocal (hObject) && GetLocalFor (
            hValue, hObject->Get (context (), hKey)
        ) && SetResultToValue (rResult, hValue)
    ) || SetResultToUndefined (rResult);
}


/************************
 *----  Type Query  ----*
 ************************/

void VA::Node::Export::JSIsUndefined (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsUndefined ();
}
void VA::Node::Export::JSIsNull (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsNull ();
}
void VA::Node::Export::JSIsNullOrUndefined (vxa_result_t &rResult) {
    local_value_t hValue = value ();
    rResult = hValue->IsNull () || hValue->IsUndefined ();
}
void VA::Node::Export::JSIsTrue (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsTrue ();
}
void VA::Node::Export::JSIsFalse (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsFalse ();
}
void VA::Node::Export::JSIsName (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsName ();
}
void VA::Node::Export::JSIsString (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsString ();
}
void VA::Node::Export::JSIsSymbol (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsSymbol ();
}
void VA::Node::Export::JSIsFunction (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsFunction ();
}
void VA::Node::Export::JSIsArray (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsArray ();
}
void VA::Node::Export::JSIsObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsObject ();
}
void VA::Node::Export::JSIsBoolean (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsBoolean ();
}
void VA::Node::Export::JSIsNumber (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsNumber ();
}
void VA::Node::Export::JSIsExternal (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsExternal ();
}
void VA::Node::Export::JSIsInt32 (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsInt32 ();
}
void VA::Node::Export::JSIsUint32 (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsUint32 ();
}
void VA::Node::Export::JSIsDate (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsDate ();
}
void VA::Node::Export::JSIsArgumentsObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsArgumentsObject ();
}
void VA::Node::Export::JSIsBooleanObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsBooleanObject ();
}
void VA::Node::Export::JSIsNumberObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsNumberObject ();
}
void VA::Node::Export::JSIsStringObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsStringObject ();
}
void VA::Node::Export::JSIsSymbolObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsSymbolObject ();
}
void VA::Node::Export::JSIsNativeError (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsNativeError ();
}
void VA::Node::Export::JSIsRegExp (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsRegExp ();
}
void VA::Node::Export::JSIsGeneratorFunction (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsGeneratorFunction ();
}
void VA::Node::Export::JSIsGeneratorObject (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsGeneratorObject ();
}
void VA::Node::Export::JSIsPromise (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsPromise ();
}
void VA::Node::Export::JSIsMap (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsMap ();
}
void VA::Node::Export::JSIsSet (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsSet ();
}
void VA::Node::Export::JSIsMapIterator (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsMapIterator ();
}
void VA::Node::Export::JSIsSetIterator (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsSetIterator ();
}
void VA::Node::Export::JSIsWeakMap (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsWeakMap ();
}
void VA::Node::Export::JSIsWeakSet (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsWeakSet ();
}
void VA::Node::Export::JSIsArrayBuffer (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsArrayBuffer ();
}
void VA::Node::Export::JSIsArrayBufferView (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsArrayBufferView ();
}
void VA::Node::Export::JSIsTypedArray (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsTypedArray ();
}
void VA::Node::Export::JSIsUint8Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsUint8Array ();
}
void VA::Node::Export::JSIsUint8ClampedArray (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsUint8ClampedArray ();
}
void VA::Node::Export::JSIsInt8Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsInt8Array ();
}
void VA::Node::Export::JSIsUint16Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsUint16Array ();
}
void VA::Node::Export::JSIsInt16Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsInt16Array ();
}
void VA::Node::Export::JSIsUint32Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsUint32Array ();
}
void VA::Node::Export::JSIsInt32Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsInt32Array ();
}
void VA::Node::Export::JSIsFloat32Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsFloat32Array ();
}
void VA::Node::Export::JSIsFloat64Array (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsFloat64Array ();
}
void VA::Node::Export::JSIsDataView (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsDataView ();
}
void VA::Node::Export::JSIsSharedArrayBuffer (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsSharedArrayBuffer ();
}
void VA::Node::Export::JSIsProxy (vxa_result_t &rResult) {
    HandleScope iHS (this);
    rResult = value()->IsProxy ();
}


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Export::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : BaseClass::ClassBuilder (pClass) {
    defineMethod ("at:"                         , &Export::JSAt);
    defineMethod (".at:"                        , &Export::JSAt);

    defineMethod (".global"                     , &Export::JSGlobal);

    defineMethod (".strictEquals:"              , &Export::JSStrictEquals);

    defineMethod (".toString"                   , &Export::JSToString);

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

    defineMethod (".new"                        , &Export::JSNew);
    defineMethod (".new:"                       , &Export::JSNew);
    defineMethod (".new:and:"                   , &Export::JSNew);
    defineMethod (".new:and:and:"               , &Export::JSNew);
    defineMethod (".new:and:and:and:"           , &Export::JSNew);
    defineMethod (".new:and:and:and:and:"       , &Export::JSNew);
    defineMethod (".new:and:and:and:and:and:"   , &Export::JSNew);

    defineDefault (&Export::JSCallback);
}

namespace {
    Vxa::VCollectable<VA::Node::Export> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(VA::Node::Export);
