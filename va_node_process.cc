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

#include "va_node_isolate.h"


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
    Primary () {
    }

//  Destruction
private:
    ~Primary () {
    }

//  Access
public:
    virtual size_t isolateCacheSize () const override {
        return m_iIsolateCache.size ();
    }

//  Model Management
private:
    virtual bool attach (
        ClassTraits<Isolate>::retaining_ptr_t &rpModelObject, v8::Isolate *pIsolate
    ) override;
    virtual bool detach (Isolate *pModelObject) override;

//  Queue Management
private:
    virtual bool okToDecommision (Isolated *pIsolated) override;

//  State
private:
    std::unordered_map<v8::Isolate*,ClassTraits<Isolate>::notaining_ptr_t> m_iIsolateCache;
};


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


/****************************
 ****************************
 *****  Decommisioning  *****
 ****************************
 ****************************/

/**********************
 *----  Isolated  ----*
 **********************/

bool VA::Node::Process::Primary::okToDecommision (Isolated *pIsolated) {
#if 0
    rescheduleOnMainThread ();
#endif
    return false;
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
        return false;
    }
    virtual bool detach (Isolate *pModelObject) override {
        return false;
    }

//  Queue Management
private:
    virtual bool okToDecommision (Isolated *pIsolated) override;

//  State
private:
    Primary::Reference const m_pPrimary;
};


/******************************
 ******************************
 *****  Model Management  *****
 ******************************
 ******************************/

/****************************
 ****************************
 *****  Decommisioning  *****
 ****************************
 ****************************/

bool VA::Node::Process::Secondary::okToDecommision (Isolated *pIsolated) {
#if 0
    rescheduleOnMainThread ();
#endif
    return false;
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
