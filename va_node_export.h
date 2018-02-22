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

        //  Decommisioning
        private:
            virtual bool decommision () override;

        //  Access
        private:
            handle_t object () const {
                return isolate ()->Local (m_hObject);
            }

        //  Methods
        public:
            void loopbackInt (Vxa::VResultBuilder &rRB, int i);
            void loopbackAny (Vxa::VResultBuilder &rRB, Vxa::VAny::value_t);

	    void interceptor (Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t);

            void adder (Vxa::VResultBuilder &rRB, Vxa::VPack<double>::value_t pack_o_ds);
            void JSToString (Vxa::VResultBuilder &rRB);
            void JSToDetail (Vxa::VResultBuilder &rRB);

        //  State
        private:
            global_t const m_hObject;
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
