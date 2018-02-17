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

bool VA::Node::Triggerable::onDeleteThis () {
/************************************************************************************
 *  This destructor may be called from any thread.  Since the 'uv' library isn't
 *  thread safe, it's only safe to call 'uv_close' from node's main thread (a.k.a.
 *  'uv_default_loop()').  Make sure that happens by noting that 'm_iTrigger.data'
 *  is initialized on construction to the non-null value of 'this'.  Use that fact
 *  to create a two phase destruction process.  The first phase begins when this
 *  object first becomes reclaimable.  Because that can happen in any thread, this
 *  this routine responds by clearing 'm_bRunning', triggering this triggerable,
 *  and returning false to prevent the destructor call from happening at this point.
 *  Because calling 'trigger' creates a reference to this object that will be cleared
 *  when the trigger is processed, this routine will be called again, this time in
 *  node's 'uv_default_loop ()' main thread.  At that point, this routine can safely
 *  call 'uv_close' and return 'true' to complete destruction.
 ************************************************************************************/
    if (m_bRunning.clearIfSet ()) {
        std::cerr
            << "VA::Node::Triggerable["
            << this
            << "]::onDeleteThis (1): "
            << referenceCount ()
            << std::endl;
        trigger ();
        return false;
    }
    std::cerr
        << "VA::Node::Triggerable["
        << this
        << "]::onDeleteThis (2): "
        << referenceCount ()
        << std::endl;
    uv_close (reinterpret_cast<uv_handle_t*>(&m_iTrigger), NULL);
    return true;
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
