/*****  VA_Node_Isolate Implementation  *****/

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

#include "va_node_isolate.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_export.h"
#include "va_node_handle_scope.h"


/*******************************
 *******************************
 *****                     *****
 *****  VA::Node::Isolate  *****
 *****                     *****
 *******************************
 *******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Isolate::Isolate (
    handle_t hIsolate
) : m_hIsolate (hIsolate), m_hValueCache (hIsolate, object_cache_t::New (hIsolate)) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Isolate::~Isolate () {
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

bool VA::Node::Isolate::GetInstance (Reference &rpInstance, v8::Isolate *pIsolate) {
    return Process::Attach (rpInstance, pIsolate);
}

/****************************
 ****************************
 *****  Decommisioning  *****
 ****************************
 ****************************/

/********************
 *----  MySelf  ----*
 ********************/

bool VA::Node::Isolate::onDeleteThis () {
//  return Process::Detach (this);
    return false;
}

/********************
 *----  Others  ----*
 ********************/

bool VA::Node::Isolate::okToDecommision (Isolated *pIsolated) const {
    return Process::OkToDecommision (pIsolated);
}


/****************************
 ****************************
 *****  Access Helpers  *****
 ****************************
 ****************************/

bool VA::Node::Isolate::UnwrapString (
    VString &rString, maybe_value_t hValue, bool bDetail
) const {
    local_value_t hLocalValue;
    return GetLocalFor (hLocalValue, hValue) && UnwrapString (rString, hLocalValue);
}

bool VA::Node::Isolate::UnwrapString (
    VString &rString, local_value_t hValue, bool bDetail
) const {
    return UnwrapString (
        rString, bDetail ? hValue->ToDetailString (
            context ()
        ) : hValue->ToString (
            context ()
        )
    );
}

bool VA::Node::Isolate::UnwrapString (VString &rString, maybe_string_t hString) const {
    local_string_t hLocalString;
    return GetLocalFor (hLocalString, hString) && UnwrapString (rString, hLocalString);
}

bool VA::Node::Isolate::UnwrapString (VString &rString, local_string_t hString) const {
    string_t::Utf8Value pString (hString);
    rString.setTo (*pString);
    return true;
}

/******************************
 ******************************
 *****  Creation Helpers  *****
 ******************************
 ******************************/

VA::Node::local_resolver_t VA::Node::Isolate::NewResolver () const {
    return resolver_t::New (context ()).ToLocalChecked ();
}

VA::Node::local_string_t VA::Node::Isolate::NewString (char const *pString) const {
    return string_t::NewFromUtf8 (m_hIsolate, pString);
}

/*******************************
 *******************************
 *****  Exception Helpers  *****
 *******************************
 *******************************/

void VA::Node::Isolate::ThrowTypeError (char const *pMessage) const {
    m_hIsolate->ThrowException (v8::Exception::TypeError (NewString (pMessage)));
}

/***************************
 ***************************
 *****  Export Access  *****
 ***************************
 ***************************/

bool VA::Node::Isolate::GetExport (Vxa::export_return_t &rExport, local_value_t hValue){
    ExportReference pValue;
    if (!Attach (pValue, hValue))
        return false;

    rExport.setTo (Vxa::Export (pValue));
    return true;
}


/******************************
 ******************************
 *****  Model Management  *****
 ******************************
 ******************************/

bool VA::Node::Isolate::Attach (
    ExportReference &rpModelObject, maybe_value_t hValue
) {
    local_value_t hLocalValue;
    return GetLocalFor (hLocalValue, hValue) && Attach (rpModelObject, hLocalValue);
}

bool VA::Node::Isolate::Attach (
    ExportReference &rpModelObject, local_value_t hValue
) {
    HandleScope iHS (this);

    object_cache_handle_t hValueCache (LocalFor (m_hValueCache));
    local_value_t hCachedValue (hValueCache->Get (hValue));

    if (!hCachedValue.IsEmpty () && hCachedValue->IsExternal ()) {
        rpModelObject.setTo (reinterpret_cast<Export*>(hCachedValue.As<v8::External>()->Value()));
        std::cerr
            << "VA::Node::Isolate::Attach: " << this << ", found: " << rpModelObject.referent () << std::endl;
    } else {
        rpModelObject.setTo (new Export (this, hValue));
        hValueCache->Set (hValue, v8::External::New (m_hIsolate, rpModelObject.referent ()));
        std::cerr
            << "VA::Node::Isolate["
            << this
            << "]::Attach: "
            << rpModelObject.referent ()
            << (hValueCache->Has (hValue) ? " attached" : " uncached")
            << std::endl;
    }
    return rpModelObject.isntNil ();
}

bool VA::Node::Isolate::Detach (Export *pModelObject) {
    HandleScope iHS (this);
    object_cache_handle_t const hCache = LocalFor (m_hValueCache);
    local_value_t         const hModelValue = pModelObject->value ();
    local_value_t               hCacheValue;

/*****************
 ****  Consider the model object gone if:
 *
 *  1) it was just deleted from the cache (hCache->Delete (hModelValue) returns true)  . . . . OR:
 *  2) it isn't in the cache ((hCacheValue= hCache->Get (hModelValue)).IsEmpty ()) . . . . . . OR:
 *  3) the cached value isn't a v8::External (hCacheValue->IsExternal () returns false)  . . . OR:
 *  4) the cached external isn't the model object (reinterpret_cast<...> != pModelObject)
 *
 ****************/
    return [&](bool bGone) {
        std::cerr
            << "VA::Node::Isolate["
            << this
            << "]::Detach: "
            << pModelObject
            << (bGone ? " detached" : " retained")
            << std::endl;
        return bGone;
    } (
        hCache->Delete (hModelValue)                         ||
        (hCacheValue = hCache->Get (hModelValue)).IsEmpty () ||
        !hCacheValue->IsExternal ()                          ||
        hCacheValue.As<v8::External>()->Value() != pModelObject
    );
}


/***************************
 ***************************
 *****  Result Return  *****
 ***************************
 ***************************/

bool VA::Node::Isolate::MaybeSetResultTo (
    Vxa::VResultBuilder &rRB, local_value_t hValue
) {
    return !hValue.IsEmpty () && (
        MaybeSetResultToInt32  (rRB, hValue) ||
        MaybeSetResultToDouble (rRB, hValue) ||
        MaybeSetResultToString (rRB, hValue) ||
        MaybeSetResultToObject (rRB, hValue)
    );
}

bool VA::Node::Isolate::MaybeSetResultToInt32 (
    Vxa::VResultBuilder &rRB, local_value_t hValue
) {
    int32_t iResult;
    if (hValue->IsInt32 () && hValue->Int32Value (context ()).To (&iResult)) {
        rRB = iResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::MaybeSetResultToDouble (
    Vxa::VResultBuilder &rRB, local_value_t hValue
) {
    double iResult;
    if (hValue->IsNumber () && hValue->NumberValue (context ()).To (&iResult)) {
        rRB = iResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::MaybeSetResultToString (
    Vxa::VResultBuilder &rRB, local_value_t hValue
) {
    VString iResult;
    if (hValue->IsString () && UnwrapString (iResult, hValue)) {
        rRB = iResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::MaybeSetResultToObject (
    Vxa::VResultBuilder &rRB, local_value_t hValue
) {
    ExportReference pResult;
    if (Attach (pResult, hValue)) {
        rRB = pResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::SetResultTo (
    Vxa::VResultBuilder &rRB, maybe_value_t hMaybe
) {
    local_value_t hValue;
    return (
        GetLocalFor (hValue, hMaybe) && MaybeSetResultTo (rRB, hValue)
    ) || SetResultToUndefined (rRB);
}

bool VA::Node::Isolate::SetResultTo (
    Vxa::VResultBuilder &rRB, local_value_t hValue
) {
    return MaybeSetResultTo (rRB, hValue) || SetResultToUndefined (rRB);
}

bool VA::Node::Isolate::SetResultToUndefined (Vxa::VResultBuilder &rRB) {
//    V8<v8::Primitive>::local v8::Undefined (m_hIsolate);
    ExportReference pResult;
    if (Attach (pResult, local_value_t (v8::Undefined (m_hIsolate)))) {
        rRB = pResult;
    } else {
        rRB = false;
    }
    return true;
}
