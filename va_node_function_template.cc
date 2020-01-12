/*****  VA_Node_Function_Template Implementation  *****/

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

#include "va_node_function_template.h"

/************************
 *****  Supporting  *****
 ************************/


/****************************************
 ****************************************
 *****  VA::Node::FunctionTemplate  *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::FunctionTemplate::FunctionTemplate (
    Isolate *pIsolate, v8::FunctionCallback pCallback
) : BaseClass (pIsolate), m_hTemplate (
    isolateHandle (), function_template_t::New (isolateHandle (), pCallback)
) {
}

/***************************
 ***************************
 *****  Instantiation  *****
 ***************************
 ***************************/

bool VA::Node::FunctionTemplate::MaybeGetFunction (local_function_t &rResult) const {
    return GetLocalFor (
        rResult, templateHandle ()->GetFunction (context ())
    );
}
