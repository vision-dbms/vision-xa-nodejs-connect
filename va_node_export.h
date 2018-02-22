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
            local_value_t object () const {
                return isolate ()->Local (m_hValue);
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
