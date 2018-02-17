#ifndef VA_Node_Isolated_Interface
#define VA_Node_Isolated_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

#include "va_node_isolate.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        class Isolated : public Vxa::VCollectableObject {
            DECLARE_ABSTRACT_RTTLITE (Isolated, Vxa::VCollectableObject);

        //  Construction
        protected:
            Isolated (Isolate *pIsolate);

        //  Destruction
        protected:
            ~Isolated ();

        //  Access
        public:
            Isolate *isolate () const {
                return m_pIsolate;
            }
            v8::Isolate *isolateHandle () const {
                return m_pIsolate->handle ();
            }
            v8::Local<v8::Context> context () const {
                return m_pIsolate->getCurrentContext ();
            }

        //  Handle Conversion
        protected:
            template <typename S> v8::Local<S> GetLocal (
                v8::PersistentBase<S> const &rThat
            ) const {
                return m_pIsolate->Local (rThat);
            }

        //  Object Creation
        protected:
            string_handle_t NewString (char const *pString) const {
                return m_pIsolate->NewString (pString);
            }

        //  State
        private:
            Isolate::Reference const m_pIsolate;
        };
    } // namespace VA::Node

} // namespace VA


#endif
