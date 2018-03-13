#ifndef VA_Node_Export_Interface
#define VA_Node_Export_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_isolated.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Export : public Isolated {
            DECLARE_CONCRETE_RTTLITE (Export, Isolated);

            friend class Isolate;

        //  Class Builder
        public:
            class ClassBuilder
                : public BaseClass::ClassBuilder
            {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };
            friend class ClassBuilder;

        //  Construction
        private:
            Export (Isolate *pIsolate, local_value_t hValue);

        //  Destruction
        private:
            ~Export ();

        //  Decommisioning
        private:
            virtual bool decommision () override;

        //  Local Access
        private:
        //  ... this -> local handle
            local_value_t value () const {
                return LocalFor (m_hValue);
            }
            local_value_t GetLocal () const {
                return LocalFor (m_hValue);
            }
        //  ... this -> local handle (maybe)
            template <typename handle_t> bool GetLocal (handle_t &rhLocal) const {
                return this->GetLocalFrom (rhLocal, m_hValue);
            }

        //  Test Methods
        protected:
            void TestBool   (vxa_result_t &rResult, bool bTrue);
            void TestObject (vxa_result_t &rResult, Export* pObject);

        //  JS Methods
        protected:
            void JSCallback (vxa_result_t &rResult, vxa_pack_t const &rPack);
            void JSCall     (vxa_result_t &rResult, vxa_pack_t const &rPack);
            void JSNew      (vxa_result_t &rResult, vxa_pack_t const &rPack);

            void JSToString (vxa_result_t &rResult);
            void JSToDetail (vxa_result_t &rResult);
            void JSUnwrap (vxa_result_t &rResult);

            void JSHasProperty (vxa_result_t &rResult, VString const &rPropertyName);

            void JSIsUndefined (vxa_result_t &rResult);
            void JSIsNull (vxa_result_t &rResult);
            void JSIsNullOrUndefined (vxa_result_t &rResult);
            void JSIsTrue (vxa_result_t &rResult);
            void JSIsFalse (vxa_result_t &rResult);
            void JSIsName (vxa_result_t &rResult);
            void JSIsString (vxa_result_t &rResult);
            void JSIsSymbol (vxa_result_t &rResult);
            void JSIsFunction (vxa_result_t &rResult);
            void JSIsArray (vxa_result_t &rResult);
            void JSIsObject (vxa_result_t &rResult);
            void JSIsBoolean (vxa_result_t &rResult);
            void JSIsNumber (vxa_result_t &rResult);
            void JSIsExternal (vxa_result_t &rResult);
            void JSIsInt32 (vxa_result_t &rResult);
            void JSIsUint32 (vxa_result_t &rResult);
            void JSIsDate (vxa_result_t &rResult);
            void JSIsArgumentsObject (vxa_result_t &rResult);
            void JSIsBooleanObject (vxa_result_t &rResult);
            void JSIsNumberObject (vxa_result_t &rResult);
            void JSIsStringObject (vxa_result_t &rResult);
            void JSIsSymbolObject (vxa_result_t &rResult);
            void JSIsNativeError (vxa_result_t &rResult);
            void JSIsRegExp (vxa_result_t &rResult);
            void JSIsAsyncFunction (vxa_result_t &rResult);
            void JSIsGeneratorFunction (vxa_result_t &rResult);
            void JSIsGeneratorObject (vxa_result_t &rResult);
            void JSIsPromise (vxa_result_t &rResult);
            void JSIsMap (vxa_result_t &rResult);
            void JSIsSet (vxa_result_t &rResult);
            void JSIsMapIterator (vxa_result_t &rResult);
            void JSIsSetIterator (vxa_result_t &rResult);
            void JSIsWeakMap (vxa_result_t &rResult);
            void JSIsWeakSet (vxa_result_t &rResult);
            void JSIsArrayBuffer (vxa_result_t &rResult);
            void JSIsArrayBufferView (vxa_result_t &rResult);
            void JSIsTypedArray (vxa_result_t &rResult);
            void JSIsUint8Array (vxa_result_t &rResult);
            void JSIsUint8ClampedArray (vxa_result_t &rResult);
            void JSIsInt8Array (vxa_result_t &rResult);
            void JSIsUint16Array (vxa_result_t &rResult);
            void JSIsInt16Array (vxa_result_t &rResult);
            void JSIsUint32Array (vxa_result_t &rResult);
            void JSIsInt32Array (vxa_result_t &rResult);
            void JSIsFloat32Array (vxa_result_t &rResult);
            void JSIsFloat64Array (vxa_result_t &rResult);
            void JSIsDataView (vxa_result_t &rResult);
            void JSIsSharedArrayBuffer (vxa_result_t &rResult);
            void JSIsProxy (vxa_result_t &rResult);
            void JSIsWebAssemblyCompiledModule (vxa_result_t &rResult);

        //  State
        private:
            persistent_value_t const m_hValue;
        };
    } // namespace Node

} // namespace VA


#endif
