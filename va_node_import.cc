/*****  VA_Node_Import Implementation  *****/

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

#include "va_node_import.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_handle_scope.h"
#include "va_node_function_template.h"
#include "va_node_object_template.h"


/******************************
 ******************************
 *****                    *****
 *****  VA::Node::Import  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Import::Import (
    Isolate *pIsolate
) : BaseClass (pIsolate) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Import::~Import () {
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Import::decommission () {
//    return isolate ()->Detach (this) && BaseClass::decommission ();
    return BaseClass::decommission ();
}

/********************************************
 ********************************************
 *****  External Access and Management  *****
 ********************************************
 ********************************************/

VA::Node::local_external_t VA::Node::Import::getExternal () {
    if (m_hExternal.IsEmpty ()) {
    //  The external we're about to create must hold a hard (retaining) reference to 'this'...
        retain ();
        local_external_t const hExternal (NewExternal (this));
        m_hExternal.Reset (isolateHandle (), hExternal);
        m_hExternal.SetWeak (
            this, &ThisClass::WeakCallbackFinalizer, v8::WeakCallbackType::kFinalizer
        );
        return hExternal;
    }
    return LocalFor (m_hExternal);
}

void VA::Node::Import::WeakCallbackFinalizer (v8::WeakCallbackInfo<ThisClass> const &rCallbackInfo) {
//  'WeakCallbackInfo::GetParameter()' supplying a pointer to the 'Import' object to be 'release'd:
    rCallbackInfo.GetParameter ()-> weakCallbackFinalizer (rCallbackInfo);
}

void VA::Node::Import::weakCallbackFinalizer (v8::WeakCallbackInfo<ThisClass> const &rCallbackInfo) {
//  Documentation in v8::WeakCallbackInfo suggests that 'm_hExternal.Reset()' is required here, ...
    m_hExternal.Reset ();

//  ... and, of course, we need to 'release' 'this':
    release ();
}


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Import::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : BaseClass::ClassBuilder (pClass) {
}

namespace {
    Vxa::VCollectable<VA::Node::Import> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(VA::Node::Import);
