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
