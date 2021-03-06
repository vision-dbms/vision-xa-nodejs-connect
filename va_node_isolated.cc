/*****  VA_Node_Isolated Implementation  *****/

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

#include "va_node_isolated.h"

/************************
 *****  Supporting  *****
 ************************/

#include "va_node_callback.h"

#include <iostream>


/********************************
 ********************************
 *****                      *****
 *****  VA::Node::Isolated  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::Node::Isolated::Isolated (
    Isolate *pIsolate
) : m_pIsolate (pIsolate) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::Node::Isolated::~Isolated () {
}

/*****************************
 *****************************
 *****  Decommissioning  *****
 *****************************
 *****************************/

bool VA::Node::Isolated::onDeleteThis () {
    return m_pIsolate->okToDecommission (this) && decommission ();
}

bool VA::Node::Isolated::decommission () {
    return true;
}

/***************************
 ***************************
 *****  Task Launcher  *****
 ***************************
 ***************************/

namespace {
    class TaskLauncher final : public VA::Node::Callback {
        DECLARE_CONCRETE_RTTLITE (TaskLauncher, Callback);

    //  Construction
    public:
        TaskLauncher (Vxa::VTask *pTask) : m_pTask (pTask) {
        }
    //  Destruction
    private:
        ~TaskLauncher () {
        }

    //  Execution
    private:
        virtual void run () override {
            m_pTask->runWithMonitor ();
        }


    //  State
    private:
        Vxa::VTask::Reference const m_pTask;
    };
}

bool VA::Node::Isolated::launchTask (Vxa::VTask *pTask) {
    TaskLauncher::Reference const pLauncher (new TaskLauncher (pTask));
    pLauncher->trigger ();
    return true;
}


/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::Node::Isolated::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : BaseClass::ClassBuilder (pClass) {
}
