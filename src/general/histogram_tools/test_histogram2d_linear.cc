/* This file is a part of MolStat, which is distributed under the Creative
   Commons Attribution-NonCommercial 4.0 International Public License.
   MolStat (c) 2014, Northwestern University. */
/**
 * \file test_histogram2d_linear.cc
 * \brief Test suite for the Histogram2D class (linear binning)
 *
 * \author Matthew G.\ Reuter
 * \date May 2014
 */

#include <cstdio>
#include <assert.h>
#include <cmath>

#include "histogram2d.h"
#include "bin_linear.h"

using namespace std;

/**
 * \brief Main function for testing the Histogram2D class (linear binning).
 *
 * \param[in] argc The number of command-line arguments.
 * \param[in] argv The command-line arguments.
 * \return Exit status: 0 if the code passes the test, non-zero otherwise.
 */
int main(int argc, char **argv) {
	shared_ptr<BinStyle> bstyle(make_shared<BinLinear>());
	Histogram2D hist({{2,2}}, {{0.,0.}}, {{1.,1.}}, bstyle);
	const double thresh = 1.0e-6;

	// artificially populate the histogram
	hist.add_data({{0.4, 0.4}}); // 1
	hist.add_data({{0.3, 0.7}}); // 2
	hist.add_data({{0.4, 0.0}}); // 1
	hist.add_data({{1.0, 0.7}}); // 4 -- outside since 1. is excluded
	hist.add_data({{0.1, 0.8}}); // 2
	hist.add_data({{0.6, 0.1}}); // 3
	hist.add_data({{0.2, 0.2}}); // 1
	hist.add_data({{0.3, 0.0}}); // 1
	hist.add_data({{0.7, 1.0}}); // 4 -- outside since 1. is excluded
	hist.add_data({{0.0, 0.8}}); // 2

	// check the bin contents and the iterator class
	Histogram2D::const_iterator iter = hist.begin();

	// bin 0, 0 (#1 above)
	// the average is (0.25, 0.25) and the bin count should be 4
	assert(abs(iter.get_variable()[0] - 0.25) < thresh);
	assert(abs(iter.get_variable()[1] - 0.25) < thresh);
	assert(abs(iter.get_bin_count() - 4.*bstyle->dudg(0.25)) < thresh);

	++iter;
	// bin 0, 1 (#2 above)
	// the average is (0.25, 0.75) and the bin count should be 3
	assert(abs(iter.get_variable()[0] - 0.25) < thresh);
	assert(abs(iter.get_variable()[1] - 0.75) < thresh);
	assert(abs(iter.get_bin_count() - 3.*bstyle->dudg(0.75)) < thresh);

	iter++;
	// bin 1, 0 (#3 above)
	// the average is (0.75, 0.25) and the bin count should be 1
	assert(abs(iter.get_variable()[0] - 0.75) < thresh);
	assert(abs(iter.get_variable()[1] - 0.25) < thresh);
	assert(abs(iter.get_bin_count() - 1.*bstyle->dudg(0.25)) < thresh);

	++iter;
	// bin 1, 1 (#4 above)
	// the average is (0.75, 0.75) and the bin count should be 0
	assert(abs(iter.get_variable()[0] - 0.75) < thresh);
	assert(abs(iter.get_variable()[1] - 0.75) < thresh);
	assert(abs(iter.get_bin_count() - 0.*bstyle->dudg(0.75)) < thresh);

	// just for sanity
	++iter;
	assert(iter == hist.end());

	return 0;
}
