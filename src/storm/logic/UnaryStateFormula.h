#ifndef STORM_LOGIC_UNARYSTATEFORMULA_H_
#define STORM_LOGIC_UNARYSTATEFORMULA_H_

#include "storm/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class UnaryStateFormula : public StateFormula {
        public:
            UnaryStateFormula(std::shared_ptr<Formula const> subformula);
            
            virtual ~UnaryStateFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isUnaryStateFormula() const override;
            
            Formula const& getSubformula() const;
            
            virtual void gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const override;
            virtual void gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const override;
            virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;

        private:
            std::shared_ptr<Formula const> subformula;
        };
    }
}

#endif /* STORM_LOGIC_UNARYSTATEFORMULA_H_ */
