/*****  VA_Node_Isolated Implementation  *****/

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

#include "va_node_isolated.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_callback.h"

#include <iostream>


/********************************
 ********************************
 *****                      *****
 *****  VA::Node::Isolated  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Isolated::Isolated (
    Isolate *pIsolate
) : m_pIsolate (pIsolate) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Isolated::~Isolated () {
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Isolated::onDeleteThis () {
    return m_pIsolate->okToDecommission (this) && decommission ();
}

bool VA::Node::Isolated::decommission () {
    return true;
}

/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Isolated::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : BaseClass::ClassBuilder (pClass) {
}
