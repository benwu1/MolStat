/**
 * \file uniform.h
 * \brief Interface for the uniform distribution.
 *
 * \author Matthew G.\ Reuter
 * \date May 2014
 */

#ifndef __uniform_h__
#define __uniform_h__

#include <memory>
#include <vector>
#include <string>
#include <gsl/gsl_rng.h>
#include "rng.h"

using std::shared_ptr;

/**
 * \brief Uniform distribution.
 */
class UniformDistribution : public RandomDistribution {
protected:
	/**
	 * \brief The lower bound of possible values.
	 */
	double lower;

	/**
	 * \brief The upper bound of possible values.
	 */
	double upper;

public:
	UniformDistribution() = delete;

	/**
	 * \brief Constructor specifying the range.
	 *
	 * \param[in] low The lower bound.
	 * \param[in] up The upper bound.
	 */
	UniformDistribution(const double low, const double up);

	/**
	 * \brief Destructor.
	 */
	~UniformDistribution() = default;

	/**
	 * \brief Samples from the uniform distribution.
	 *
	 * \param[in] r The handle for GSL random number generation.
	 * \return The random number.
	 */
	virtual double sample(shared_ptr<gsl_rng> r) const;
};

#endif