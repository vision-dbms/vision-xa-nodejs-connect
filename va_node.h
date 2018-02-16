#ifndef VA_Node_Interface
#define VA_Node_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VExportable.h"

#include "v8.h"

#include <iostream>

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        typedef v8::Local<v8::Value>      value_handle_t;
        typedef v8::Local<v8::String>     string_handle_t;

        bool GetExport (
            Vxa::export_return_t &rResult, v8::Isolate *pIsolate, value_handle_t iValue
        );

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
     *>  This needs to move upstream to V or one of its classes  <*
     *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        template <typename T> struct ClassTraits {
            typedef V::VAggregatePointer<T> notaining_ptr_t;
            typedef VReference<T>           retaining_ptr_t;
        };

    } // namespace VA::Node
} // VA


#endif
