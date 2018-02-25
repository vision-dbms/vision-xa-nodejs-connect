/*****  VA_Node_Export Implementation  *****/

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

#include "va_node_export.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_handle_scope.h"

#include "Vxa_VAny.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"


/******************************
 ******************************
 *****                    *****
 *****  VA::Node::Export  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Export::Export (
    Isolate *pIsolate, local_value_t hValue
) : BaseClass (pIsolate), m_hValue (*pIsolate, hValue) {
    std::cerr << "VA::Node::Export::Export: " << this << std::endl;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Export::~Export () {
    std::cerr << "VA::Node::Export::~Export: " << this << std::endl;
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Export::decommision () {
    std::cerr << "VA::Node::Export::decommision: " << this << std::endl;
    return isolate ()->Detach (this) && BaseClass::decommision ();
}

/*********************
 *********************
 *****  Methods  *****
 *********************
 *********************/

/*************************
 *----  loopbackInt  ----*
 *************************/

void VA::Node::Export::loopbackInt (
    Vxa::VResultBuilder &rRB, int i
) {
    rRB = i;
}

/*************************
 *----  loopbackAny  ----*
 *************************/

namespace {
    class LoopbackSink : public Vxa::VAny::Client {
    public:
        LoopbackSink (Vxa::VResultBuilder &rRB) : m_rRB (rRB) {
        }
        ~LoopbackSink () {
        }
    private:
        template <typename value_t> void onImpl (value_t iValue) {
            m_rRB = iValue;
        }
    public:
        virtual void on (int iValue) override {
            onImpl (iValue);
        }
        virtual void on (double iValue) override {
            onImpl (iValue);
        }
        virtual void on (VString const &iValue) override {
            onImpl (iValue);
        }
    private:
        Vxa::VResultBuilder &m_rRB;
    };
}

void VA::Node::Export::loopbackAny (
    Vxa::VResultBuilder &rRB, Vxa::VAny::value_t rAny
) {
    LoopbackSink iAnySink (rRB);
    rAny.supply (iAnySink);
}

/*************************
 *----  interceptor  ----*
 *************************/

namespace {
    class InterceptorSink : public Vxa::VAny::Client {
    public:
        InterceptorSink (VString &rMessage) : m_rMessage (rMessage) {
        }
        ~InterceptorSink () {
        }
    private:
        template <typename value_t> void onImpl (value_t iValue) {
            m_rMessage << iValue;
        }
    public:
        virtual void on (int iValue) override {
            onImpl (iValue);
        }
        virtual void on (double iValue) override {
            onImpl (iValue);
        }
        virtual void on (VString const &iValue) override {
            onImpl (iValue);
        }
    private:
        VString &m_rMessage;
    };
}

void VA::Node::Export::interceptor (
    Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t rPack
) {
    VString iMessage;
    Vxa::VTask const *const pTask = rRB.task ();
    iMessage
	<< "Called as '"
	<< pTask->methodName ()
	<< "' with parameter count of "
	<< pTask->parameterCount ()
	<< " and task size of "
	<< pTask->cardinality ();

    InterceptorSink iAnySink (iMessage);
    Vxa::cardinality_t const cParameters = rPack.parameterCount ();
    for (Vxa::cardinality_t xParameter = 0; xParameter < cParameters; xParameter++) {
        iMessage << "\n " <<  xParameter << ": ";
        rPack.parameterValue (xParameter).supply (iAnySink);
    }
    iMessage << "\n";
    rRB = iMessage;
}

void VA::Node::Export::adder (Vxa::VResultBuilder &rRB, Vxa::VPack<double>::value_t pack_o_ds) {
    double result = 0.0;
    for (unsigned int xD = 0; xD < pack_o_ds.parameterCount (); xD++)
        result += pack_o_ds[xD];
    rRB = result;
}


/***************************
 *****  JS Operations  *****
 ***************************/

void VA::Node::Export::JSCallback (Vxa::VResultBuilder &rRB, Vxa::VPack<Vxa::VAny::value_t>::value_t) {
}

void VA::Node::Export::JSToString (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    VString iResult;
    GetString (iResult, object()->ToString (context ()));
    rRB = iResult;
}

void VA::Node::Export::JSToDetail (Vxa::VResultBuilder &rRB) {
    HandleScope iHS (this);
    VString iResult;
    GetString (iResult, object()->ToDetailString (context ()));
    rRB = iResult;
}


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Export::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Vxa::Object::ClassBuilder (pClass) {
    defineMethod ("loopbackInt:", &Export::loopbackInt);
    defineMethod ("loopbackAny:", &Export::loopbackAny);

    defineMethod ("add"		, &Export::adder);
    defineMethod ("add:"	, &Export::adder);
    defineMethod ("add:a:"	, &Export::adder);
    defineMethod ("add:a:a:"	, &Export::adder);
    defineMethod ("add:a:a:a:"	, &Export::adder);
    defineMethod ("add:a:a:a:a:", &Export::adder);

    defineDefault (&Export::interceptor);

    defineMethod (".toString"   , &Export::JSToString);
    defineMethod (".toDetail"   , &Export::JSToDetail);
}

namespace {
    Vxa::VCollectable<VA::Node::Export> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(VA::Node::Export);
