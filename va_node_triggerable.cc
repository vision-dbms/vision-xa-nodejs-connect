/*****  VA_Node_Triggerable Implementation  *****/

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

#include "va_node_triggerable.h"

/************************
 *****  Supporting  *****
 ************************/

#include <node.h>
#include <iostream>


/***********************************
 ***********************************
 *****                         *****
 *****  VA::Node::Triggerable  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Triggerable::Triggerable (
    Isolated *pIsolated
) : Triggerable (pIsolated->isolate ()) {
}

VA::Node::Triggerable::Triggerable (
    Isolate *pIsolate
) : BaseClass (pIsolate), m_bRunning (true) {
    assert (
        uv_async_init (uv_default_loop (), &m_iTrigger, &ThisClass::Run) == 0
    );
    m_iTrigger.data = this;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Triggerable::~Triggerable () {
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Triggerable::decommision () {
    uv_close (reinterpret_cast<uv_handle_t*>(&m_iTrigger), NULL);
    return BaseClass::decommision ();
}


/**************************************
 **************************************
 *****  Triggering and Execution  *****
 **************************************
 **************************************/

void VA::Node::Triggerable::trigger () {
    std::cerr
        << "VA::Node::Triggerable["
        << this
        << "]::trigger: "
        << referenceCount ()
        << std::endl;
    retain ();  // reference removed below in 'Run'
    uv_async_send (&m_iTrigger);
}

void VA::Node::Triggerable::Run (uv_async_t *pHandle) {
    Reference const pThis (static_cast<ThisClass*>(pHandle->data));
    if (pThis)
        pThis->Run ();
}

void VA::Node::Triggerable::Run () {
    std::cerr
        << "VA::Node::Triggerable["
        << this
        << "]::Run: "
        << referenceCount ()
        << std::endl;
    v8::HandleScope iScope (isolateHandle ());
    run ();
    release ();  // ... reference created by 'schedule' removed here.
}
