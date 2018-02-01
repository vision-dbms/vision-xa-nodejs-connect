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
    Process *pProcess, handle_t hIsolate
) : m_pProcess (pProcess), m_hIsolate (hIsolate), m_hObjectCache (hIsolate, object_cache_t::New (hIsolate)) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Isolate::~Isolate () {
}

bool VA::Node::Isolate::onDeleteThis () {
//    return m_pProcess->Detach (this);
    return false;
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

bool VA::Node::Isolate::GetInstance (Reference &rpInstance, v8::Isolate *pIsolate) {
    Process::Reference pThisProcess;
    return Process::GetInstance (pThisProcess) && pThisProcess->Attach (rpInstance, pIsolate);
}

/******************************
 ******************************
 *****  Model Management  *****
 ******************************
 ******************************/

bool VA::Node::Isolate::Attach (
    ClassTraits<Export>::retaining_ptr_t &rpModelObject, v8::Local<v8::Object> hObject
) {
    return false;
}

bool VA::Node::Isolate::Attach (
    ClassTraits<Export>::retaining_ptr_t &rpModelObject, v8::Local<v8::Value> hObject
) {
    v8::HandleScope iHandleScope (m_hIsolate);

    object_cache_handle_t hObjectCache (Local (m_hObjectCache));
    value_handle_t const hCachedObject (hObjectCache->Get (hObject));

    if (!hCachedObject.IsEmpty () && hCachedObject->IsExternal ()) {
        rpModelObject.setTo (reinterpret_cast<Export*>(hCachedObject.As<v8::External>()->Value()));
        std::cerr
            << "VA::Node::Isolate::Attach: " << this << ", found: " << rpModelObject.referent () << std::endl;
    } else {
        rpModelObject.setTo (new Export (this, hObject));
        hObjectCache->Set (hObject, v8::External::New (m_hIsolate, rpModelObject.referent ()));
        std::cerr
            << "VA::Node::Isolate::Attach: " << this << ", added: " << rpModelObject.referent () << std::endl;
    }
    return rpModelObject.isntNil ();
}

bool VA::Node::Isolate::Detach (Export *pModelObject) {
    v8::HandleScope iHandleScope (m_hIsolate);
    bool const bResult = pModelObject && Local (m_hObjectCache)->Delete (pModelObject->object ());

    std::cerr << "VA::Node::Isolate::Detach<Export>: " << pModelObject << bResult << std::endl;

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
