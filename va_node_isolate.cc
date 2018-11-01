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

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

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

bool VA::Node::Isolate::okToDecommission (Isolated *pIsolated) const {
    return Process::OkToDecommission (pIsolated);
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
/************************************************/
/*
    std::cerr
        << "VA::Node::Isolate["
        << std::setw(14) << this
        << ","
        << std::setw(6) << callCount ()
        << "] GetExport "
        << pValue
        << " ["
        << pValue->objectCluster ()
        << " "
        << pValue->objectIndex ()
        << " / "
        << pValue->objectCluster ()->cardinality ()
        << "] "
        << rExport
        << std::endl;
*/
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

namespace {
    template <typename T> void Sink (T t) {
    }
}

bool VA::Node::Isolate::Attach (
    ExportReference &rpModelObject, local_value_t hValue
) {
    HandleScope iHS (this);
    local_context_t      const hContext = context ();
    local_object_cache_t const hCache (LocalFor (m_hValueCache));

    local_value_t hModelObject;
    if (GetLocalFor (hModelObject, hCache->Get (hContext, hValue)) && hModelObject->IsExternal ()) {
        rpModelObject.setTo (reinterpret_cast<Export*>(hModelObject.As<v8::External>()->Value()));

    /*
        std::cerr
            << "VA::Node::Isolate["
            << std::setw(14) << this
            << ","
            << std::setw(6) << callCount ()
            << "] Getting   "
            << rpModelObject.referent ()
            << std::endl;
    */
    } else {
        rpModelObject.setTo (new Export (this, hValue));
        Sink (
            hCache->Set (
                hContext, hValue, v8::External::New (m_hIsolate, rpModelObject.referent ())
            )
        );

    /*
        std::cerr
            << "VA::Node::Isolate["
            << std::setw(14) << this
            << ","
            << std::setw(6) << callCount ()
            << "] Setting   "
            << rpModelObject.referent ()
            << std::endl;
    */
    }
    return rpModelObject.isntNil ();
}

bool VA::Node::Isolate::Detach (Export *pModelObject) {
    HandleScope iHS (this);
    local_context_t       const hContext = context ();
    local_object_cache_t  const hCache = LocalFor (m_hValueCache);
    local_value_t         const hModelValue = pModelObject->value ();
    local_value_t               hCacheValue;
    bool                        bDeleted;

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
    /*
        std::cerr
            << "VA::Node::Isolate["
            << std::setw(14) << this
            << ","
            << std::setw(6) << callCount ()
            << "] Clearing  "
            << pModelObject
            << (bGone ? " Gone" : " Kept")
            << std::endl;
    */
        return bGone;
    } (
        !GetUnwrappedFromMaybe(
            bDeleted, hCache->Delete (hContext, hModelValue)
        )                                                              ||
        bDeleted                                                       ||
        !GetLocalFor (hCacheValue, hCache->Get(hContext, hModelValue)) ||
        hCacheValue.IsEmpty ()                                         ||
        !hCacheValue->IsExternal ()                                    ||
        hCacheValue.As<v8::External>()->Value() != pModelObject
    );
}


/**************************
 **************************
 *****  Call Helpers  *****
 **************************
 **************************/

/*********************
 *****  ArgSink  *****
 *********************/

class VA::Node::Isolate::ArgPack::ArgSink : public Vxa::VAny::Client {
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

/*********************
 *****  ArgPack  *****
 *********************/

/*----------------*/
VA::Node::Isolate::ArgPack::ArgPack (
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
VA::Node::Isolate::ArgPack::~ArgPack () {
}


/***************************
 ***************************
 *****  Result Return  *****
 ***************************
 ***************************/

/*************************
 *****  Maybe Value  *****
 *************************/

bool VA::Node::Isolate::MaybeSetResultToValue (
    local_value_t &rResult, local_value_t hValue
) {
    rResult = hValue;
    return true;
}

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

/**********************************
 *****  SetResultToUndefined  *****
 **********************************/

bool VA::Node::Isolate::SetResultToUndefined (local_value_t &rResult) {
    return GetLocalUndefined (rResult);
}

bool VA::Node::Isolate::SetResultToUndefined (vxa_result_t &rResult) {
    local_value_t hUndefined; ExportReference pResult;
    if (SetResultToUndefined (hUndefined) && Attach (pResult, hUndefined)) {
        rResult = pResult;
    } else {
        rResult = false;
    }
    return true;
}
