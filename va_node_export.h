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
            void loopbackAny (Vxa::VResultBuilder &rRB, Vxa::VAny::value_t);

	    void interceptor (Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t);

            void adder (Vxa::VResultBuilder &rRB, Vxa::VPack<double>::value_t pack_o_ds);

        //  State
        private:
            Isolate::Reference const m_pIsolate;
            global_t           const m_hObject;
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
