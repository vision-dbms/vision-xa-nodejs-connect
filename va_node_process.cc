/*****  VA_Node_Process Implementation  *****/

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

#include "va_node_process.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_isolate.h"


/*******************************
 *******************************
 *****                     *****
 *****  VA::Node::Process  *****
 *****                     *****
 *******************************
 *******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Process::Process () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Process::~Process () {
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

bool VA::Node::Process::GetInstance (Reference &rpResult) {
    static Reference g_pInstance;
    rpResult.setTo (g_pInstance);         // ... grab the cached instance
    if (!rpResult) {                      // ... no cached instance?
        rpResult.setTo (new Process ());  // ... make a new one and race to safely remember it...
        if (!g_pInstance.interlockedSetIfNil (rpResult))
            rpResult.setTo (g_pInstance); // ... using the winner's result if we didn't win that race.
    }
    return rpResult.isntNil ();
}

/******************************
 ******************************
 *****  Model Management  *****
 ******************************
 ******************************/

bool VA::Node::Process::Attach (ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate) {
    ClassTraits<Isolate>::notaining_ptr_t &rpCachedModelObject = m_iIsolateCache[pIsolate];
    if (rpCachedModelObject)
        rpModelObject.setTo (rpCachedModelObject);
    else {
        rpModelObject.setTo (new Isolate (this, pIsolate));
        rpCachedModelObject.setTo (rpModelObject);
    }
    return rpModelObject.isntNil ();
}

bool VA::Node::Process::Detach (Isolate *pModelObject) {
    return pModelObject && m_iIsolateCache.erase (pModelObject->m_hIsolate) == 1;
}
