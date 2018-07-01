#ifndef Vision_Evaluation_Gofer_Interface
#define Vision_Evaluation_Gofer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInstance.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vision_Evaluation_Value.h"
#include "Vsa_IEvaluator.h"
#include "Vxa_ICollection.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vision {
    namespace Evaluation {
        typedef Vxa::ICollection export_t;

        typedef Vca::VGoferInterface<Vsa::IEvaluator>  evaluator_gofer_t;
        typedef Vca::VGoferInstance<Value>             evaluation_result_gofer_t;
        typedef Vca::VGoferInterface<export_t>         export_gofer_t;

        class Gofer : public Vca::VGoferInstance<Value> {
            DECLARE_CONCRETE_RTTLITE (Gofer, evaluation_result_gofer_t);

        //  class EvaluatorClient
        public:
            class EvaluatorClient;

        //  Construction
        public:
            template <typename evaluator_t, typename expression_t, typename export_t> Gofer (
                evaluator_t pEvaluator, expression_t iExpression, export_t pExport
            ) : m_pEvaluator (pEvaluator), m_iExpression (iExpression), m_pExport (pExport) {
            }
        //  Destruction
        private:
            ~Gofer () {
            };

        //  Callbacks and Triggers
        private:
            void onData () override;
            void onNeed () override;

        //  State
        private:
            Vca::VInstanceOfInterface<Vsa::IEvaluator> m_pEvaluator;
            Vca::VInstanceOf_String                    m_iExpression;
            Vca::VInstanceOfInterface<export_t>        m_pExport;
        };
    } // namespace Evaluation
} // namespace Vision

#endif
