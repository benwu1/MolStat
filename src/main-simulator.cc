/**
 * \file main-simulator.cc
 * \brief Main function for simulating conductance data using Landauer theory.
 *
 * This code reads in various input parameters from standard in, and uses these
 * parameters to simulate conductance data. The conductance data is designed to
 * be binned into conductance histograms. Both zero-bias (1D) and voltage-
 * dependent (2D) conductance data/histograms can be simulated.
 *
 * The expected input has the following structure, detailed by line number.
 *    -# The model to use for simulating conductance data. Possibilities are
 *       - SymmetricVoltageIndependentModel
 *       - AsymmetricVoltageIndependentModel
 *       - SymmetricVoltageOneSiteModel
 *
 *       Details on this models can be found elsewhere in the documentation.
 *    -# Type of conductance to calculate. This can be
 *       - Differential
 *       - Static
 *       - ZeroBias
 *
 *       Static and differential conductances are voltage-dependent, the
 *       ZeroBias option calculates the zero-bias differential conductance.
 *    -# The number of conductance data points to simulate. One will be output
 *       per line.
 *    -# The Fermi energy of the system.
 *    -# (Remaining lines): Probability distributions for any necessary
 *       parameters (one per line). Each line will be
 *
 *       name distribution [distribution-parameters]
 *
 *       More information can be found elsewhere in the documentation. If
 *       voltage-dependent histograms are requested, distributions must be
 *       specified for eta (the relative voltage drop at the two leads)
 *       and for V (the voltage). Each model will have certain required
 *       parameters.
 *
 * \author Matthew G.\ Reuter
 * \date April 2014
 */

#include <memory>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include "string_tools.h"
#include "aux_simulator/rng.h"
#include "aux_simulator/symmetric_voltage_independent.h"
#include "aux_simulator/asymmetric_voltage_independent.h"
#include "aux_simulator/symmetric_voltage_one_site.h"

using namespace std;

/**
 * \brief Enum for the types of conductance calculations we have.
 */
enum class CalculationType {
	Static, // Voltage-Dependent Static Conductance
	Differential, // Voltage-Dependend Differential Conductance
	ZeroBias // Zero-Bias Differential Conductance
};

/**
 * \brief Takes the list of distributions from the input deck and constructs
 *    a model of the specified type.
 *
 * \throw invalid_argument if any required distributions are not specified.
 *
 * \param[in] str The name of the type, assumed to be lowercase.
 * \param[in] parameters Map of distribution names (lowercase) to the
 *    distributions.
 * \return A shared pointer to the model object.
 */
shared_ptr<ConductanceModel> set_distributions(const string str,
	const map<string, shared_ptr<RandomDistribution>> &parameters);

/**
 * \brief Main function for simulating a histogram.
 *
 * Parses the input parameters and outputs randomly-generated conductance
 * data.
 *
 * \param[in] argc The number of command-line arguments.
 * \param[in] argv The command-line arguments.
 * \return Exit status; 0 for normal.
 */
int main(int argc, char **argv) {

	// initialize the GSL random number generator
	gsl_rng_env_setup();
	shared_ptr<gsl_rng> r(gsl_rng_alloc(gsl_rng_default), &gsl_rng_free);
	gsl_rng_set(r.get(), 0xFEEDFACE);

	CalculationType type;
	int i, n;
	double EF;
	map<string, shared_ptr<RandomDistribution>> parameters;
	shared_ptr<ConductanceModel> model;
	shared_ptr<RandomDistribution> dist_V, dist_eta;
	function<void(void)> conductance_function;

	string line, modeltype, name;
	vector<string> tokens;

	// setup the simulation -- read in parameters from stdin
	// Line 1: One token specifying the model to use
	try {
		line = getline(stdin);
	}
	catch(const runtime_error &e) {
		fprintf(stderr, "Error: %s.\n", e.what());
		return 0;
	}

	tokenize(line, tokens);
	if(tokens.size() < 1) {
		fprintf(stderr, "Error: model name expected in line 1.\n");
		return 0;
	}

	// make a link to the correct model creator (need to continue reading data,
	// store for later)
	modeltype = tokens[0];
	make_lower(modeltype);

	// Line 2: Type of conductance to calculate
	try {
		line = getline(stdin);
	}
	catch(const runtime_error &e) {
		fprintf(stderr, "Error: %s.\n", e.what());
		return 0;
	}
	tokenize(line, tokens);
	if(tokens.size() < 1) {
		fprintf(stderr, "Error: conductance type expected in line 2.\n");
		return 0;
	}
	make_lower(tokens[0]);
	if(tokens[0] == "static")
		type = CalculationType::Static;
	else if(tokens[0] == "differential")
		type = CalculationType::Differential;
	else if(tokens[0] == "zerobias")
		type = CalculationType::ZeroBias;
	else {
		fprintf(stderr, "Error: Unrecognized conductance type in line 2.\n   " \
			"It must be \"Static\", \"Differential\", or \"ZeroBias\".\n");
		return 0;
	}

	// Line 3: number of trials
	try {
		line = getline(stdin);
	}
	catch(const runtime_error &e) {
		fprintf(stderr, "Error: %s.\n", e.what());
		return 0;
	}
	tokenize(line, tokens);
	if(tokens.size() < 1) {
		fprintf(stderr, "Error: number of trials expected in line 3.\n");
		return 0;
	}
	try {
		n = stoi(tokens[0]);
	}
	catch(const invalid_argument &e) {
		fprintf(stderr, "Error: unrecognizable number '%s'.\n",
			tokens[0].c_str());
		return 0;
	}

	// Line 4: Fermi level
	try {
		line = getline(stdin);
	}
	catch(const runtime_error &e) {
		fprintf(stderr, "Error: %s.\n", e.what());
		return 0;
	}
	tokenize(line, tokens);
	if(tokens.size() < 1) {
		fprintf(stderr, "Error Fermi energy expected in line 4.\n");
		return 0;
	}
	try {
		EF = stod(tokens[0]);
	}
	catch(const invalid_argument &e) {
		fprintf(stderr, "Error: unable to parse %s to the Fermi energy.\n",
			tokens[0].c_str());
		return 0;
	}

	// all subsequent lines specify random number distributions
	// EOF is flagged by a runtime_error in the getline function
	try {
		while(true) {
			line = getline(stdin);

			tokenize(line, tokens);
			if(tokens.size() > 0) {
				name = tokens[0];
				make_lower(name);

				// need to remove the front entry for the distribution generator
				tokens.erase(tokens.begin());

				// create a random distribution from the contents of the line
				try {
					parameters[name] = distribution_from_tokens(tokens);
				}
				catch(const invalid_argument &e) {
					fprintf(stderr, "Error: unable to form a random number " \
						"distribution from:\n   %s\n%s\n", line.c_str(), e.what());
				}
			}
		}
	}
	catch(const runtime_error &e) {
	}

	// set the distributions required by the model
	try {
		model = set_distributions(modeltype, parameters);
	}
	catch(const invalid_argument &e) {
		fprintf(stderr, "Error initializing the model: %s\n", e.what());
		return 0;
	}

	// set up the run function for each type...
	// this may require other distributions to be set
	if(type == CalculationType::Static ||
		type == CalculationType::Differential) {

		// make sure there are distributions for V and eta
		try {
			dist_eta = parameters.at("eta");
		}
		catch(const out_of_range &e) {
			fprintf(stderr, "Error: a distribution for \"eta\" must be " \
				"specified.\n");
			return 0;
		}
		try {
			dist_V = parameters.at("v");
		}
		catch(const out_of_range &e) {
			fprintf(stderr, "Error: a distribution for \"V\" must be specified." \
				"\n");
			return 0;
		}

		// set the conductance function
		if(type == CalculationType::Static) {
			conductance_function = [=] () {
				double V = dist_V->sample(r);
				double GV = model->static_conductance(r, EF, dist_eta->sample(r),
					V);
				printf("%.6f %.6f\n", V, GV);
			};
		}
		else if(type == CalculationType::Differential) {
			conductance_function = [=] () {
				double V = dist_V->sample(r);
				double GV = model->diff_conductance(r, EF, dist_eta->sample(r), V);
				printf("%.6f %.6f\n", V, GV);
			};
		}
	}
	else if(type == CalculationType::ZeroBias) {
		// no extra distributions required... proceed to the calculation
		conductance_function = [=] () {
			printf("%.6f\n", model->zero_bias_conductance(r, EF));
		};
	}
	else {
		// should never be here
		fprintf(stderr, "An unknown error has occured.\n");
		return 0;
	}

	// Get the requested number of voltage-conductance data points
	for (i = 0; i < n; ++i) {
		conductance_function();
	}

	return 0;
}

shared_ptr<ConductanceModel> set_distributions(const string str,
	const map<string, shared_ptr<RandomDistribution>> &parameters) {

	// create the actual model and process the random number distributions
	if(str == "symmetricvoltageindependentmodel") {
		shared_ptr<RandomDistribution> dist_gamma, dist_eps;

		// populate the gamma and epsilon distributions
		try {
			dist_gamma = parameters.at("gamma");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"gamma\" must be " \
				"specified.");
		}

		try {
			dist_eps = parameters.at("epsilon");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"epsilon\" must be " \
				"specified.");
		}

		return make_shared<SymmetricVoltageIndependentModel>(dist_eps,
			dist_gamma);
	}
	else if(str == "symmetricvoltageonesitemodel") {
		shared_ptr<RandomDistribution> dist_gamma, dist_eps;

		// populate the gamma and epsilon distributions
		try {
			dist_gamma = parameters.at("gamma");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"gamma\" must be " \
				"specified.");
		}

		try {
			dist_eps = parameters.at("epsilon");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"epsilon\" must be " \
				"specified.");
		}

		return make_shared<SymmetricVoltageOneSiteModel>(dist_eps, dist_gamma);
	}
	else if(str == "asymmetricvoltageindependentmodel") {
		shared_ptr<RandomDistribution> dist_gammaL, dist_gammaR, dist_eps;

		// populate the gamma and epsilon distributions
		try {
			dist_gammaL = parameters.at("gammal");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"gammaL\" must be " \
				"specified.");
		}

		try {
			dist_gammaR = parameters.at("gammar");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"gammaR\" must be " \
				"specified.");
		}

		try {
			dist_eps = parameters.at("epsilon");
		}
		catch(const out_of_range &e) {
			throw invalid_argument("A distribution for \"epsilon\" must be " \
				"specified.");
		}

		return make_shared<AsymmetricVoltageIndependentModel>(dist_eps,
			dist_gammaL, dist_gammaR);
	}
	else
		throw invalid_argument("Unrecognized model. Options are:\n" \
			"   SymmetricVoltageIndependentModel - " \
				"Symmetric-Coupling, Voltage-Independent Transmission\n" \
			"   SymmetricVoltageOneSiteModel - " \
				"Symmetric-Coupling, Voltage-Dependent One-Site Model\n" \
			"   AsymmetricVoltageIndependentModel - " \
				"Asymmetric-Coupling, Voltage-Independent Transmission\n");

	// should never be here
	throw invalid_argument("Shouldn't be here.");
	return nullptr;
}

#if 0
// Double-site, voltage-dependent model
static double transmission_d(double V, double gamma, double epsilon,
	double beta, double E) {

	double temp = 4.*(E-epsilon)*(E-epsilon) - 4.*beta*beta - gamma*gamma - V*V;

	return 16.*gamma*gamma*beta*beta / (temp*temp +
		16.*gamma*gamma*(E-epsilon)*(E-epsilon));
}

static double dtdvint_d(double V, double gamma, double beta, double z) {
	const double bv = 4.*beta*beta + V*V;
	const double bvg = bv + gamma*gamma;
	const std::complex<double> arctan = atan(2.*z /
		std::complex<double>(gamma, -sqrt(bv)));

	return 8.*V*gamma*gamma*beta*beta*z*(4.*z*z + gamma*gamma - 3.*bv) /
		(bv*bvg*(16.*z*z*z*z + 8.*(gamma*gamma - bv)*z*z + bvg*bvg))

		- 8.*V*gamma*beta*beta / (bvg*bvg) * std::real(arctan)

		- 4.*V*gamma*gamma*beta*beta*(3.*bv + gamma*gamma) /
			(bvg*bvg*bv*sqrt(bv)) * std::imag(arctan);
}

double diff_conductance_d(double V, double gamma, double epsilon, double eta,
	double EF) {

	const double beta = -3.0;

	return eta*transmission_d(V, gamma, epsilon, beta, EF + eta*V) +
		(1.-eta)*transmission_d(V, gamma, epsilon, beta, EF + (eta-1.)*V) +
		dtdvint_d(V, gamma, beta, EF - epsilon + eta*V) -
		dtdvint_d(V, gamma, beta, EF - epsilon + (eta-1.)*V);
}

static double tint_d(double V, double gamma, double beta, double z) {
	const std::complex<double> arg(gamma, -sqrt(4.*beta*beta + V*V));

	return 4.*beta*beta*gamma/(V * sqrt(4.*beta*beta+V*V)) *
		std::imag(atan(2.*z/arg)/arg);
}

double static_conductance_d(double V, double gamma, double epsilon, double eta,
	double EF) {

	const double beta = -3.0;

	return tint_d(V, gamma, beta, EF - epsilon + eta*V)
		- tint_d(V, gamma, beta, EF - epsilon + (eta - 1.)*V);
}
#endif