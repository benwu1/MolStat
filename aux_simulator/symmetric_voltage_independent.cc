/**
 * \file voltage_independent.cc
 * \brief Implementation of the voltage-independent tight-binding model for
 *        calculating conductances.
 *
 * \author Matthew G.\ Reuter
 * \date April 2014
 */

#include "symmetric_voltage_independent.h"
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include "../string_tools.h"

using namespace std;

double SymmetricVoltageIndependentModel::static_conductance(
	shared_ptr<gsl_rng> r, const double EF, const double eta,
	const double V) const {

	// get model parameters from the random distributions
	double eps = dist_eps->sample(r);
	double gamma = dist_gamma->sample(r);

	return static_conductance(EF, V, eta, eps, gamma);
}

double SymmetricVoltageIndependentModel::diff_conductance(
	shared_ptr<gsl_rng> r, const double EF, const double eta,
	const double V) const {

	// get model parameters from the random distributions
	double eps = dist_eps->sample(r);
	double gamma = dist_gamma->sample(r);

	return diff_conductance(EF, V, eta, eps, gamma);
}

double SymmetricVoltageIndependentModel::transmission(const double E,
	const double eps, const double gamma) {

	return gamma*gamma / ((E-eps)*(E-eps) + gamma*gamma);
}

double SymmetricVoltageIndependentModel::static_conductance(const double EF,
	const double V, const double eta, const double eps, const double gamma) {

	return gamma / V
		* (atan((EF-eps+eta*V) / gamma) - atan((EF-eps+(eta-1.)*V) / gamma));
}

double SymmetricVoltageIndependentModel::diff_conductance(const double EF,
	const double V, const double eta, const double eps, const double gamma) {

	return eta * transmission(EF + eta*V, eps, gamma) +
		(1.-eta) * transmission(EF + (eta-1.)*V, eps, gamma);
}
