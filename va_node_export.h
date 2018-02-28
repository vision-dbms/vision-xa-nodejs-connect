#ifndef VA_Node_Export_Interface
#define VA_Node_Export_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_isolated.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VAny.h"
#include "Vxa_VPack.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {

    /*------------------------*
     *----  class Export  ----*
     *------------------------*/

        class Export : public Isolated {
            DECLARE_CONCRETE_RTTLITE (Export, Isolated);

            friend class Isolate;

	//  class ClassBuilder
	public:
            class ClassBuilder;

        //  Construction
        private:
            Export (Isolate *pIsolate, local_value_t hValue);

        //  Destruction
        private:
            ~Export ();

        //  Decommisioning
        private:
            virtual bool decommision () override;

        //  Access
        private:
        /*************************************************************************
         *--->  The following 'Get*' will be migrating to VA::Node::Isolate  <---*
         *************************************************************************/
            template <typename handle_t> bool GetLocal (handle_t &rhLocal) const {
                typename V8<handle_t>::maybe hMaybe;
                return GetMaybe (hMaybe) && hMaybe.ToLocal (&rhLocal);
            }

            template <typename handle_t> bool GetMaybe (handle_t &rhMaybe) const {
                return false;
            }
            bool GetMaybe (V8<v8::Boolean>::maybe &rhMaybe) const {
                rhMaybe = value()->ToBoolean (context ());
                return true;
            }
            bool GetMaybe (V8<v8::Number>::maybe &rhMaybe) const {
                rhMaybe = value()->ToNumber (context ());
                return true;
            }
            bool GetMaybe (V8<v8::String>::maybe &rhMaybe, bool bToDetailString) const {
                rhMaybe = bToDetailString
                    ? value()->ToDetailString (context ())
                    : value()->ToString (context ());
                return true;
            }
            bool GetMaybe (V8<v8::Object>::maybe &rhMaybe) const {
                rhMaybe = value()->ToObject (context ());
                return true;
            }
            bool GetMaybe (V8<v8::Integer>::maybe &rhMaybe) const {
                rhMaybe = value()->ToInteger (context ());
                return true;
            }
            bool GetMaybe (V8<v8::Uint32>::maybe &rhMaybe) const {
                rhMaybe = value()->ToUint32 (context ());
                return true;
            }
            bool GetMaybe (V8<v8::Int32>::maybe &rhMaybe) const {
                rhMaybe = value()->ToInt32 (context ());
                return true;
            }
            bool GetMaybe (V8<v8::Value>::maybe &rhMaybe) const {
                rhMaybe = value();
                return true;
            }

            local_value_t value () const {
                return BaseClass::GetLocal (m_hValue);
            }

        //  Return Helpers
        private:
            void ReturnUnwrapped (Vxa::VResultBuilder &rRB, maybe_value_t hValue) const;
            void ReturnUnwrapped (Vxa::VResultBuilder &rRB, local_value_t hValue) const;

        //  Test Methods
        public:
            void loopbackAny (Vxa::VResultBuilder &rRB, Vxa::VAny::value_t);

	    void interceptor (Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t);
            void adder (Vxa::VResultBuilder &rRB, Vxa::VPack<double>::value_t pack_o_ds);

        //  JS Methods
        public:
            void JSCallback (Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t);

            void JSToString (Vxa::VResultBuilder &rRB);
            void JSToDetail (Vxa::VResultBuilder &rRB);
            void JSUnwrap (Vxa::VResultBuilder &rRB);

            void JSHasProperty (Vxa::VResultBuilder &rRB, VString const &rPropertyName);

            void JSIsUndefined (Vxa::VResultBuilder &rRB);
            void JSIsNull (Vxa::VResultBuilder &rRB);
            void JSIsNullOrUndefined (Vxa::VResultBuilder &rRB);
            void JSIsTrue (Vxa::VResultBuilder &rRB);
            void JSIsFalse (Vxa::VResultBuilder &rRB);
            void JSIsName (Vxa::VResultBuilder &rRB);
            void JSIsString (Vxa::VResultBuilder &rRB);
            void JSIsSymbol (Vxa::VResultBuilder &rRB);
            void JSIsFunction (Vxa::VResultBuilder &rRB);
            void JSIsArray (Vxa::VResultBuilder &rRB);
            void JSIsObject (Vxa::VResultBuilder &rRB);
            void JSIsBoolean (Vxa::VResultBuilder &rRB);
            void JSIsNumber (Vxa::VResultBuilder &rRB);
            void JSIsExternal (Vxa::VResultBuilder &rRB);
            void JSIsInt32 (Vxa::VResultBuilder &rRB);
            void JSIsUint32 (Vxa::VResultBuilder &rRB);
            void JSIsDate (Vxa::VResultBuilder &rRB);
            void JSIsArgumentsObject (Vxa::VResultBuilder &rRB);
            void JSIsBooleanObject (Vxa::VResultBuilder &rRB);
            void JSIsNumberObject (Vxa::VResultBuilder &rRB);
            void JSIsStringObject (Vxa::VResultBuilder &rRB);
            void JSIsSymbolObject (Vxa::VResultBuilder &rRB);
            void JSIsNativeError (Vxa::VResultBuilder &rRB);
            void JSIsRegExp (Vxa::VResultBuilder &rRB);
            void JSIsAsyncFunction (Vxa::VResultBuilder &rRB);
            void JSIsGeneratorFunction (Vxa::VResultBuilder &rRB);
            void JSIsGeneratorObject (Vxa::VResultBuilder &rRB);
            void JSIsPromise (Vxa::VResultBuilder &rRB);
            void JSIsMap (Vxa::VResultBuilder &rRB);
            void JSIsSet (Vxa::VResultBuilder &rRB);
            void JSIsMapIterator (Vxa::VResultBuilder &rRB);
            void JSIsSetIterator (Vxa::VResultBuilder &rRB);
            void JSIsWeakMap (Vxa::VResultBuilder &rRB);
            void JSIsWeakSet (Vxa::VResultBuilder &rRB);
            void JSIsArrayBuffer (Vxa::VResultBuilder &rRB);
            void JSIsArrayBufferView (Vxa::VResultBuilder &rRB);
            void JSIsTypedArray (Vxa::VResultBuilder &rRB);
            void JSIsUint8Array (Vxa::VResultBuilder &rRB);
            void JSIsUint8ClampedArray (Vxa::VResultBuilder &rRB);
            void JSIsInt8Array (Vxa::VResultBuilder &rRB);
            void JSIsUint16Array (Vxa::VResultBuilder &rRB);
            void JSIsInt16Array (Vxa::VResultBuilder &rRB);
            void JSIsUint32Array (Vxa::VResultBuilder &rRB);
            void JSIsInt32Array (Vxa::VResultBuilder &rRB);
            void JSIsFloat32Array (Vxa::VResultBuilder &rRB);
            void JSIsFloat64Array (Vxa::VResultBuilder &rRB);
            void JSIsDataView (Vxa::VResultBuilder &rRB);
            void JSIsSharedArrayBuffer (Vxa::VResultBuilder &rRB);
            void JSIsProxy (Vxa::VResultBuilder &rRB);
            void JSIsWebAssemblyCompiledModule (Vxa::VResultBuilder &rRB);

        //  State
        private:
            persistent_value_t const m_hValue;
        };

    /*--------------------------------------*
     *----  class Export::ClassBuilder  ----*
     *--------------------------------------*/

        class Export::ClassBuilder : public Isolated::ClassBuilder {
        protected:
            ClassBuilder (Vxa::VClass *pClass);
        };

    } // namespace Node

} // namespace VA


#endif
