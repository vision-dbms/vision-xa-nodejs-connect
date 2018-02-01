/*****  VA_Node Implementation  *****/

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

#include "va_node.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_export.h"


/**********************
 **********************
 *****            *****
 *****  VA::Node  *****
 *****            *****
 **********************
 **********************/

/***************************
 ***************************
 *****  Export Access  *****
 ***************************
 ***************************/

bool VA::Node::GetExport (Vxa::export_return_t &rExport, Isolate::handle_t hIsolate, value_handle_t hObject) {
    Isolate::Reference pIsolate; Export::Reference pObject;
    if (Isolate::GetInstance (pIsolate, hIsolate) && pIsolate->Attach (pObject, hObject))
        rExport.setTo (Vxa::Export (pObject));
    else
        rExport.clear ();
    return rExport.isntNil ();
}
