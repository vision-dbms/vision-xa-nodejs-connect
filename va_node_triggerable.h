#ifndef VA_Node_Triggerable_Interface
#define VA_Node_Triggerable_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_isolated.h"
#include <uv.h>

#include "V_VTwiddler.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Triggerable : public Isolated {
            DECLARE_ABSTRACT_RTTLITE (Triggerable, Isolated);

        //  Construction
        protected:
            Triggerable (Isolated *pIsolated);
            Triggerable (Isolate *pIsolate);

        //  Destruction
        protected:
            ~Triggerable ();

        //  Decommisioning
        protected:
            virtual bool decommision () override;

        //  Triggering
        protected:
            void trigger ();

        //  Execution
        private:
            static void Run (uv_async_t *pHandle);
            void Run ();

            virtual void run () = 0;

        //  State
        private:
            uv_async_t   m_iTrigger;
            V::VTwiddler m_bRunning;
        };
    } // namespace VA::Node

} // namespace VA


#endif
