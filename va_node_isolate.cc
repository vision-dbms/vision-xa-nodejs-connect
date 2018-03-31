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
    isolate_handle_t hIsolate
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

namespace {
    template <typename T> bool GetUnwrappedFromMaybe (
        T &rUnwrapped, v8::Maybe<T> const &rMaybe
    ) {
        if (rMaybe.IsJust ()) {
            rUnwrapped = rMaybe.FromJust ();
            return true;
        }
        return false;
    }
}

bool VA::Node::Isolate::GetUnwrapped (
    bool &rUnwrapped, local_value_t hValue
) const {
    return GetUnwrappedFromMaybe (
        rUnwrapped, hValue->BooleanValue (context ())
    );
}

bool VA::Node::Isolate::GetUnwrapped (
    double &rUnwrapped, local_value_t hValue
) const {
    return GetUnwrappedFromMaybe (
        rUnwrapped, hValue->NumberValue (context ())
    );
}

bool VA::Node::Isolate::GetUnwrapped (
    int32_t &rUnwrapped, local_value_t hValue
) const {
    return GetUnwrappedFromMaybe (
        rUnwrapped, hValue->Int32Value (context ())
    );
}

bool VA::Node::Isolate::UnwrapString (
    VString &rString, local_value_t hValue, bool bDetailed
) const {
    local_string_t hString;
    return GetLocalFrom (hString, hValue, bDetailed) && UnwrapString (rString, hString);
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
        // std::cerr
        //     << "VA::Node::Isolate::Attach: " << this << ", found: " << rpModelObject.referent () << std::endl;
    } else {
        rpModelObject.setTo (new Export (this, hValue));
        hValueCache->Set (hValue, v8::External::New (m_hIsolate, rpModelObject.referent ()));
        // std::cerr
        //     << "VA::Node::Isolate["
        //     << this
        //     << "]::Attach: "
        //     << rpModelObject.referent ()
        //     << (hValueCache->Has (hValue) ? " attached" : " uncached")
        //     << std::endl;
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
        // std::cerr
        //     << "VA::Node::Isolate["
        //     << this
        //     << "]::Detach: "
        //     << pModelObject
        //     << (bGone ? " detached" : " retained")
        //     << std::endl;
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

/**************************
 *****  Call Helpers  *****
 **************************/

class VA::Node::Isolate::Args {
//  class ArgSink
public:
    class ArgSink : public Vxa::VAny::Client {
    public:
        ArgSink (
            local_value_t &rResult, Isolate *pIsolate
        ) : m_rResult (rResult), m_pIsolate (pIsolate) {
        }
        ~ArgSink () {
        }
    public:
        virtual bool on (int iValue) override {
            m_rResult = m_pIsolate->NewNumber (iValue);
            return true;
        }
        virtual bool on (double iValue) override {
            m_rResult = m_pIsolate->NewNumber (iValue);
            return true;
        }
        virtual bool on (VString const &iValue) override {
            m_rResult = m_pIsolate->NewString (iValue);
            return true;
        }
        virtual bool on (VCollectableObject *pObject) override {
            Export *pExport = dynamic_cast<Export*>(pObject);
            if (pExport)
                m_rResult = pExport->value ();
            else
                m_rResult = m_pIsolate->LocalUndefined ();
            return true;
        }
    private:
        local_value_t& m_rResult;
        Isolate* const m_pIsolate;
    };

//  Construction
public:
    Args (Isolate *pIsolate, vxa_pack_t rPack);

//  Destruction
public:
    ~Args ();

//  Access
public:
    int argc () const {
        return m_aArgs.elementCount ();
    }
    local_value_t *argv () {
        return m_aArgs.elementArray ();
    }

//  State
private:
    VkDynamicArrayOf<local_value_t> m_aArgs;
};

/*----------------*/
VA::Node::Isolate::Args::Args (
    Isolate *pIsolate, vxa_pack_t rPack
) : m_aArgs (rPack.parameterCount ()) {
    int const cArgs = argc ();
    for (int xArg = 0; xArg < cArgs; xArg++) {
        local_value_t &rJSArg = m_aArgs[xArg];
        ArgSink iArgSink (rJSArg, pIsolate);
        rPack.parameterValue (xArg).supply (iArgSink);
        if (rJSArg.IsEmpty ())
            rJSArg = pIsolate->LocalUndefined ();
    }
}
/*----------------*/
VA::Node::Isolate::Args::~Args () {
}
/*----------------*/


/**********************
 ****  Maybe Call  ****
 **********************/

bool VA::Node::Isolate::MaybeSetResultToCall (
    vxa_result_t &rResult, local_value_t hReceiver, local_value_t hCallable, vxa_pack_t rPack
) {
    return MaybeSetResultToCallOf<local_function_t> (rResult, hReceiver, hCallable, rPack)
        || MaybeSetResultToCallOf<local_object_t>   (rResult, hReceiver, hCallable, rPack);
}

bool VA::Node::Isolate::MaybeSetResultToCall (
    vxa_result_t &rResult, local_value_t hReceiver, local_function_t hCallable, vxa_pack_t rPack
) {
    v8::TryCatch iCatcher (*this);
    Args args (this, rPack);
    return MaybeSetResultToValue (
        rResult, hCallable->Call (context (), hReceiver, args.argc (), args.argv ())
    ) || MaybeSetResultToError (rResult, iCatcher);
}

bool VA::Node::Isolate::MaybeSetResultToCall (
    vxa_result_t &rResult, local_value_t hReceiver, local_object_t hCallable, vxa_pack_t rPack
) {
    v8::TryCatch iCatcher (*this);
    Args args (this, rPack);
    return hCallable->IsCallable () && (
        MaybeSetResultToValue (
            rResult, hCallable->CallAsFunction (
                context (), hReceiver, args.argc (), args.argv ()
            )
        ) || MaybeSetResultToError (rResult, iCatcher)
    );
}


/*************************
 *****  Maybe Error  *****
 *************************/

bool VA::Node::Isolate::MaybeSetResultToError (
    vxa_result_t &rResult, v8::TryCatch &rCatcher
) {
    return rCatcher.HasCaught () && MaybeSetResultToValue (
        rResult, rCatcher.Exception ()
    );
}


/*************************
 *****  Maybe Value  *****
 *************************/

bool VA::Node::Isolate::MaybeSetResultToValue (
    vxa_result_t &rResult, local_value_t hValue
) {
    return !hValue.IsEmpty () && (
        MaybeSetResultToInt32  (rResult, hValue) ||
        MaybeSetResultToDouble (rResult, hValue) ||
        MaybeSetResultToString (rResult, hValue) ||
        MaybeSetResultToObject (rResult, hValue)
    );
}

bool VA::Node::Isolate::MaybeSetResultToInt32 (
    vxa_result_t &rResult, local_value_t hValue
) {
    int32_t iResult;
    if (hValue->IsInt32 () && GetUnwrapped (iResult, hValue)) {
        rResult = iResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::MaybeSetResultToDouble (
    vxa_result_t &rResult, local_value_t hValue
) {
    double iResult;
    if (hValue->IsNumber () && GetUnwrapped (iResult, hValue)) {
        rResult = iResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::MaybeSetResultToString (
    vxa_result_t &rResult, local_value_t hValue
) {
    VString iResult;
    if (hValue->IsString () && UnwrapString (iResult, hValue)) {
        rResult = iResult;
        return true;
    }
    return false;
}

bool VA::Node::Isolate::MaybeSetResultToObject (
    vxa_result_t &rResult, local_value_t hValue
) {
    ExportReference pResult;
    if (Attach (pResult, hValue)) {
        rResult = pResult;
        return true;
    }
    return false;
}

/*******************************
 *****  SetResultToGlobal  *****
 *******************************/

bool VA::Node::Isolate::SetResultToGlobal (vxa_result_t &rResult) {
    local_value_t hGlobal = context()->Global ();
    return SetResultToValue (rResult, hGlobal);
}

/**********************************
 *****  SetResultToUndefined  *****
 **********************************/

bool VA::Node::Isolate::SetResultToUndefined (vxa_result_t &rResult) {
    local_value_t hUndefined; ExportReference pResult;
    if (GetLocalUndefined (hUndefined) && Attach (pResult, hUndefined)) {
        rResult = pResult;
    } else {
        rResult = false;
    }
    return true;
}
