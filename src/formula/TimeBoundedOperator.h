/*
 * TimeBoundedOperator.h
 *
 *  Created on: 10.04.2013
 *      Author: thomas
 */

#ifndef TIMEBOUNDEDOPERATOR_H_
#define TIMEBOUNDEDOPERATOR_H_

#include "AbstractPathFormula.h"
#include "exceptions/InvalidArgumentException.h"

namespace storm {
namespace formula {

/*!
 * @brief
 * Class for a Abstract formula tree with a operator node as root that uses a time interval
 * (with upper and lower bound)
 *
 * This class does not provide support for sub formulas; this has to be done in concretizations of this abstract class.
 *
 *
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see AbstractFormula
 */
template<class T>
class TimeBoundedOperator: public storm::formula::AbstractPathFormula<T> {
public:
	/**
	 * Constructor
	 *
	 * @param lowerBound The lower bound
	 * @param upperBound The upper bound
	 * @throw InvalidArgumentException if the lower bound is larger than the upper bound.
	 */
	TimeBoundedOperator(T lowerBound, T upperBound) {
		setInterval(lowerBound, upperBound);
	}

	/**
	 * Destructor
	 */
	virtual ~TimeBoundedOperator() {
		// Intentionally left empty
	}

	/**
	 * Getter for lowerBound attribute
	 *
	 * @return lower bound of the operator.
	 */
	T getLowerBound() const {
		return lowerBound;
	}

	/**
	 * Getter for upperBound attribute
	 * @return upper bound of the operator.
	 */
	T getUpperBound() const {
		return upperBound;
	}

	/**
	 * Set the time interval for the time bounded operator
	 *
	 * @param lowerBound
	 * @param upperBound
	 * @throw InvalidArgumentException if the lower bound is larger than the upper bound.
	 */
	void setInterval(T lowerBound, T upperBound) {
		if (lowerBound > upperBound) {
			throw new storm::exceptions::InvalidArgumentException("Lower bound is larger than upper bound");
		}
		this->lowerBound = lowerBound;
		this->upperBound = upperBound;
	}


	/*!
	 * @returns a string representation of the Interval of the formula
	 * 			May be used in subclasses to simplify string output.
	 */
	virtual std::string toString() const {
		std::string result = "[";
		result += std::to_string(lowerBound);
		result += ";";
		result += "]";
		return result;
	}

private:
	T lowerBound, upperBound;
};

} /* namespace formula */
} /* namespace storm */
#endif /* TIMEBOUNDEDOPERATOR_H_ */
