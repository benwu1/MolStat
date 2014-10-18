/* This file is a part of MolStat, which is distributed under the Creative
   Commons Attribution-NonCommercial 4.0 International Public License.
   MolStat (c) 2014, Northwestern University. */
/**
 * \file simulator.cc
 * \brief Implements the molstat::Simulator class for simulating histograms.
 *
 * \author Matthew G.\ Reuter
 * \date October 2014
 */

#include "simulator.h"
#include "simulate_model.h"
#include "simulator_exceptions.h"

namespace molstat {

Simulator::Simulator(std::shared_ptr<SimulateModel> &model_)
	: model(model_), obs_functions() {
}

std::valarray<double> Simulator::simulate(gsl_rng_ptr &r) const {
	std::size_t num_obs{ obs_functions.size() };

	if(num_obs == 0)
		throw molstat::NoObservables();

	std::valarray<double> ret( num_obs );

	// get some parameters
	const std::valarray<double> params{ model->generateParameters(r) };

	// calculate each of the observables
	for(std::size_t j = 0; j < num_obs; ++j)
		ret[j] = obs_functions[j](params);

	return ret;
}

void Simulator::setObservable(std::size_t j, const std::type_index &obs) {
	std::size_t length { obs_functions.size() };

	if(j > length)
		throw std::out_of_range("Observable index is out of range.");

	// getObservableFunction will throw IncompatibleObservable if this doesn't
	// work... let it pass upwards.
	ObservableFunction func { model->getObservableFunction(obs) };

	if(j < length)
		obs_functions[j] = func;
	else
		obs_functions.push_back(func);
}

} // namespace MolStat