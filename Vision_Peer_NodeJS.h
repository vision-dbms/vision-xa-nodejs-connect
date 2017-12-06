#ifndef Vision_Peer_NodeJS_Interface
#define Vision_Peer_NodeJS_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vision {
    namespace Peer {
        namespace NodeJS {
            template <typename T> class Wrapped {
            public:
                Wrapped (T *pT) {
                }
                ~Wrapped () {
                }

            public:
                template <typename S> Wrapped<T>& defineMethod (S pS) {
                }
            };
        } // namespace Vision::Peer::NodeJS
    } // namespace Vision::Peer
} // Vision


#endif
