/*****  VA_Node_Process Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "va_node_process.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_callback.h"
#include "va_node_isolated.h"

#include "V_VQueue.h"

#include <unordered_map>
#include <uv.h>


/***********************************************************
 ***********************************************************
 *****                                                 *****
 *****  {}::template <typename Request> class UVQueue  *****
 *****                                                 *****
 ***********************************************************
 ***********************************************************/

namespace {
    template <typename Request> class UVQueue {
        DECLARE_NUCLEAR_FAMILY (UVQueue<Request>);

    public:
        UVQueue (uv_async_cb pCB, void *pData) : m_xState (Running) {
            uv_async_init (uv_default_loop (), &m_hTrigger, pCB);
            m_hTrigger.data = pData;
        }
        ~UVQueue () {
            close ();
        }

        bool running () const {
            return Running == m_xState;
        }
        bool stopping () const {
            return Stopping == m_xState;
        }
        bool stopped () const {
            return Stopped == m_xState;
        }

        bool enqueue (Request const &rRequest) {
            if (stopped ())
                return false;

            bool const bFirst = m_iQueue.enqueue (rRequest);
            uv_async_send (&m_hTrigger);
            return bFirst;
        }
        bool dequeue (Request &rRequest) {
            bool const bWorkToDo = m_iQueue.dequeue (rRequest);
            if (stopping ())
                stop ();
            return bWorkToDo;
        }

    public:
        void stop () {
            switch (m_xState) {
            case Running:
                m_xState = Stopping;
            //  No 'break;'
            case Stopping:
                if (m_iQueue.size () == 0) {
                    close ();
                }
            //  No 'break;'
            default:
                break;
            }
        }
    private:
        void close () {
            if (!stopped ()) {
                m_xState = Stopped;
                uv_close (reinterpret_cast<uv_handle_t*>(&m_hTrigger), NULL);
            }
        }

    //  State
    private:
        V::VQueue<Request> m_iQueue;
        uv_async_t m_hTrigger;
        enum {Running, Stopping, Stopped} m_xState;
    };
}

/****************************************
 ****************************************
 *****                              *****
 *****  VA::Node::Process::Primary  *****
 *****                              *****
 ****************************************
 ****************************************/

class VA::Node::Process::Primary final : public Process {
    DECLARE_CONCRETE_RTTLITE (Primary, Process);

//  Construction
public:
    Primary ();

//  Destruction
private:
    ~Primary ();

//  Access
public:
    virtual size_t isolateCacheSize () const override {
        return m_iIsolateCache.size ();
    }

//  Model Management
public:
    virtual bool attach (
        ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate
    ) override;
    virtual bool detach (Isolate *pModelObject) override;

//  Object Management
private:
    virtual bool okToDecommission (Isolated *pIsolated) override;

//  Shutdown
private:
    virtual bool onShutdown (Isolate *pIsolate) override;

//  Queue Management
public:
    void enqueueDecommission (Isolated *pIsolated);
private:
    static void ProcessDecommissions (uv_async_t *pAsyncHandle);
    void processDecommissions ();

//  Callback Management
public:
    virtual void scheduleCallback (Callback *pCallback) override;
private:
    static void ProcessCallbacks (uv_async_t *pAsyncHandle);
    void processCallbacks ();

//  State
private:
    std::unordered_map<v8::Isolate*,ClassTraits<Isolate>::notaining_ptr_t> m_iIsolateCache;
    UVQueue<Isolated::Reference> m_qDecommissions;
    UVQueue<Callback::Reference> m_qCallbacks;
};


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Process::Primary::Primary (
) : m_qDecommissions (
    &ThisClass::ProcessDecommissions, this
), m_qCallbacks (
    &ThisClass::ProcessCallbacks, this
) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Process::Primary::~Primary () {
}

/******************************
 ******************************
 *****  Model Management  *****
 ******************************
 ******************************/

bool VA::Node::Process::Primary::attach (
    ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate
) {
    ClassTraits<Isolate>::notaining_ptr_t &rpCachedModelObject = m_iIsolateCache[pIsolate];
    if (rpCachedModelObject)
        rpModelObject.setTo (rpCachedModelObject);
    else {
        rpModelObject.setTo (new Isolate (pIsolate));
        rpCachedModelObject.setTo (rpModelObject);
    }
    return rpModelObject.isntNil ();
}

bool VA::Node::Process::Primary::detach (Isolate *pModelObject) {
    return pModelObject && m_iIsolateCache.erase (pModelObject->m_hIsolate) == 1;
}

/*******************************
 *******************************
 *****  Object Management  *****
 *******************************
 *******************************/

bool VA::Node::Process::Primary::okToDecommission (Isolated *pIsolated) {
    return true;
}

void VA::Node::Process::Primary::enqueueDecommission (Isolated *pIsolated) {
    m_qDecommissions.enqueue (Isolated::Reference (pIsolated));
}


bool VA::Node::Process::Primary::onShutdown (Isolate *pIsolate) {
    m_qDecommissions.stop ();
    m_qCallbacks.stop ();
    return true;
}

void VA::Node::Process::Primary::ProcessDecommissions (uv_async_t *pHandle) {
    ThisClass* const pThis (static_cast<ThisClass*>(pHandle->data));
    if (pThis)
        pThis->processDecommissions ();
}

void VA::Node::Process::Primary::processDecommissions () {
    Isolated::Reference pDecommissionable;
    while (m_qDecommissions.dequeue (pDecommissionable));
}

/*********************************
 *********************************
 *****  Callback Management  *****
 *********************************
 *********************************/

void VA::Node::Process::Primary::scheduleCallback (Callback *pCallback) {
    m_qCallbacks.enqueue (Callback::Reference (pCallback));
}

void VA::Node::Process::Primary::ProcessCallbacks (uv_async_t *pHandle) {
    ThisClass* const pThis (static_cast<ThisClass*>(pHandle->data));
    if (pThis)
        pThis->processCallbacks ();
}

void VA::Node::Process::Primary::processCallbacks () {
    Callback::Reference pCallback;
    while (m_qCallbacks.dequeue (pCallback))
        pCallback->run ();
}


/******************************************
 ******************************************
 *****                                *****
 *****  VA::Node::Process::Secondary  *****
 *****                                *****
 ******************************************
 ******************************************/

class VA::Node::Process::Secondary final : public Process {
    DECLARE_CONCRETE_RTTLITE (Secondary, Process);

//  Construction
public:
    Secondary (Primary *pPrimary) : m_pPrimary (pPrimary) {
    }

//  Destruction
private:
    ~Secondary () {
    }

//  Access
public:
    virtual size_t isolateCacheSize () const override {
        return m_pPrimary->isolateCacheSize ();
    }

//  Model Management
private:
    virtual bool attach (
        ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate
    ) override {
        return m_pPrimary->attach (rpModelObject, pIsolate);
    }
    virtual bool detach (Isolate *pModelObject) override {
        return m_pPrimary->detach (pModelObject);
    }

//  Object Management
private:
    virtual bool okToDecommission (Isolated *pIsolated) override;
    virtual bool onShutdown (Isolate *pIsolate) override;

//  Callback Management
private:
    virtual void scheduleCallback (Callback *pCallback) override;

//  State
private:
    Primary::Reference const m_pPrimary;
};


/******************************
 ******************************
 *****  Model Management  *****
 ******************************
 ******************************/

/*********************************
 *********************************
 *****  Lifetime Management  *****
 *********************************
 *********************************/

bool VA::Node::Process::Secondary::okToDecommission (Isolated *pIsolated) {
    m_pPrimary->enqueueDecommission (pIsolated);
    return false;
}

bool VA::Node::Process::Secondary::onShutdown (Isolate *pIsolate) {
    return false;
}

/*********************************
 *********************************
 *****  Callback Management  *****
 *********************************
 *********************************/

void VA::Node::Process::Secondary::scheduleCallback (Callback *pCallback) {
    m_pPrimary->scheduleCallback (pCallback);
}


/*******************************
 *******************************
 *****                     *****
 *****  VA::Node::Process  *****
 *****                     *****
 *******************************
 *******************************/

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

VA::Node::Process *VA::Node::Process::Implementation () {
    static Primary::Reference     g_pGlobalImplementation;
    thread_local static Reference g_pThreadImplementation;
    if (g_pThreadImplementation) {
    } else if (g_pGlobalImplementation) {
        g_pThreadImplementation = new Secondary (g_pGlobalImplementation);
    } else {
        g_pGlobalImplementation = new Primary ();
        g_pThreadImplementation = g_pGlobalImplementation;
    }
    return g_pThreadImplementation;
}
