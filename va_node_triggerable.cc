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

#include "va_node_handle_scope.h"

#include <node.h>


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
) : BaseClass (pIsolate) {
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

bool VA::Node::Triggerable::decommission () {
    uv_close (reinterpret_cast<uv_handle_t*>(&m_iTrigger), NULL);
    return BaseClass::decommission ();
}


/**************************************
 **************************************
 *****  Triggering and Execution  *****
 **************************************
 **************************************/

void VA::Node::Triggerable::trigger () {
    retain ();  // reference removed below in 'Run'
    uv_async_send (&m_iTrigger);
}

void VA::Node::Triggerable::Run (uv_async_t *pHandle) {
    Reference const pThis (static_cast<ThisClass*>(pHandle->data));
    if (pThis)
        pThis->Run ();
}

void VA::Node::Triggerable::Run () {
    HandleScope iHS (this);
    run ();
    release ();  // ... reference created by 'schedule' removed here.
}
