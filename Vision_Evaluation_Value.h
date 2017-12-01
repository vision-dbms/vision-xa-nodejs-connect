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
            Value (IEvaluationResult *pResult, VString const &rOutput) : m_pResult (pResult), m_iOutput (rOutput) {
            }
            Value (ThisClass const &rOther) {
                setTo (rOther);
            }
            Value () {
            }
        public:
            ~Value () {
            }

        //  Query
        public:
            bool operator== (ThisClass const &rOther) const {
                return m_pResult == rOther.m_pResult && m_iOutput == rOther.m_iOutput;
            }

        //  Update
        public:
            void setTo (ThisClass const &rOther) {
                m_pResult.setTo (rOther.m_pResult);
                m_iOutput.setTo (rOther.m_iOutput);
            }

            ThisClass& operator=(ThisClass const &rOther) {
                setTo (rOther);
                return *this;
            }

        //  State
        private:
            Vsa::IEvaluationResult::Reference m_pResult;
            VString                           m_iOutput;
        };
    } // namespace Evaluation
} // namespace Vision

#endif
