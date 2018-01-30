#ifndef VA_Node_Interface
#define VA_Node_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VExportable.h"

#include "v8.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace VA {
    namespace Node {
        using v8::Local;

        bool GetExport (Vxa::export_return_t &rExport, v8::Isolate *pIsolate, Local<v8::Value> iValue);
        bool GetExport (Vxa::export_return_t &rExport, Local<v8::Context> iContext, Local<v8::Value> iValue);

    } // namespace VA::Node
} // VA


#endif
