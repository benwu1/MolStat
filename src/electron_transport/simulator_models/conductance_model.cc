/* This file is a part of MolStat, which is distributed under the Creative
   Commons Attribution-NonCommercial 4.0 International Public License.
   MolStat (c) 2014, Northwestern University. */
/**
 * \file conductance_model.cc
 * \brief Function for constructing a ConductanceModel from the name of the
 *    model and a list of named random number distributions.
 *
 * \author Matthew G.\ Reuter
 * \date May 2014
 */

#include "conductance_model.h"
#include "symmetric_one_site.h"
#include "asymmetric_one_site.h"
#include "symmetric_voltage_one_site.h"
#include "asymmetric_voltage_one_site.h"
#include "symmetric_two_site.h"
#include "asymmetric_two_site.h"
#include "symmetric_voltage_two_site.h"
#include "independent_voltage_two_site.h"

using namespace std;

shared_ptr<ConductanceModel> make_model(const std::string str,
	const std::map<std::string, shared_ptr<RandomDistribution>> &parameters) {

	// create the actual model and process the random number distributions
	if(str == "symmetriconesitemodel") {
		shared_ptr<RandomDistribution> dist_gamma, dist_eps;

		dist_gamma = find_distribution("gamma", parameters);
		dist_eps = find_distribution("epsilon", parameters);

		return make_shared<SymmetricOneSiteModel>(dist_eps, dist_gamma);
	}
	else if(str == "asymmetriconesitemodel") {
		shared_ptr<RandomDistribution> dist_gammaL, dist_gammaR, dist_eps;

		dist_gammaL = find_distribution("gammal", parameters);
		dist_gammaR = find_distribution("gammar", parameters);
		dist_eps = find_distribution("epsilon", parameters);

		return make_shared<AsymmetricOneSiteModel>(dist_eps, dist_gammaL,
			dist_gammaR);
	}
	else if(str == "symmetricvoltageonesitemodel") {
		shared_ptr<RandomDistribution> dist_gamma, dist_eps, dist_a;

		dist_gamma = find_distribution("gamma", parameters);
		dist_eps = find_distribution("epsilon", parameters);
		dist_a = find_distribution("a", parameters);

		return make_shared<SymmetricVoltageOneSiteModel>(dist_eps, dist_gamma,
			dist_a);
	}
	else if(str == "asymmetricvoltageonesitemodel") {
		shared_ptr<RandomDistribution> dist_gammal, dist_gammar, dist_eps, dist_a;

		dist_gammal = find_distribution("gammal", parameters);
		dist_gammar = find_distribution("gammar", parameters);
		dist_eps = find_distribution("epsilon", parameters);
		dist_a = find_distribution("a", parameters);

		return make_shared<AsymmetricVoltageOneSiteModel>(dist_eps, dist_gammal,
			dist_gammar, dist_a);
	}
	else if(str == "symmetrictwositemodel") {
		shared_ptr<RandomDistribution> dist_gamma, dist_eps, dist_beta;

		dist_gamma = find_distribution("gamma", parameters);
		dist_eps = find_distribution("epsilon", parameters);
		dist_beta = find_distribution("beta", parameters);

		return make_shared<SymmetricTwoSiteModel>(dist_eps, dist_gamma,
			dist_beta);
	}
	else if(str == "asymmetrictwositemodel") {
		shared_ptr<RandomDistribution> dist_gammaL, dist_gammaR, dist_eps,
			dist_beta;

		dist_gammaL = find_distribution("gammal", parameters);
		dist_gammaR = find_distribution("gammar", parameters);
		dist_eps = find_distribution("epsilon", parameters);
		dist_beta = find_distribution("beta", parameters);

		return make_shared<AsymmetricTwoSiteModel>(dist_eps, dist_gammaL,
			dist_gammaR, dist_beta);
	}
	else if(str == "symmetricvoltagetwositemodel") {
		shared_ptr<RandomDistribution> dist_gamma, dist_eps, dist_beta;

		dist_gamma = find_distribution("gamma", parameters);
		dist_eps = find_distribution("epsilon", parameters);
		dist_beta = find_distribution("beta", parameters);

		return make_shared<SymmetricVoltageTwoSiteModel>(dist_eps, dist_gamma,
			dist_beta);
	}
	else if(str == "independentvoltagetwositemodel") {
		shared_ptr<RandomDistribution> dist_eps1, dist_gamma1, dist_a1,
			dist_eps2, dist_gamma2, dist_a2;

		dist_eps1 = find_distribution("epsilon1", parameters);
		dist_gamma1 = find_distribution("gamma1", parameters);
		dist_a1 = find_distribution("a1", parameters);
		dist_eps2 = find_distribution("epsilon2", parameters);
		dist_gamma2 = find_distribution("gamma2", parameters);
		dist_a2 = find_distribution("a2", parameters);

		return make_shared<IndependentVoltageTwoSiteModel>(dist_eps1,
			dist_gamma1, dist_a1, dist_eps2, dist_gamma2, dist_a2);
	}
	else
		throw invalid_argument("Unrecognized model. Options are:\n" \
			"   SymmetricOneSiteModel - " \
				"Symmetric-Coupling, One-Site Model\n" \

			"   AsymmetricOneSiteModel - " \
				"Asymmetric-Coupling, One-Site Model\n" \

			"   SymmetricVoltageOneSiteModel - " \
				"Symmetric-Coupling, Voltage-Dependent One-Site Model\n" \

			"   AsymmetricVoltageOneSiteModel - " \
				"Asymmetric-Coupling, Voltage-Dependent One-Site Model\n" \

			"   SymmetricTwoSiteModel - " \
				"Symmetric-Coupling, Two-Site Model\n" \

			"   AsymmetricTwoSiteModel - " \
				"Asymmetric-Coupling, Two-Site Model\n" \

			"   SymmetricVoltageTwoSiteModel - " \
				"Symmetric-Coupling, Voltage-Dependent Two-Site Model\n" \

			"   IndependentVoltageTwoSiteModel - " \
				"Sum of two Symmetric-Coupling, Voltage-Dependent One-Site " \
				"Models\n");

	// should never be here
	throw invalid_argument("Shouldn't be here.");
	return nullptr;
}