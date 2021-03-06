#ifndef VA_Node_Entity_Interface
#define VA_Node_Entity_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "va_node.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Entity : public Vxa::VCollectableObject {
            DECLARE_ABSTRACT_RTTLITE (Entity, Vxa::VCollectableObject);

        //  Class Builder
        public:
            class ClassBuilder
                : public BaseClass::ClassBuilder
            {
            protected:
                ClassBuilder (Vxa::VClass *pClass);
            };
            friend class ClassBuilder;

        //  Construction
        protected:
            Entity ();

        //  Destruction
        protected:
            ~Entity ();
        };
    } // namespace VA::Node
} // namespace VA


#endif
