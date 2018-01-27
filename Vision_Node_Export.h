#ifndef Vision_Node_Value_Interface
#define Vision_Node_Value_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

#include "Vision_Node_Store.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vision {
    namespace Node {
        using v8::Isolate;
        using v8::Local;
        using v8::Persistent;

        class Value : public Vxa::VCollectableObject {
            DECLARE_CONCRETE_RTTLITE (Value, Vxa::Object);

	//  Class Builder
	public:
	    class ClassBuilder : public Vxa::Object::ClassBuilder {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };

        //  Construction
        public:
            template <class V> Value (Isolate *pIsolate, V v)
                : m_pStore (Store::GetStoreFor (pIsolate))
                , m_xValue (m_pStore->insert (v))
            {
            }

        //  Destruction
        private:
            ~Value ();

        //  State
        private:
            Store::Reference const m_pStore;
            Store::index_t   const m_xValue;
        }; // class Value
    } // namespace Vision::Node
} // Vision


#endif
