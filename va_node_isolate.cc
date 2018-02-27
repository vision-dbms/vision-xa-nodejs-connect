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

bool VA::Node::Isolate::GetString (VString &rString, maybe_string_t hString) const {
    local_string_t hLocalString;
    return hString.ToLocal<string_t> (&hLocalString) && GetString (rString, hLocalString);
}

bool VA::Node::Isolate::GetString (VString &rString, local_string_t hString) const {
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
    return resolver_t::New (getCurrentContext ()).ToLocalChecked ();
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
    Export::Reference pValue;
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
    ClassTraits<Export>::retaining_ptr_t &rpModelObject, maybe_value_t hValue
) {
    local_value_t hLocalValue;
    return hValue.ToLocal<value_t> (&hLocalValue) && Attach (rpModelObject, hLocalValue);
}

bool VA::Node::Isolate::Attach (
    ClassTraits<Export>::retaining_ptr_t &rpModelObject, local_value_t hValue
) {
    HandleScope iHS (this);

    object_cache_handle_t hValueCache (Local (m_hValueCache));
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
            << " attached"
            << std::endl;
    }
    return rpModelObject.isntNil ();
}

bool VA::Node::Isolate::Detach (Export *pModelObject) {
    HandleScope iHS (this);
    bool const bResult = pModelObject && Local (m_hValueCache)->Delete (pModelObject->value ());

    std::cerr
        << "VA::Node::Isolate["
        << this
        << "]::Detach: "
        << pModelObject
        << (bResult ? " detached" : " retained")
        << std::endl;

    return bResult;
}

/*****************************************************************

class V8_EXPORT NativeWeakMap : public Data {
 public:
  static Local<NativeWeakMap> New(Isolate* isolate);
  void Set(Local<Value> key, Local<Value> value);
  Local<Value> Get(Local<Value> key) const;
  bool Has(Local<Value> key);
  bool Delete(Local<Value> key);

 *****************************************************************/
