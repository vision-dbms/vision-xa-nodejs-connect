#include <node.h>
#include <v8.h>

#include "Vk.h"

#include "V_VQueue.h"

#include "Vision_Evaluation_Gofer.h"

#include "va_node_export.h"
#include "va_node_resolver.h"
#include "va_node_triggerable.h"

#include "Vca_VcaGofer.h"
#include "Vca_IDirectory.h"
#include "Vca_IPipeFactory.h"

#include "Vca_VServerApplication.h"

namespace {
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

/************************
 *----  ResultSink  ----*
 ************************/

    class ResultSink : public Vca::VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (ResultSink, Vca::VRolePlayer);

    //  Aliases
    public:
        typedef VN::Resolver Resolver;

    //  class Resolution
    public:
        class Resolution : public Resolver::Resolution {
            DECLARE_CONCRETE_RTTLITE (Resolution, Resolver::Resolution);

        //  Friends
            friend class ResultSink;

        //  Construction
        public:
            Resolution (
                Resolver *pResolver, VE::Value iResult
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
        }; // class ResultSink::Resolution

    //  class Rejection
    public:
        class Rejection : public Resolver::Resolution {
            DECLARE_CONCRETE_RTTLITE (Rejection, Resolver::Resolution);

        //  Friends
            friend class ResultSink;

        //  Construction
        public:
            Rejection (
                Resolver *pResolver, Vca::IError *pInterface, VString const &rMessage
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
        }; // class ResultSink::Rejection

    //  Construction
    public:
        ResultSink (
            Resolver *pResolver, VE::evaluation_result_gofer_t *pResultGofer
        ) : m_pResolver (pResolver) {
            retain (); {
                pResultGofer->supplyMembers (this, &ThisClass::onData, &ThisClass::onError);
            } untain ();
        }

    //  Destruction
    private:
        ~ResultSink () {
        }

    //  Access
    public:
        Resolver::local_promise_t GetPromise () const {
            return m_pResolver->promise ();
        }

    //  Outcomes
    private:
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
        Resolver::Reference m_pResolver;
    };

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
        Vxa::export_return_t iExport;
        pIsolate->GetExport (iExport, args[0]);

        ServerContext::arg_storage_t aServerArgs (args.Length ());
        aServerArgs[0] = "-node-";
        for (unsigned int xArg = 1; xArg < aServerArgs.elementCount (); xArg++) {
            if (!pIsolate->UnwrapString (aServerArgs[xArg], args[xArg])) {
                VString iMessage;
                iMessage << "Invalid Argument: " << xArg;
                pIsolate->ThrowTypeError (iMessage);
            }
        }

        Server::Reference pServer (new Server (new ServerContext (aServerArgs), iExport));
        args.GetReturnValue ().Set (pServer->start ());
    }

/**********************
 *----  Evaluate  ----*
 **********************/

    void Evaluate(FunctionCallbackInfo<Value> const& args) {
        Vca::VCohortClaim cohortClaim;

        VN::Isolate::Reference pIsolate;
        VN::Isolate::GetInstance (pIsolate, args.GetIsolate());

    //  Access the required expression argument...
        VString iExpression;
        if (args.Length () < 1 || !pIsolate->UnwrapString (iExpression, args[0])) {
            pIsolate->ThrowTypeError ("Missing Expression");
            return;
        }

    //  Access the client export if supplied...
        Vxa::export_return_t iExport;
        if (args.Length () >= 2) {
            pIsolate->GetExport (iExport, args[1]);
        }

    //  Set up the evaluation...
        VE::Gofer::Reference const pGofer (
            new VE::Gofer (
                [&]() {
                    VString iEvaluatorName;
                    return args.Length () >= 3 && pIsolate->UnwrapString (iEvaluatorName, args[2])
                        ? EvaluatorGoferFor (iEvaluatorName) : DefaultEvaluator ();
                }(), iExpression, iExport
            )
        );

    //  Start the evaluation...
        ResultSink::Reference const pResultSink (
            new ResultSink (new VN::Resolver (pIsolate), pGofer)
        );

    //  And return the promise:
        args.GetReturnValue().Set (pResultSink->GetPromise ());
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
        NODE_SET_METHOD(exports, "v", Evaluate);
        NODE_SET_METHOD(exports, "o", Offer);
        NODE_SET_METHOD(exports, "cachedIsolateCount", CachedIsolateCount);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // anonymous namespace
