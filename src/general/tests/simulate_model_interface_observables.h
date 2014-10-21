/* This file is a part of MolStat, which is distributed under the Creative
   Commons Attribution-NonCommercial 4.0 International Public License.
   MolStat (c) 2014, Northwestern University. */
/**
 * \file simulate_model_interface_observables.h
 * \brief Dummy observables for testing the various MolStat classes for
 *    simulating data.
 *
 * \author Matthew G.\ Reuter
 * \date October 2014
 */

#ifndef __test_simulate_model_interface_observables_h__
#define __test_simulate_model_interface_observables_h__

#include <memory>
#include <valarray>
#include <vector>
#include <string>

#include <general/simulator_tools/observable.h>

using namespace std;

/**
 * \internal
 * \brief Dummy observable class.
 * \endinternal
 */
class BasicObs1 : public molstat::Observable<BasicObs1> {
public:
	BasicObs1() :
		Observable(&BasicObs1::Obs1) {}

	virtual ~BasicObs1() = default;
	virtual double Obs1(const valarray<double> &vals) const = 0;
};

/**
 * \internal
 * \brief Dummy observable class.
 * \endinternal
 */
class BasicObs2 : public molstat::Observable<BasicObs2> {
public:
	BasicObs2() :
		Observable(&BasicObs2::Obs2) {}

	virtual ~BasicObs2() = default;
	virtual double Obs2(const valarray<double> &vals) const = 0;
};

/**
 * \internal
 * \brief Dummy observable class.
 * \endinternal
 */
class BasicObs3 : public molstat::Observable<BasicObs3> {
public:
	BasicObs3() :
		Observable(&BasicObs3::Obs3) {}

	virtual ~BasicObs3() = default;
	virtual double Obs3(const valarray<double> &vals) const = 0;
};

/**
 * \internal
 * \brief Dummy observable class.
 * \endinternal
 */
class BasicObs4 : public molstat::Observable<BasicObs4> {
public:
	BasicObs4() :
		Observable(&BasicObs4::Obs4) {}

	virtual ~BasicObs4() = default;
	virtual double Obs4(const valarray<double> &vals) const = 0;
};

#endif