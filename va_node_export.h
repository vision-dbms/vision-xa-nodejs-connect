#ifndef VA_Node_Export_Interface
#define VA_Node_Export_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_isolate.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {

    /*------------------------*
     *----  class Export  ----*
     *------------------------*/

        class Export : public Vxa::VCollectableObject {
            DECLARE_CONCRETE_RTTLITE (Export, Vxa::Object);

            friend class Isolate;

        //  Aliases
            typedef v8::Value object_t;
            typedef v8::Local<object_t>      handle_t;
            typedef v8::Persistent<object_t> global_t;

	//  class ClassBuilder
	public:
            class ClassBuilder;

        //  Construction
        private:
            Export (Isolate *pIsolate, handle_t hObject);

        //  Destruction
        private:
            ~Export ();

            bool onDeleteThis ();

        //  Access
            Isolate *isolate () const {
                return m_pIsolate;
            }
            handle_t object () const {
                return m_pIsolate->Local (m_hObject);
            }

        //  Methods
        public:
            void loopbackInt (Vxa::VResultBuilder &rRB, int i);
            void loopbackAny (Vxa::VResultBuilder &rRB, Vxa::VAny const &rAny);

	    void defthod (Vxa::VResultBuilder &rRB);

        //  State
        private:
            Isolate::Reference const m_pIsolate;
            global_t           const m_hObject;
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
