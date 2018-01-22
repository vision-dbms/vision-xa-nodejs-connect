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
        typedef Vxa::ICollection context_t;

        typedef Vca::VGoferInterface<Vsa::IEvaluator>  evaluator_gofer_t;
        typedef Vca::VGoferInstance<Value>             evaluation_result_gofer_t;
        typedef Vca::VGoferInterface<context_t>        context_gofer_t;

        class Gofer : public Vca::VGoferInstance<Value> {
            DECLARE_CONCRETE_RTTLITE (Gofer, evaluation_result_gofer_t);

        //  class EvaluatorClient
        public:
            class EvaluatorClient;

        //  Construction
        public:
            template <typename evaluator_t, typename expression_t, typename context_t> Gofer (
                evaluator_t iEvaluator, expression_t iExpression, context_t iContext
            ) : m_iEvaluator (iEvaluator), m_iExpression (iExpression), m_iContext (iContext) {
            }
        //  Destruction
        private:
            ~Gofer () {
            };

        //  Callbacks and Triggers
        private:
            void onData ();
            void onNeed ();

        //  State
        private:
            Vca::VInstanceOfInterface<Vsa::IEvaluator> m_iEvaluator;
            Vca::VInstanceOf_String                    m_iExpression;
            Vca::VInstanceOfInterface<Vxa::ICollection>m_iContext;
        };
    } // namespace Evaluation
} // namespace Vision

#endif
