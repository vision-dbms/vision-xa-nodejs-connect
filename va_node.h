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
        template <typename T> struct V8 {
            typedef v8::Local<T> local;
            typedef v8::MaybeLocal<T> maybe;
            typedef v8::Persistent<T> persistent;
            typedef v8::PersistentBase<T> persistent_base;
        };
        template <typename T> struct V8<v8::Local<T> > : public V8<T> {
        };
        template <typename T> struct V8<v8::MaybeLocal<T> > : public V8<T> {
        };
        template <typename T> struct V8<v8::Persistent<T> > : public V8<T> {
        };
        template <typename T> struct V8<v8::PersistentBase<T> > : public V8<T> {
        };

        typedef v8::Boolean boolean_t;
        typedef V8<boolean_t>::local local_boolean_t;
        typedef V8<boolean_t>::maybe maybe_boolean_t;
        typedef V8<boolean_t>::persistent persistent_boolean_t;

        typedef v8::Context context_t;
        typedef V8<context_t>::local local_context_t;
        typedef V8<context_t>::maybe maybe_context_t;
        typedef V8<context_t>::persistent persistent_context_t;

        typedef v8::Object object_t;
        typedef V8<object_t>::local local_object_t;
        typedef V8<object_t>::maybe maybe_object_t;
        typedef V8<object_t>::persistent persistent_object_t;

        typedef v8::Primitive primitive_t;
        typedef V8<primitive_t>::local local_primitive_t;
        typedef V8<primitive_t>::maybe maybe_primitive_t;
        typedef V8<primitive_t>::persistent persistent_primitive_t;

        typedef v8::Promise promise_t;
        typedef V8<promise_t>::local local_promise_t;
        typedef V8<promise_t>::maybe maybe_promise_t;
        typedef V8<promise_t>::persistent persistent_promise_t;

        typedef promise_t::Resolver resolver_t;
        typedef V8<resolver_t>::local local_resolver_t;
        typedef V8<resolver_t>::maybe maybe_resolver_t;
        typedef V8<resolver_t>::persistent persistent_resolver_t;

        typedef v8::String string_t;
        typedef V8<string_t>::local local_string_t;
        typedef V8<string_t>::maybe maybe_string_t;
        typedef V8<string_t>::persistent persistent_string_t;

        typedef v8::Value value_t;
        typedef V8<value_t>::local local_value_t;
        typedef V8<value_t>::maybe maybe_value_t;
        typedef V8<value_t>::persistent persistent_value_t;

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
