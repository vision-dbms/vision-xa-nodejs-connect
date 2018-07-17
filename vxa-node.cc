#include <node.h>
#include <v8.h>

#include "Vk.h"

#include "V_VQueue.h"

#include "Vision_Evaluation_Gofer.h"

#include "va_node_callback.h"
#include "va_node_export.h"
#include "va_node_handle_scope.h"
#include "va_node_resolver.h"

#include "Vca_VcaGofer.h"
#include "Vca_IDirectory.h"
#include "Vca_IPipeFactory.h"

#include "Vca_VServerApplication.h"

namespace VA {
    using V::VString;

    using v8::FunctionCallbackInfo;
    using v8::Value;

/*********************
 *----  Helpers  ----*
 *********************/
    namespace VE = Vision::Evaluation;
    namespace VN = VA::Node;

    char const *ValueOrElse (char const *pName, char const *pElse) {
        char const *pValue = getenv (pName);
        return pValue ? pValue : pElse;
    }
    VE::evaluator_gofer_t *EvaluatorGoferFor (VString const &rEvaluatorName) {
        VString iProtocol, iEvaluatorName;
        bool const bUsingNet = rEvaluatorName.getPrefix (
            ':', iProtocol, iEvaluatorName
        ) && iProtocol.equalsIgnoreCase ("net");
        if (bUsingNet)
            return new Vca::Gofer::Named<Vsa::IEvaluator,Vca::IPipeFactory> (iEvaluatorName);
        else
            return new Vca::Gofer::Named<Vsa::IEvaluator,Vca::IDirectory>   (iEvaluatorName);
    }

    VE::evaluator_gofer_t *DefaultEvaluator () {
        static VE::evaluator_gofer_t::Reference const pGofer (
            EvaluatorGoferFor (ValueOrElse ("NodeEvaluator", "dir:NodeEvaluator"))
        );
        return pGofer;
    }

/**********************************
 *----  class ServerContext  -----*
 **********************************/

    class ServerContext final : public VReferenceable {
        DECLARE_CONCRETE_RTTLITE (ServerContext, VReferenceable);

    //  Aliases
    public:
        typedef VkDynamicArrayOf<VString> arg_storage_t;

    //  Construction
    public:
        ServerContext (arg_storage_t const &rArgs);

    //  Destruction
    private:
        ~ServerContext () {
        }

    //  Application Context
    public:
        Vca::VApplicationContext *applicationContext () const;

    //  State
    private:
        arg_storage_t m_aArgs;
        argv_t        m_pArgs;
    };

/****************/
    ServerContext::ServerContext (
        arg_storage_t const &rArgs
    ) : m_aArgs (rArgs), m_pArgs (new char* [rArgs.elementCount ()]) {
        for (unsigned int xArg = 1; xArg < m_aArgs.elementCount (); xArg++) {
            m_pArgs[xArg] = m_aArgs[xArg].storage ();
        }
    }

/****************/
    Vca::VApplicationContext *ServerContext::applicationContext () const {
        return new Vca::VApplicationContext (m_aArgs.elementCount (), m_pArgs);
    }

/**************************
 *----  class Server  ----*
 **************************/

    class Server final : public Vca::VServerApplication {
        DECLARE_CONCRETE_RTTLITE (Server, VServerApplication);

    //  Construction
    public:
        Server (ServerContext *pContext, Vxa::export_return_t const &rExport);

    //  Destruction
    private:
        ~Server ();

    //  Control
    private:
        virtual bool start_() override;

    //  State
    private:
        ServerContext::Reference const m_pServerContext;
    };

/****************/
    Server::Server (
        ServerContext *pContext, Vxa::export_return_t const &rExport
    ) : BaseClass (pContext->applicationContext ()), m_pServerContext (pContext) {
        aggregate (rExport);
    }

/****************/
    Server::~Server () {
    }

/****************/
    bool Server::start_() {
        return BaseClass::start_() && offerSelf () && isStarting ();
    }

/*******************
 *----  Offer  ----*
 *******************/

    void Offer(FunctionCallbackInfo<Value> const& args) {
        Vca::VCohortClaim cohortClaim;

        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate());

    //  Access the server export...
        if (args.Length () < 1) {
            pIsolate->ThrowTypeError ("Missing Object");
            return;
        }
        Vxa::export_return_t pExport;
        pIsolate->GetExport (pExport, args[0]);

        ServerContext::arg_storage_t aServerArgs (args.Length ());
        aServerArgs[0] = "-node-";
        for (unsigned int xArg = 1; xArg < aServerArgs.elementCount (); xArg++) {
            if (!pIsolate->UnwrapString (aServerArgs[xArg], args[xArg])) {
                VString iMessage;
                iMessage << "Invalid Argument: " << xArg;
                pIsolate->ThrowTypeError (iMessage);
            }
        }

        Server::Reference pServer (new Server (new ServerContext (aServerArgs), pExport));
        args.GetReturnValue ().Set (pServer->start ());
    }

/*******************************
 *----  InternalEvaluator  ----*
 *******************************/

    class InternalEvaluator final : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (InternalEvaluator, Vca::VRolePlayer);

    //  Evaluate
    public:
        static void Evaluate(FunctionCallbackInfo<Value> const& args);

    //  class Resolution
    public:
        class Resolution : public VN::Resolver::Resolution {
            DECLARE_CONCRETE_RTTLITE (Resolution, VN::Resolver::Resolution);

        //  Friends
            friend class InternalEvaluator;

        //  Construction
        public:
            Resolution (
                VN::Resolver *pResolver, VE::Value iResult
            ) : BaseClass (pResolver), m_iResult (iResult) {
            }

        //  Destruction
        private:
            ~Resolution () {
            }

        //  Execution
        private:
            void run () override {
                Resolve (NewString (m_iResult.output ()));
            }

        //  State
        private:
            VE::Value const m_iResult;
        }; // class InternalEvaluator::Resolution

    //  class Rejection
    public:
        class Rejection : public VN::Resolver::Resolution {
            DECLARE_CONCRETE_RTTLITE (Rejection, VN::Resolver::Resolution);

        //  Friends
            friend class InternalEvaluator;

        //  Construction
        public:
            Rejection (
                VN::Resolver *pResolver, Vca::IError *pInterface, VString const &rMessage
            ) : BaseClass (pResolver), m_pInterface (pInterface), m_iMessage (rMessage) {
            }

        //  Destruction
        private:
            ~Rejection () {
            }

        //  Execution
        private:
            void run () override {
                Reject (NewString (m_iMessage));
            }

        //  State
        private:
            Vca::IError::Reference m_pInterface;
            VString const          m_iMessage;
        }; // class InternalEvaluator::Rejection

    //  Construction
    private:
        InternalEvaluator (VN::Isolate *pIsolate) : m_pResolver (new VN::Resolver (pIsolate)) {
        };
    //  Destruction
    private:
        ~InternalEvaluator () {
        }

    //  Access
    public:
        VN::Resolver::local_promise_t promise () const {
            return m_pResolver->promise ();
        }

    //  Resolution
    public:
        void resolveUsing (VE::Gofer *pGofer) {
            pGofer->supplyMembers (this, &ThisClass::onData, &ThisClass::onError);
        }

        void onData (VE::Value iResult) {
            Resolution::Reference const pOutcome (
                new Resolution (m_pResolver, iResult)
            );
            pOutcome->trigger ();
        }
        void onError (Vca::IError *pInterface, VString const &rMessage) {
            Rejection::Reference const pOutcome (
                new Rejection (m_pResolver, pInterface, rMessage)
            );
            pOutcome->trigger ();
        }

    //  State
    private:
        VN::Resolver::Reference const m_pResolver;
    };

/*************************************************************************
 *-----  Arguments:
 *
 *    arg[0]:  vision expression
 *    arg[1]:  optional client object
 *    arg[2]:  optional evaluator name
 *
 *************************************************************************/
    void InternalEvaluator::Evaluate(FunctionCallbackInfo<Value> const& args) {
        static const int xa_VisionExpression = 0;
        static const int xa_ExportedObject   = 1;
        static const int xa_EvaluatorName    = 2;

        Vca::VCohortClaim cohortClaim;

        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate());

    //  Create a resolver...
        InternalEvaluator::Reference const pResolver (new InternalEvaluator (pIsolate));
        args.GetReturnValue().Set (pResolver->promise ());

    //  Access the Vision expression (required)...
        VString iExpression;
        if (args.Length () <= xa_VisionExpression || !pIsolate->UnwrapString (
                iExpression, args[xa_VisionExpression]
            )
        ) {
            pResolver->onError (0, "Missing Expression");
            return;
        }

    //  Access the client export (optional)...
        Vxa::export_return_t pExport;
        if (args.Length () > xa_ExportedObject) {
            pIsolate->GetExport (pExport, args[xa_ExportedObject]);
        }

    //  Prepare the evaluation...
        VE::Gofer::Reference const pGofer (
            new VE::Gofer (
                [&]() {
                    VString iEvaluatorName;
                    return args.Length () > xa_EvaluatorName && pIsolate->UnwrapString (
                            iEvaluatorName, args[xa_EvaluatorName]
                    ) ? EvaluatorGoferFor (iEvaluatorName) : DefaultEvaluator ();
                }(), iExpression, pExport
            )
        );

    //  And start it:
        pResolver->resolveUsing (pGofer);
    }


/*******************************
 *----  ExternalEvaluator  ----*
 *******************************/

    class ExternalEvaluator : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ExternalEvaluator, Vca::VRolePlayer);

    //  Resolution
    public:
        class Resolution;

    //  Evaluate
    public:
        static void Evaluate(FunctionCallbackInfo<Value> const& args);

    //  Construction
    private:
        ExternalEvaluator (
            VN::Export *pRejectCallback, VN::Export *pResolveCallback, VE::Gofer *pGofer
        );
        ~ExternalEvaluator () {
        }

    //  Resolution
    private:
        void onData (VE::Value iResult);
        void onError (Vca::IError *pInterface, VString const &rMessage);

    //  State
    private:
        VN::Export::Reference m_pRejectCallback, m_pResolveCallback;
    };

/******************************************************************/
    class ExternalEvaluator::Resolution final : public VN::Callback {
        DECLARE_CONCRETE_RTTLITE (Resolution, VN::Callback);

    //  Construction
    public:
        Resolution (
            VN::Export *pCallback, VString const &rResult
        ) : m_pCallback (pCallback), m_iResult (rResult) {
            retain (); {
                trigger ();
            } untain ();
        }
    private:
        ~Resolution () {
        }

    //  Execution
    private:
        void run () override {
            VN::HandleScope iHS (m_pCallback);
            VN::local_value_t hResult;
            m_pCallback->Call (hResult, m_pCallback->LocalUndefined (), m_iResult);
        }

    //  State
    private:
        VN::Export::Reference const m_pCallback;
        VString               const m_iResult;
    };

/******************************************************************/
    ExternalEvaluator::ExternalEvaluator (
        VN::Export *pRejectCallback, VN::Export *pResolveCallback, VE::Gofer *pGofer
    ) : m_pRejectCallback (pRejectCallback), m_pResolveCallback (pResolveCallback) {
        retain (); {
            pGofer->supplyMembers (this, &ThisClass::onData, &ThisClass::onError);
        } untain ();
    };

/******************************************************************/
    void ExternalEvaluator::onData (VE::Value iResult) {
        (new Resolution (m_pResolveCallback, iResult.output()))->discard ();
    }
    void ExternalEvaluator::onError (Vca::IError *pInterface, VString const &rMessage) {
        (new Resolution (m_pRejectCallback, rMessage))->discard ();
    }

/*************************************************************************
 *-----  Arguments:
 *
 *    arg[0]:  promise constructor's 'resolve' callback
 *    arg[1]:  promise constructor's 'reject'  callback
 *    arg[2]:  vision expression
 *    arg[3]:  optional client object
 *    arg[4]:  optional evaluator name
 *
 *************************************************************************/
    void ExternalEvaluator::Evaluate(FunctionCallbackInfo<Value> const& args) {
        static const int xa_RejectCallback   = 0;
        static const int xa_ResolveCallback  = 1;
        static const int xa_VisionExpression = 2;
        static const int xa_ExportedObject   = 3;
        static const int xa_EvaluatorName    = 4;

        Vca::VCohortClaim cohortClaim;

        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate());

    //  Access the promise reject callback (required) ...
        VN::Export::Reference pRejectCallback;
        if (args.Length () <= xa_RejectCallback || !pIsolate->Attach (
                pRejectCallback, args[xa_RejectCallback]
            )
        ) {
            pIsolate->ThrowTypeError ("Missing Reject Callback");
            return;
        }

    //  Access the promise resolve callback (required) ...
        VN::Export::Reference pResolveCallback;
        if (args.Length () <= xa_ResolveCallback || !pIsolate->Attach (
                pResolveCallback, args[xa_ResolveCallback]
            )
        ) {
            VN::local_value_t hResult;
            pRejectCallback->Call (
                hResult, pIsolate->LocalUndefined (), "Missing Resolve Callback"
            );
            return;
        }

    //  Access the Vision expression (required)...
        VString iExpression;
        if (args.Length () <= xa_VisionExpression || !pIsolate->UnwrapString (
                iExpression, args[xa_VisionExpression]
            )
        ) {
            VN::local_value_t hResult;
            pRejectCallback->Call (
                hResult, pIsolate->LocalUndefined (), "Missing Expression"
            );
            return;
        }

    //  Access the client export if supplied...
        Vxa::export_return_t pExport;
        if (args.Length () > xa_ExportedObject) {
            pIsolate->GetExport (pExport, args[xa_ExportedObject]);
        }

    //  Prepare the evaluation...
        VE::Gofer::Reference const pGofer (
            new VE::Gofer (
                [&]() {
                    VString iEvaluatorName;
                    return args.Length () > xa_EvaluatorName && pIsolate->UnwrapString (
                        iEvaluatorName, args[xa_EvaluatorName]
                    ) ? EvaluatorGoferFor (iEvaluatorName) : DefaultEvaluator ();
                }(), iExpression, pExport
            )
        );

    //  ... and start it:
        (new ExternalEvaluator (pRejectCallback, pResolveCallback, pGofer))->discard ();
    }

/**************************
 *----  State Access  ----*
 **************************/

    void CachedIsolateCount (const FunctionCallbackInfo<Value>& args) {
        args.GetReturnValue ().Set (
            static_cast<uint32_t>(VA::Node::Process::IsolateCacheSize ())
        );
    }

/**********************************
 *----  Module Initialization ----*
 **********************************/
    void Init (VN::local_object_t exports, VN::local_object_t module) {
        NODE_SET_METHOD(exports, "v" , InternalEvaluator::Evaluate);
        NODE_SET_METHOD(exports, "v2", ExternalEvaluator::Evaluate);
        NODE_SET_METHOD(exports, "o", Offer);
        NODE_SET_METHOD(exports, "cachedIsolateCount", CachedIsolateCount);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
