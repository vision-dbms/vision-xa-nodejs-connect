/*****  Vision_Node_Value Implementation  *****/

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

#include "Vision_Node_Value.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCollectable.h"
//#include "Vxa_VResultBuilder.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vision::Node::Value  *****
 *****                       *****
 *********************************
 *********************************/

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vision::Node::Value::~Value () {
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

Vision::Node::Value::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Vxa::Object::ClassBuilder (pClass) {
}

namespace {
    Vxa::VCollectable<Vision::Node::Value> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(Vision::Node::Value);
