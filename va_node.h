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
        template <typename type> struct V8 {
            typedef v8::Local<type> local;
            typedef v8::MaybeLocal<type> maybe;
            typedef v8::Persistent<type> persistent;
        };

        typedef v8::Value value_t;
        typedef typename V8<value_t>::local local_value_t;
        typedef typename V8<value_t>::maybe maybe_value_t;
        typedef typename V8<value_t>::persistent persistent_value_t;

        typedef v8::String string_t;
        typedef typename V8<string_t>::local local_string_t;
        typedef typename V8<string_t>::maybe maybe_string_t;
        typedef typename V8<string_t>::persistent persistent_string_t;

        typedef v8::Promise::Resolver resolver_t;
        typedef V8<resolver_t>::local local_resolver_t;
        typedef V8<resolver_t>::maybe maybe_resolver_t;
        typedef V8<resolver_t>::persistent persistent_resolver_t;

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
