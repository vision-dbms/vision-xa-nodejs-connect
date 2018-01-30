#ifndef VA_Node_Export_Interface
#define VA_Node_Export_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

#include "va_node_gateway.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        using v8::Isolate;
        using v8::Local;
        using v8::Persistent;
        using v8::Value;

    /*------------------------*
     *----  class Export  ----*
     *------------------------*/

        class Export : public Vxa::VCollectableObject {
            DECLARE_CONCRETE_RTTLITE (Export, Vxa::Object);

	//  class ClassBuilder
	public:
            class ClassBuilder;

        //  Construction
        private:
            Export (Isolate *pIsolate, Local<Value> iValue);

        //  Destruction
        private:
            ~Export ();

        //  State
        private:
            Gateway::Reference const m_pGateway;

        };

    /*--------------------------------------*
     *----  class Export::ClassBuilder  ----*
     *--------------------------------------*/

        class Export::ClassBuilder : public Vxa::Object::ClassBuilder {
        protected:
            ClassBuilder (Vxa::VClass *pClass);
        };

    } // namespace Node

} // namespace VA


#endif
