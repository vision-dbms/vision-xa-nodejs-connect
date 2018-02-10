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
    Isolate *pIsolate, handle_t hObject
) : m_pIsolate (pIsolate), m_hObject (pIsolate->isolate (), hObject) {
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

bool VA::Node::Export::onDeleteThis () {
//    return m_pIsolate->Detach (this);
    std::cerr << "VA::Node::Export::onDeleteThis: " << this << std::endl;
    return false;
}

/*********************
 *********************
 *****  Methods  *****
 *********************
 *********************/

void VA::Node::Export::loopbackInt (
    Vxa::VResultBuilder &rRB, int i
) {
    rRB = i;
}

namespace {
    class LBAnyClient : public Vxa::VAny::Client {
    public:
        LBAnyClient (Vxa::VResultBuilder &rRB) : m_rRB (rRB) {
        }
        ~LBAnyClient () {
        }
    private:
        template <typename value_t> void onImpl (value_t iValue) {
            m_rRB = iValue;
        }
    public:
        virtual void on (int iValue) OVERRIDE {
            onImpl (iValue);
        }
        virtual void on (double iValue) OVERRIDE {
            onImpl (iValue);
        }
        virtual void on (VString const &iValue) OVERRIDE {
            onImpl (iValue);
        }
    private:
        Vxa::VResultBuilder &m_rRB;
    };
}

void VA::Node::Export::loopbackAny (
    Vxa::VResultBuilder &rRB, Vxa::VAny const &rAny
) {
    LBAnyClient iAnyClient (rRB);
    rAny.supply (iAnyClient);
}

void VA::Node::Export::defthod (Vxa::VResultBuilder &rRB) {
    VString iMessage;
    Vxa::VTask const *const pTask = rRB.task ();
    iMessage
	<< "Called as '"
	<< pTask->methodName ()
	<< "' with parameter count of "
	<< pTask->parameterCount ()
	<< " and task size of "
	<< pTask->cardinality ()
	<< "\n";
    rRB = iMessage;
}


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Export::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Vxa::Object::ClassBuilder (pClass) {
    defineMethod ("loopbackInt:", &Export::loopbackInt);
    defineMethod ("loopbackAny:", &Export::loopbackAny);

    defineDefault (&Export::defthod);
}

namespace {
    Vxa::VCollectable<VA::Node::Export> g_iMeta;
}

DEFINE_VXA_COLLECTABLE(VA::Node::Export);
