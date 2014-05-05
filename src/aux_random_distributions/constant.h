/**
 * \file constant.h
 * \brief Interface for the constant distribution.
 *
 * \author Matthew G.\ Reuter
 * \date May 2014
 */

#ifndef __constant_h__
#define __constant_h__

#include <memory>
#include <vector>
#include <string>
#include <gsl/gsl_rng.h>
#include "rng.h"

using std::shared_ptr;

/**
 * \brief Constant distribution.
 *
 * This distribution is included for the case where the value should be fixed.
 */
class ConstantDistribution : public RandomDistribution {
protected:
	/**
	 * \brief The fixed value to return.
	 */
	double value;

public:
	ConstantDistribution() = delete;

	/**
	 * \brief Constructor specifying the fixed value.
	 *
	 * \param[in] val The fixed value.
	 */
	ConstantDistribution(const double val);

	/**
	 * \brief Destructor.
	 */
	~ConstantDistribution() = default;

	/**
	 * \brief Samples from the constant distribution (returns value).
	 *
	 * \param[in] r The handle for GSL random number generation.
	 * \return The random number.
	 */
	virtual double sample(shared_ptr<gsl_rng> r) const;
};

#endif