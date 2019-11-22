#include <node.h>
#include <v8.h>

#include "Vk.h"

#include "V_VQueue.h"

#include "Vision_Evaluation_Gofer.h"

#include "va_node_callback.h"
#include "va_node_export.h"
#include "va_node_handle_scope.h"

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
        static ServerContext *New (
            VN::Isolate *pIsolate, FunctionCallbackInfo<Value> const& args, int xFirstArg
        );
    private:
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
    ServerContext *ServerContext::New (
        VN::Isolate *pIsolate, FunctionCallbackInfo<Value> const& args, int xFirstArg
    ) {
        int const cArgs = args.Length () > xFirstArg ? args.Length () - xFirstArg : 0;
        ServerContext::arg_storage_t aServerArgs (cArgs + 1);
        aServerArgs[0] = "-node-";
        for (int xArg = 0; xArg < cArgs; xArg++) {
            if (!pIsolate->UnwrapString (aServerArgs[xArg + 1], args[xArg + xFirstArg])) {
                VString iMessage;
                iMessage << "Invalid Argument: " << xArg;
                pIsolate->ThrowTypeError (iMessage);
            }
        }
        return new ServerContext (aServerArgs);
    }

    ServerContext::ServerContext (
        arg_storage_t const &rArgs
    ) : m_aArgs (rArgs), m_pArgs (new char* [rArgs.elementCount ()]) {
        for (unsigned int xArg = 0; xArg < m_aArgs.elementCount (); xArg++) {
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

    /****************
     *  StateSnapshot
     ****************/
    public:
        class StateSnapshot {
        // Ctor/Dtor
        public:
            StateSnapshot (
                Server *pServer
            ) : m_pServer (
                pServer
            ), m_iRunStateName (
                pServer->runStateName ()
            ), m_iActivityCount (
                m_pServer->activityCount ()
            ), m_iActiveOfferCount (
                m_pServer->activeOfferCount ()
            ), m_iPassiveOfferCount (
                m_pServer->passiveOfferCount ()
            ), m_iListenerCount (
                m_pServer->listenerCount ()
            ) {
            }
            ~StateSnapshot () {
            }

        //  Access
        public:
            Server *server () const {
                return m_pServer;
            }
            VString const &runStateName () const {
                return m_iRunStateName;
            }

            Vca::count_t activityCount () const {
                return m_iActivityCount;
            }
            Vca::count_t activeOfferCount () const {
                return m_iActiveOfferCount;
            }
            Vca::count_t passiveOfferCount () const {
                return m_iPassiveOfferCount;
            }
            Vca::count_t listenerCount () const {
                return m_iListenerCount;
            }

        //  Passthrough
        public:
            void invokeStateCallback () const {
                m_pServer->invokeStateCallback (*this);
            }

        //  State
        private:
            Server::Reference const m_pServer;
            VString           const m_iRunStateName;
            Vca::count_t      const m_iActivityCount;
            Vca::count_t      const m_iActiveOfferCount;
            Vca::count_t      const m_iPassiveOfferCount;
            Vca::count_t      const m_iListenerCount;
        };

    /****************
     *  StateCallback
     ****************/
    public:
        class StateCallback final : public VN::Callback {
            DECLARE_CONCRETE_RTTLITE (StateCallback, VN::Callback);

        //  Construction
        public:
            StateCallback (
                Server *pServer
            ) : BaseClass (pServer->stateCallback()), m_iStateSnapshot (pServer) {
                retain (); {
                    trigger ();
                } untain ();
            }
        private:
            ~StateCallback () {
            }

        //  Execution
        private:
            void run () override {
                m_iStateSnapshot.invokeStateCallback ();
            }

        //  State
        private:
            StateSnapshot const m_iStateSnapshot;
        };
        friend class StateCallback;

    /****************
     *  Server
     ****************/

    //  Offer
    public:
        static void Offer(FunctionCallbackInfo<Value> const& args);

    //  Construction
    private:
        Server (
            ServerContext *pContext,
            Vxa::export_return_t const &rExport,
            VN::Export *pStateCallback = 0
        );

    //  Destruction
    private:
        ~Server ();

    //  Access
    private:
        VN::Export *stateCallback () const {
            return m_pStateCallback;
        }

    //  Control
    private:
        virtual bool start_() override;

        virtual void onRunStateChange_(RunState xOldState, RunState xNewState) override;

        void scheduleStateCallback ();
        void invokeStateCallback (StateSnapshot const &rSnapshot) const;

    //  State
    private:
        ServerContext::Reference const m_pServerContext;
        VN::Export::Reference    const m_pStateCallback;
    };

/****************/
    Server::Server (
        ServerContext *pContext, Vxa::export_return_t const &rExport, VN::Export *pStateCallback
    ) : BaseClass (pContext->applicationContext ()), m_pServerContext (
        pContext
    ), m_pStateCallback (pStateCallback) {
        aggregate (rExport);
    }

/****************/
    Server::~Server () {
    }

/****************/
    bool Server::start_() {
        return BaseClass::start_() && offerSelf () && isStarting ();
    }

    void Server::onRunStateChange_(RunState xOldState, RunState xNewState) {
        BaseClass::onRunStateChange_(xOldState, xNewState);
        scheduleStateCallback ();
    }

    void Server::scheduleStateCallback () {
        if (m_pStateCallback) {
            StateCallback::Reference const pStateCB (
                new StateCallback (this)
            );
        }
    }
    void Server::invokeStateCallback (StateSnapshot const &rSnapshot) const {
        VN::HandleScope iHS (m_pStateCallback);
        VN::local_value_t hResult;
        node::async_context aContext = {0,0};
        m_pStateCallback->Call (
            hResult, aContext, m_pStateCallback->NewObject (),
            rSnapshot.runStateName (),
            rSnapshot.activityCount (),
            rSnapshot.activeOfferCount (),
            rSnapshot.passiveOfferCount (),
            rSnapshot.listenerCount ()
        );
    }

/*************************************************************************
 *-----  Arguments:
 *
 *    arg[0]:  on stop callback
 *    arg[1]:  offered object
 *    arg[2+]: server command options
 *
 *************************************************************************/
    void Server::Offer(FunctionCallbackInfo<Value> const& args) {
        static const int xa_StateCallback    = 0;
        static const int xa_ExportedObject   = 1;
        static const int xa_ServerOptions    = 2;

        Vca::VCohortClaim cohortClaim;

        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate());

    //  Access the onStop callback (required) ...
        VN::Export::Reference pStateCallback;
        if (args.Length () <= xa_StateCallback || !pIsolate->Attach (
                pStateCallback, args[xa_StateCallback]
            )
        ) {
            pIsolate->ThrowTypeError ("Missing State Callback");
            return;
        }

    //  Access the server export (required) ...
        if (args.Length () <= xa_ExportedObject) {
            pIsolate->ThrowTypeError ("Missing Object");
            return;
        }
        Vxa::export_return_t pExport;
        pIsolate->GetExport (pExport, args[xa_ExportedObject]);

    //  ... and start the server:
        Vca::VCohortClaim cohortClaim2 (Vca::VCohort::Vca (),false);
        Server::Reference pServer (
            new Server (
                ServerContext::New (pIsolate, args, xa_ServerOptions), pExport, pStateCallback
            )
        );
        args.GetReturnValue ().Set (pServer->start ());
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
        ) : BaseClass (pCallback), m_pCallback (pCallback), m_iResult (rResult) {
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
            node::async_context aContext = {0,0};
            m_pCallback->Call (hResult, aContext, m_pCallback->NewObject (), m_iResult);
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
 *----  Vca Shutdown  ----*
 **************************/

    void Shutdown (FunctionCallbackInfo<Value> const& args) {
        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate ());
        args.GetReturnValue ().Set (pIsolate->onShutdown ());
    }

/**************************
 *----  State Access  ----*
 **************************/

    void CachedIsolateCount (FunctionCallbackInfo<Value> const& args) {
        args.GetReturnValue ().Set (
            static_cast<uint32_t>(VA::Node::Process::IsolateCacheSize ())
        );
    }

/**********************************
 *----  Module Initialization ----*
 **********************************/
    void Init (VN::local_object_t exports, VN::local_object_t module) {
        NODE_SET_METHOD(exports, "v" , ExternalEvaluator::Evaluate);
        NODE_SET_METHOD(exports, "o" , Server::Offer);

        NODE_SET_METHOD(exports, "shutdown", Shutdown);

        NODE_SET_METHOD(exports, "cachedIsolateCount", CachedIsolateCount);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
