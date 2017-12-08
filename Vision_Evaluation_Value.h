#ifndef Vision_Evaluation_Value_Interface
#define Vision_Evaluation_Value_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_IEvaluationResult.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vision {
    namespace Evaluation {
        class Value {
            DECLARE_NUCLEAR_FAMILY (Value);

        //  Aliases
        public:
            typedef Vsa::IEvaluationResult IEvaluationResult;

        //  Construction / Destruction
        public:
            Value (IEvaluationResult *pObject, VString const &rOutput) : m_pObject (pObject), m_iOutput (rOutput) {
            }
            Value (ThisClass const &rOther) {
                setTo (rOther);
            }
            Value () {
            }
        public:
            ~Value () {
            }

        //  Access
        public:
            IEvaluationResult *object () const {
                return m_pObject;
            }
            VString const &output () const {
                return m_iOutput;
            }

        //  Query
        public:
            bool operator== (ThisClass const &rOther) const {
                return m_pObject == rOther.m_pObject && m_iOutput == rOther.m_iOutput;
            }

        //  Update
        public:
            void setTo (ThisClass const &rOther) {
                m_pObject.setTo (rOther.m_pObject);
                m_iOutput.setTo (rOther.m_iOutput);
            }

            ThisClass& operator=(ThisClass const &rOther) {
                setTo (rOther);
                return *this;
            }

        //  State
        private:
            IEvaluationResult::Reference m_pObject;
            VString                      m_iOutput;
        };
    } // namespace Evaluation
} // namespace Vision

/**********************
 *****  TypeInfo  *****
 **********************/

#ifndef Vision_Evaluation_Value
#define Vision_Evaluation_Value extern
#endif

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vision_Evaluation_Value_Instantiations)

Vision_Evaluation_Value template class Vca::VTypeInfoHolder<Vision::Evaluation::Value>;
Vision_Evaluation_Value template class Vca::VTypeInfoHolder<Vision::Evaluation::Value const&>;

#endif

#endif
