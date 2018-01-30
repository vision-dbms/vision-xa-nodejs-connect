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

bool VA::Node::GetExport (Vxa::export_return_t &rExport, Local<v8::Context> iContext, Local<v8::Value> iValue) {
    return false;
}

bool VA::Node::GetExport (Vxa::export_return_t &rExport, v8::Isolate *pIsolate, Local<v8::Value> iValue) {
    return false;
}
