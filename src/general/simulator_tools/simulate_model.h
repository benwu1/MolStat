/* This file is a part of MolStat, which is distributed under the Creative
   Commons Attribution-NonCommercial 4.0 International Public License.

   (c) 2014 Northwestern University. */

/**
 * \file simulate_model.h
 * \brief Defines the molstat::SimulateModel class for simulating histograms,
 *    the molstat::CompositeSimulateModel class for more complicated models,
 *    the molstat::SimulateModelFactory class for creating models at runtime,
 *    and other auxiliary types/aliases for the simulator interface.
 *
 * These classes work closely with molstat::Simulator for simulating
 * single-molecule behavior. Here is a rundown of the various classes in this
 * file, and how they work together.
 *
 * molstat::SimulateModel -- the main/base class for a model system. As
 * described in \ref sec_add_simulate, this class specifies the names of the
 * model's physical parameters and provides the interface for simulating data.
 * Any model deriving from this class \b must use virtual inheritance.
 *
 * molstat::CompositeSimulateModel -- the base class for a composite model
 * system (i.e., a model that is built upon some number of submodels). A
 * submodel type must be specified (see \ref sec_add_simulate), as well as a
 * mechanism for combining the observables together.
 * 
 * molstat::SimulateModelFactory -- a factory class for constructing a
 * molstat::SimulateModel. By separating out construction from usage, we can
 * provide better error messages/diagnostics during construction and spend less
 * time checking the internal state of the simulator model.
 *
 * \author Matthew G.\ Reuter
 * \date October 2014
 */

#ifndef __simulate_model_h__
#define __simulate_model_h__

#include <memory>
#include <functional>
#include <valarray>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <list>
#include <typeinfo>
#include <typeindex>
#include <general/random_distributions/rng.h>
#include <general/string_tools.h>

namespace molstat {

// forward declarations
class SimulateModel;
class RandomDistribution;

/**
 * \brief The signature of a function that calculates an observable.
 *
 * A molstat::ObservableFunction may throw molstat::NoObservableProduced if
 * the specified set of model parameters does not result in the observable.
 */
using ObservableFunction =
	std::function<double(const std::valarray<double> &)>;

/**
 * \brief The signature of a function that produces an ObservableFunction,
 *    given the model.
 *
 * \throw molstat::IncompatibleObservable may be thrown if the observable
 *    and model are incompatible.
 */
using ObservableFactory =
	std::function<ObservableFunction(std::shared_ptr<const SimulateModel>)>;

/**
 * \brief Alias for the index type (alias for std::type_index) of an
 *    Observable.
 */
using ObservableIndex = std::type_index;

/**
 * \brief Alias for the model type.
 *
 * Full models should return the type_index of molstat::SimulateModel. When
 * a more complicated model is needed, perhaps one derived from
 * molstat::CompositeModel, the underlying models may need to be a specific
 * type, and this allows runtime checking of the issue.
 */
using SimulateModelType = std::type_index;

/**
 * \brief Base class for a model that uses model parameters to calculate
 *    observables.
 *
 * All models for simulating data should derive (\b virtual inheritance) from
 * this class. Derived classes must implement functions that provide the number
 * of model parameters needed, as well as a name for each parameter. Derived
 * classes should also probably derive from molstat::Observable so that the
 * MolStat simulator knows that this model is compatible with that observable.
 *
 * Note: the reason for virtual inheritance is that the compatible_observables
 * map needs to be unique. The constructor in the molstat::Observable template
 * registers the function with this map.
 *
 * Construction via the molstat::SimulateModelFactory interface ensures that
 * all required random number distributions are specified before using the
 * model to simulate data.
 */
class SimulateModel : public std::enable_shared_from_this<SimulateModel>
{
protected:
	/**
	 * \brief Factories that produce an observable's function, assuming the
	 *    observable and model are compatible.
	 *
	 * The map is keyed by the molstat::ObservableIndex for the observable's
	 * class.
	 */
	std::map<ObservableIndex, ObservableFactory> compatible_observables;

	/**
	 * \brief Ordered vector of random number distributions for the various
	 *    model parameters.
	 */
	std::vector<std::shared_ptr<const RandomDistribution>> dists;

	/**
	 * \brief Gets a map of parameter name to index.
	 *
	 * \return The ordered list of names of distributions.
	 */
	virtual std::vector<std::string> get_names() const = 0;

	SimulateModel() = default;

public:
	virtual ~SimulateModel() = default;

	/**
	 * \brief Gets the type of this model.
	 *
	 * By default, all models are complete, and the type is
	 * molstat::SimulateModel. It may sometimes be necessary, however, to
	 * restrict the types of models we want to consider, and this function
	 * should be overriden.
	 *
	 * \return The type of model this class represents.
	 */
	virtual SimulateModelType getModelType() const;

	/**
	 * \brief Gets the number of model parameters for this model.
	 *
	 * \return The number of parameters used by the model for calculating
	 *    observables.
	 */
	virtual std::size_t get_num_parameters() const;

	/**
	 * \brief Gets a function that calculates an observable, given a set of
	 *    model parameters.
	 *
	 * Verifies that the model and observable are compatible and then returns
	 * a function that calculates the observable using `this`.
	 *
	 * \throw molstat::IncompatibleObservable if the desired observable is
	 *    incompatible with this model.
	 *
	 * \param[in] obs The type_index of the class for the observable.
	 * \return A function that calculates the observable.
	 */
	ObservableFunction getObservableFunction(const ObservableIndex &obs) const;

	/**
	 * \brief Generates a set of model parameters using the specified random
	 *    distributions.
	 *
	 * \param[in] engine The C++11 random number engine.
	 * \return A set of model parameters.
	 */
	virtual std::valarray<double> generateParameters(Engine &engine) const;

	// the factory needs to get at the internal details
	friend class SimulateModelFactory;
};

/**
 * \brief Specifies that a class is a "submodel" of the specified type.
 *
 * This class, similar to molstat::Observable uses a curiosuly recurring
 * template pattern to override the molstat::SimulateModel::getModelType
 * funtion.
 *
 * \tparam T The type of the submodel.
 */
template<typename T>
class SimulateSubmodel
	: public virtual SimulateModel
{
protected:
	/**
	 * \brief Sets the type of this model to be itself.
	 *
	 * \return The type of submodel this class represents.
	 */
	virtual SimulateModelType getModelType() const override
	{
		return std::type_index{ typeid(T) };
	}
};

/**
 * \brief Base class for a composite model that uses both model parameters
 *    and other independent models to calculate observables.
 *
 * This class allows other submodels to be used together to calculate
 * observables. Composite models that require a specific type of submodel
 * should override the getModelType function to return the submodel type.
 */
class CompositeSimulateModel
	: public virtual SimulateModel
{
protected:
	CompositeSimulateModel() = default;

	/**
	 * \brief List of the underlying submodels.
	 *
	 * Each element in the list corresponds to a submodel; this list also helps
	 * route the correct model parameters from the composite model to each
	 * submodel.
	 *
	 * The first element is a pointer to the submodel and the second element is
	 * an array of the indices of composite model parameters for that submodel.
	 */
	std::list<std::pair<std::shared_ptr<SimulateModel>,
		                  const std::valarray<std::size_t>>>
		submodels;

public:
	virtual ~CompositeSimulateModel() = default;

	/**
	 * \brief A list of submodels and the parameters that should be passed to
	 *    them.
	 *
	 * Each element in the list corresponds to a submodel; the purpose of this
	 * list is to route the correct model parameters from the composite model
	 * to each submodel.
	 *
	 * The first element is a pointer to the submodel and the second element is
	 * an array of the parameters for that submodel.
	 */
	using SubmodelParameters =
		std::list<std::pair<std::shared_ptr<const SimulateModel>,
		                    const std::valarray<double>>>;

	/**
	 * \brief Returns the type of submodels for this composite model.
	 *
	 * \return The molstat::SimulateModelType.
	 */
	virtual SimulateModelType getSubmodelType() const = 0;

	/**
	 * \brief Partition a set of parameters for the composite model into
	 *    sets of parameters for each submodel.
	 *
	 * \param[in] cparams The set of composite model parameters.
	 * \return A \c SubmodelParameters list associating each submodel with the
	 *    parameters that should be passed to it.
	 */
	SubmodelParameters routeSubmodelParameters(
		const std::valarray<double> &cparams) const;

	/**
	 * \brief Gets the number of model parameters needed directly by the
	 *    composite model.
	 */
	std::size_t get_num_composite_parameters() const;

	/**
	 * \brief Gets the number of model parameters for this model.
	 *
	 * This accumulates all submodel parameters as well as any parameters
	 * required by the composite model itself.
	 *
	 * \return The number of parameters used by the model for calculating
	 *    observables.
	 */
	virtual std::size_t get_num_parameters() const override final;

	/**
	 * \brief Generates a set of model parameters using the specified random
	 *    distributions.
	 *
	 * This override samples from the distributions required by the composite
	 * model, as well as all distributions for the submodels.
	 *
	 * \param[in] engine The C++11 random number engine.
	 * \return A set of model parameters.
	 */
	virtual std::valarray<double> generateParameters(Engine &engine) const
		override final;

	// the factory needs to get at the internal details
	friend class SimulateModelFactory;

	// the CompositeObservable class needs to get at the submodel information
	template<typename T>
	friend class CompositeObservable;
};

/**
 * \brief Indicates that the composite model requires submodels of the
 *    templated type.
 *
 * \tparam T The type of submodel.
 */
template<typename T>
class UseSubmodelType
	: public virtual CompositeSimulateModel
{
	/**
	 * \brief Returns the type of submodels for this composite model.
	 *
	 * \return The molstat::SimulateModelType.
	 */
	virtual SimulateModelType getSubmodelType() const override
	{
		return std::type_index{ typeid(T) };
	}
};

/**
 * \brief Factory class for creating a molstat::SimulateModel at runtime.
 *
 * This class instantiates models at runtime, making sure the model is in a
 * valid state before giving the user access to it.
 */
class SimulateModelFactory
{
private:
	SimulateModelFactory() = default;

	/// Pointer to the model being constructed.
	std::shared_ptr<SimulateModel> model;

	/**
	 * \brief Pointer to the model, cast as a molstat::CompositeSimulateModel.
	 *
	 * If the model is not a composite model, this will be `nullptr`.
	 */
	std::shared_ptr<CompositeSimulateModel> comp_model;

	/**
	 * \brief The set of distribution names for the model that still need to
	 *    be specified.
	 */
	std::set<std::string> remaining_names;

	/// Cache of the names of distributions required by the model.
	std::vector<std::string> model_names;

public:
	/// \cond
	SimulateModelFactory(SimulateModelFactory &&) = default;
	SimulateModelFactory &operator=(SimulateModelFactory &&) = default;
	/// \endcond

	/**
	 * \brief Creates a molstat::SimulateModelFactory with the underlying model
	 *    of type T.
	 *
	 * \tparam T The type of molstat::SimulateModel to build with this factory.
	 * \return The factory.
	 */
	template<typename T>
	static SimulateModelFactory makeFactory();

	/**
	 * \brief Adds a random distribution to the model.
	 *
	 * \param[in] name The name of the distribution being added.
	 * \param[in] dist The distribution being added.
	 * \param[out] used_dist If specified, true if dist was used,
	 *    false otherwise. Unused if nullptr.
	 * \return The factory.
	 */
	 SimulateModelFactory &setDistribution(std::string name,
	 	std::shared_ptr<const RandomDistribution> dist,
	 	bool *used_dist = nullptr);

	 /**
	  * \brief Adds a submodel to the composite model being constructed.
	  *
	  * \throw molstat::NotCompositeSimulateModel if the model being constructed
	  *    is not a composite model.
	  * \throw molstat::IncompatibleSubmodel if the submodel being added is not
	  *    the type of model expected by the composite model.
	  *
	  * \param[in] submodel_add The model to be added.
	  * \return The factory.
	  */
	 SimulateModelFactory &addSubmodel(
	 	std::shared_ptr<SimulateModel> submodel_add);

	 /**
	  * \brief Returns the constructed model.
	  *
	  * Some runtime error checking, such as making sure all distributions are
	  * specified is performed here.
	  *
	  * \throw molstat::MissingDistribution if one of the required distributions
	  *    has not been specified.
	  * \throw molstat::NoSubmodels if a composite model is being constructed
	  *    and no submodels have been specified.
	  *
	  * \return Pointer to the constructed model.
	  */
	 std::shared_ptr<SimulateModel> getModel();
};

/**
 * \brief Gets a function that produces a molstat::ObservableIndex for the
 *    given observable type.
 *
 * \tparam T The type of molstat::Observable to use.
 * \return A function that gives the observable's molstat::ObservableIndex.
 */
template<typename T>
inline ObservableIndex GetObservableIndex()
{
	return std::type_index{ typeid(T) };
}

/// Shortcut for a function that constructs a molstat::SimulateModelFactory.
using SimulateModelFactoryFunction =
	std::function<SimulateModelFactory()>;

/**
 * \brief Gets a function that produces a molstat::SimulateModelFactory
 *    for the given model type.
 *
 * \tparam T The type of molstat::SimulateModel to construct.
 * \return A function that creates the model when invoked.
 */
template<typename T>
inline SimulateModelFactoryFunction GetSimulateModelFactory()
{
	return [] () -> SimulateModelFactory {
		return SimulateModelFactory::makeFactory<T>();
	};
}

// templated definitions
template<typename T>
SimulateModelFactory SimulateModelFactory::makeFactory()
{
	using namespace std;

	SimulateModelFactory factory;

	factory.model = make_shared<T>();

	// attempt to cast to a composite model
	factory.comp_model =
		dynamic_pointer_cast<CompositeSimulateModel>(factory.model);

	// get the names of required distributions
	factory.model_names = factory.model->get_names();

	// convert the ordered vector to just a set
	for(const std::string &iter : factory.model_names)
		factory.remaining_names.emplace(to_lower(iter));

	// set the size of the model's vector of distributions
	factory.model->dists.resize(factory.model->get_num_parameters());

	return factory;
}

} // namespace molstat

#endif
