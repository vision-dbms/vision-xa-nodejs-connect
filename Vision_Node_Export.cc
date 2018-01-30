/*****  Vision_Node_Export Implementation  *****/

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

#include "Vision_Node_Export.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCollectable.h"
//#include "Vxa_VResultBuilder.h"


/**********************************
 **********************************
 *****                        *****
 *****  Vision::Node::Export  *****
 *****                        *****
 **********************************
 **********************************/

/****************************
 ****************************
 *****  Access/Factory  *****
 ****************************
 ****************************/

bool Vision::Node::Export::GetExport (Vxa::export_return_t &rExport, Isolate *pIsolate, Local<Value> iValue) {
    return false;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vision::Node::Export::Export (Isolate *pIsolate, Local<Value> iValue) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vision::Node::Export::~Export () {
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

Vision::Node::Export::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Vxa::Object::ClassBuilder (pClass) {
}

namespace {
    Vxa::VCollectable<Vision::Node::Export> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(Vision::Node::Export);
