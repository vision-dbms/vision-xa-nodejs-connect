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

/****************************
 ****************************
 *****  Decommisioning  *****
 ****************************
 ****************************/

bool VA::Node::Isolated::onDeleteThis () {
    return m_pIsolate->okToDecommision (this) && decommision ();
}

bool VA::Node::Isolated::decommision () {
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
            std::cerr << "{}::TaskLauncher[" << this << "]::TaskLauncher: "
                      << pTask << std::endl;
        }
    //  Destruction
    private:
        ~TaskLauncher () {
            std::cerr << "{}::TaskLauncher[" << this << "]::~TaskLauncher: "
                      << m_pTask.referent () << std::endl;
        }

    //  Execution
    private:
        virtual void run () override {
            std::cerr << "{}::TaskLauncher[" << this << "]::run: "
                      << m_pTask.referent () << ": before: "
                      << m_pTask->selectorName ()
                      << std::endl;
            m_pTask->runWithMonitor ();
            std::cerr << "{}::TaskLauncher[" << this << "]::run: "
                      << m_pTask.referent () << ": after : "
                      << m_pTask->selectorName ()
                      << std::endl;
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
