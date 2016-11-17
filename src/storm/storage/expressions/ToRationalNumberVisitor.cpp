#include "storm/storage/expressions/ToRationalNumberVisitor.h"

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace expressions {
        template<typename RationalNumberType>
        ToRationalNumberVisitor<RationalNumberType>::ToRationalNumberVisitor() : ExpressionVisitor() {
            // Intentionally left empty.
        }
        
        template<typename RationalNumberType>
        RationalNumberType ToRationalNumberVisitor<RationalNumberType>::toRationalNumber(Expression const& expression) {
            return boost::any_cast<RationalNumberType>(expression.accept(*this, boost::none));
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(IfThenElseExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            RationalNumberType firstOperandAsRationalNumber = boost::any_cast<RationalNumberType>(expression.getFirstOperand()->accept(*this, data));
            RationalNumberType secondOperandAsRationalNumber = boost::any_cast<RationalNumberType>(expression.getSecondOperand()->accept(*this, data));
            switch(expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    return firstOperandAsRationalNumber + secondOperandAsRationalNumber;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    return firstOperandAsRationalNumber - secondOperandAsRationalNumber;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    return firstOperandAsRationalNumber * secondOperandAsRationalNumber;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    return firstOperandAsRationalNumber / secondOperandAsRationalNumber;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Min:
                    return std::min(firstOperandAsRationalNumber, secondOperandAsRationalNumber);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Max:
                    return std::max(firstOperandAsRationalNumber, secondOperandAsRationalNumber);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    STORM_LOG_THROW(storm::utility::isInteger(secondOperandAsRationalNumber), storm::exceptions::InvalidArgumentException, "Exponent of power operator must be a positive integer.");
                    uint_fast64_t exponentAsInteger = storm::utility::convertNumber<uint_fast64_t>(secondOperandAsRationalNumber);
                    return storm::utility::pow(firstOperandAsRationalNumber, exponentAsInteger);
                    break;
            }
            
            // Return a dummy. This point must, however, never be reached.
            STORM_LOG_ASSERT(false, "Illegal operator type.");
            return boost::any();
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BinaryRelationExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(VariableExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot transform expressions containing variables to a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BooleanLiteralExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(IntegerLiteralExpression const& expression, boost::any const& data) {
#ifdef STORM_HAVE_CARL
            return RationalNumberType(carl::rationalize<storm::RationalNumber>(static_cast<size_t>(expression.getValue())));
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Rational numbers are not supported in this build.");
#endif
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(RationalLiteralExpression const& expression, boost::any const& data) {
#ifdef STORM_HAVE_CARL
            return expression.getValue();
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Rational numbers are not supported in this build.");
#endif
        }
        
#ifdef STORM_HAVE_CARL
        template class ToRationalNumberVisitor<storm::RationalNumber>;
#endif
    }
}
