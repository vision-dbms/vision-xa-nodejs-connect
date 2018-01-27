#ifndef Vision_Node_Export_Interface
#define Vision_Node_Export_Interface

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

        class Export : public Vxa::VCollectableObject {
            DECLARE_CONCRETE_RTTLITE (Export, Vxa::Object);

	//  Class Builder
	public:
	    class ClassBuilder : public Vxa::Object::ClassBuilder {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };

        //  Construction
        public:
            template <class V> Export (Isolate *pIsolate, V v)
                : m_pStore (Store::GetStoreFor (pIsolate))
            {
            }

        //  Destruction
        private:
            ~Export ();

        //  State
        private:
            Store::Reference const m_pStore;
        }; // class Export
    } // namespace Vision::Node
} // Vision


#endif
