/*****  VA_Node_Export Implementation  *****/

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

#include "va_node_export.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCollectable.h"
//#include "Vxa_VResultBuilder.h"


/******************************
 ******************************
 *****                    *****
 *****  VA::Node::Export  *****
 *****                    *****
 ******************************
 ******************************/

/****************************
 ****************************
 *****  Access/Factory  *****
 ****************************
 ****************************/

bool VA::Node::Export::GetExport (Vxa::export_return_t &rExport, Isolate *pIsolate, Local<Value> iValue) {
    return false;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Export::Export (Isolate *pIsolate, Local<Value> iValue) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Export::~Export () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Export::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Vxa::Object::ClassBuilder (pClass) {
}

namespace {
    Vxa::VCollectable<VA::Node::Export> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(VA::Node::Export);
