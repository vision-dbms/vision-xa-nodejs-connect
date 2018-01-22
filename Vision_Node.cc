/*****  Vision_Node Implementation  *****/

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

#include "Vision_Node.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****                *****
 *****  Vision::Node  *****
 *****                *****
 **************************
 **************************/

/*************************************************************
Node notes:

Adapter processing of `jsobj.name`...

- call `v8::Object::Get` to obtain value
- not found? return undefined
- function? return result of call
- else return value
*************************************************************/

namespace Vision {
    namespace Node {
    }
}
