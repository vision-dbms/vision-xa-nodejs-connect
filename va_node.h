#ifndef VA_Node_Interface
#define VA_Node_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include <iostream>
#include <iomanip>

#include "node.h"
#include "v8.h"

#include "Vxa_VAny.h"
#include "Vxa_VPack.h"
#include "Vxa_VResultBuilder.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        using V::VString;

    /********************************
     *----  Supported Features  ----*
     ********************************/
        enum VisionAPI {
        /*****************
         *  Entries should be added to this enumeration whenever new Vision facing
         *  methods are added to this adapter.  To ensure the integrity of this API
         *  versioning scheme, all new entries MUST BE ADDED IMMEDIATELY BEFORE the
         *  'VisionAPI_Generation' entry which MUST ALWAYS BE THE LAST ENTRY in this
         *  enumeration.  Additionally, entries MUST NEVER BE MOVED OR REMOVED after
         *  the features they or their successors in this enumeration represent have
         *  been deployed.
         *****************/

             VisionAPI_AtPrimitive,

        /****************
         *  Add all new feature entries immediately above this comment !!!
         ****************/
            VisionAPI_Generation  // MUST BE LAST IN THIS LIST
        };

    /*********************************
     *----  v8 Traits and Types  ----*
     *********************************/

    /*********************************************************************************
     *****  Handle Access Function Naming Conventions:
     *
     *      LocalFor     - unconditional handle form conversion (e.g, Persistent<T> to
     *                     Local<T>). Function returns local handle.
     *                   - no referenced content examination (i.e., same T).
     *      GetLocalFor  - conditional handle form conversion (e.g, MaybeLocal<T> to
     *                     Local<T>). Function returns true if successful, handle
     *                     returned by reference argument (typically the first).
     *                   - no referenced content examination (i.e., same T).
     *                   - same T (no referenced content examination).
     *      GetLocalFrom - conditional handle form conversion with referenced content
     *                     examination (e.g., Persistent<Value> -> Local<Object>).
     *                     Function return true if successful, handle returned by
     *                     reference argument (typically the first).
     *      GetLocal     - special case of GetLocalFrom typically defined as member
     *                     functions in classes whose instances contain the required
     *                     'From' data (e.g., VA::Node::Export).
     *
     **********************************************************************************/
        template <typename T> struct V8 {
            typedef v8::Local<T> local;
            typedef v8::MaybeLocal<T> maybe;
            typedef v8::Persistent<T> persistent;
            typedef v8::PersistentBase<T> persistent_base;
        };
        template <typename T> struct V8<v8::Local<T> > : public V8<T> {
            static bool GetLocalFor (
                typename V8<T>::local &rhLocal, typename V8<T>::local const &rhSomewhere
            ) {
                rhLocal = rhSomewhere;
                return true;
            }
        };
        template <typename T> struct V8<v8::MaybeLocal<T> > : public V8<T> {
            static bool GetLocalFor (
                typename V8<T>::local &rhLocal, typename V8<T>::maybe const &rhSomewhere
            ) {
                return rhSomewhere.ToLocal (&rhLocal);
            }
        };
        template <typename T> struct V8<v8::PersistentBase<T> > : public V8<T> {
            typedef v8::PersistentBase<T> source_handle_t;
        };
        template <typename T> struct V8<v8::Persistent<T> > : public V8<v8::PersistentBase<T> > {
            typedef v8::Persistent<T> source_handle_t;
        };

    /*----------------*/
        template <typename handle_t> bool GetLocalFor (
            typename V8<handle_t>::local &rhLocal, handle_t const &rhSomewhere
        ) {
            return V8<handle_t>::GetLocalFor (rhLocal, rhSomewhere);
        }

    /*----------------*/
        typedef v8::Boolean boolean_t;
        typedef V8<boolean_t>::local local_boolean_t;
        typedef V8<boolean_t>::maybe maybe_boolean_t;
        typedef V8<boolean_t>::persistent persistent_boolean_t;

        typedef v8::Context context_t;
        typedef V8<context_t>::local local_context_t;
        typedef V8<context_t>::maybe maybe_context_t;
        typedef V8<context_t>::persistent persistent_context_t;

        typedef v8::External external_t;
        typedef V8<external_t>::local local_external_t;
        typedef V8<external_t>::maybe maybe_external_t;
        typedef V8<external_t>::persistent persistent_external_t;

        typedef v8::Function function_t;
        typedef V8<function_t>::local local_function_t;
        typedef V8<function_t>::maybe maybe_function_t;
        typedef V8<function_t>::persistent persistent_function_t;

        typedef v8::FunctionTemplate function_template_t;
        typedef V8<function_template_t>::local local_function_template_t;
        typedef V8<function_template_t>::maybe maybe_function_template_t;
        typedef V8<function_template_t>::persistent persistent_function_template_t;

        typedef v8::Integer integer_t;
        typedef V8<integer_t>::local local_integer_t;
        typedef V8<integer_t>::maybe maybe_integer_t;
        typedef V8<integer_t>::persistent persistent_integer_t;

        typedef v8::Number number_t;
        typedef V8<number_t>::local local_number_t;
        typedef V8<number_t>::maybe maybe_number_t;
        typedef V8<number_t>::persistent persistent_number_t;

        typedef v8::Object object_t;
        typedef V8<object_t>::local local_object_t;
        typedef V8<object_t>::maybe maybe_object_t;
        typedef V8<object_t>::persistent persistent_object_t;

        typedef v8::ObjectTemplate object_template_t;
        typedef V8<object_template_t>::local local_object_template_t;
        typedef V8<object_template_t>::maybe maybe_object_template_t;
        typedef V8<object_template_t>::persistent persistent_object_template_t;

        typedef v8::Primitive primitive_t;
        typedef V8<primitive_t>::local local_primitive_t;
        typedef V8<primitive_t>::maybe maybe_primitive_t;
        typedef V8<primitive_t>::persistent persistent_primitive_t;

        typedef v8::Promise promise_t;
        typedef V8<promise_t>::local local_promise_t;
        typedef V8<promise_t>::maybe maybe_promise_t;
        typedef V8<promise_t>::persistent persistent_promise_t;

        typedef v8::String string_t;
        typedef V8<string_t>::local local_string_t;
        typedef V8<string_t>::maybe maybe_string_t;
        typedef V8<string_t>::persistent persistent_string_t;

        typedef v8::Value value_t;
        typedef V8<value_t>::local local_value_t;
        typedef V8<value_t>::maybe maybe_value_t;
        typedef V8<value_t>::persistent persistent_value_t;

    /**********************************
     *----  Vxa Traits and Types  ----*
     **********************************/

        typedef Vxa::VAny::value_t             vxa_any_t;
        typedef Vxa::VPack<vxa_any_t>::value_t vxa_pack_t;
        typedef Vxa::VResultBuilder            vxa_result_t;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
     *>  The following should probably move to somewhere in V  <*
     *>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
        template <typename T> struct ClassTraits {
            typedef V::VAggregatePointer<T> notaining_ptr_t;
            typedef VReference<T>           retaining_ptr_t;
        };

    } // namespace VA::Node
} // VA


#endif
