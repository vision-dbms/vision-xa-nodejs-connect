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

/*************************************************************
Node notes:

Adapter processing of `jsobj.name`...

- call `v8::Object::Get` to obtain value
- not found? return undefined
- function? return result of call
- else return value
*************************************************************/

namespace VA {
    namespace Node {
    }
}
