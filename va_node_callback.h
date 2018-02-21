#ifndef VA_Node_Callback_Interface
#define VA_Node_Callback_Interface

/************************
 *****  Components  *****
 ************************/

#include "va_node_entity.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node_process.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Callback : public Entity {
            DECLARE_ABSTRACT_RTTLITE (Callback, Entity);

            friend Process::Primary;

        //  Construction
        protected:
            Callback ();

        //  Destruction
        protected:
            ~Callback ();

        //  Triggering
        public:
            void trigger ();

        //  Execution
        private:
            virtual void run () = 0;
        };
    } // namespace VA::Node

} // namespace VA


#endif
