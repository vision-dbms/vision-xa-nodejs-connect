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

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Export::Export (
    Isolate *pIsolate, handle_t hObject
) : m_pIsolate (pIsolate), m_hObject (pIsolate->isolate (), hObject) {
    std::cerr << "VA::Node::Export::Export: " << this << std::endl;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Export::~Export () {
    std::cerr << "VA::Node::Export::~Export: " << this << std::endl;
}

bool VA::Node::Export::onDeleteThis () {
//    return m_pIsolate->Detach (this);
    std::cerr << "VA::Node::Export::onDeleteThis: " << this << std::endl;
    return false;
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
