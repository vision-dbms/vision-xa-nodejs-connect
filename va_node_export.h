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
        using v8::Value;

        class Export : public Vxa::VCollectableObject {
            DECLARE_CONCRETE_RTTLITE (Export, Vxa::Object);

	//  Class Builder
	public:
	    class ClassBuilder : public Vxa::Object::ClassBuilder {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };

        //  Access/Factory
        public:
            static bool GetExport (Vxa::export_return_t &rExport, Isolate *pIsolate, Local<Value> iValue);

        //  Construction
        private:
            Export (Isolate *pIsolate, Local<Value> iValue);

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
