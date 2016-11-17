#ifndef STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_
#define STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_

#include <boost/variant.hpp>

#include "storm/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class CumulativeRewardFormula : public PathFormula {
        public:
            CumulativeRewardFormula(uint_fast64_t timeBound);
            
            CumulativeRewardFormula(double timeBound);
            
            virtual ~CumulativeRewardFormula() {
                // Intentionally left empty.
            }

            virtual bool isCumulativeRewardFormula() const override;
            virtual bool isRewardPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            bool hasDiscreteTimeBound() const;
            
            uint_fast64_t getDiscreteTimeBound() const;
            
            bool hasContinuousTimeBound() const;
            
            double getContinuousTimeBound() const;
            
        private:
            boost::variant<uint_fast64_t, double> timeBound;
        };
    }
}

#endif /* STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_ */
