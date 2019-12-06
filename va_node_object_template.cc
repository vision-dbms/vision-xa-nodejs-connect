/*****  VA_Node_Object_Template Implementation  *****/

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

#include "va_node_object_template.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************************
 **************************************
 *****  VA::Node::ObjectTemplate  *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::ObjectTemplate::ObjectTemplate (
    Isolate *pIsolate
) : BaseClass (pIsolate), m_hTemplate (
    isolateHandle (), object_template_t::New (isolateHandle ())
) {
    LocalFor (m_hTemplate)->SetInternalFieldCount (1);
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

bool VA::Node::ObjectTemplate::MaybeSetMemberFunction (
    VString const &rMemberName, v8::FunctionCallback pCallback
) {
    FunctionTemplate::Reference const pFunctionTemplate (new FunctionTemplate (isolate (), pCallback));

    local_string_t hMemberName;
    return NewString (hMemberName, rMemberName) && [&]() {
        templateHandle ()->Set (hMemberName, pFunctionTemplate->templateHandle ());
        return true;
    } ();
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

bool VA::Node::ObjectTemplate::MaybeGetInstance (local_object_t &rResult, void *pIFD) const {
    return GetLocalFor (
        rResult, LocalFor (m_hTemplate)->NewInstance (context ())
    ) && [&]() {
        rResult->SetInternalField (0, isolate ()->NewExternal (pIFD));
        return true;
    }();
}
