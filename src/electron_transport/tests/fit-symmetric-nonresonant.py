# This file is a part of MolStat, which is distributed under the Creative
# Commons Attribution-NonCommercial 4.0 International Public License.
# MolStat (c) 2014, Northwestern University.

##
 # @file tests/fit-symmetric-nonresonant.py
 # @brief Test suite for fitting to the symmetric, nonresonant-tunneling model.
 # 
 # @test Test suite for fitting to the symmetric, nonresonant-tunneling model.
 #
 # The data in symmetric-nonresonant.dat was generated by the simulator with
 # the following input
 # @verbatim
 # SymmetricOneSite
 # DifferentialConductance
 # 1000000
 # 100 log 10.
 # EF constant 0.
 # V constant 0.
 # epsilon normal 3. 0.05
 # gamma normal 0.5 0.05
 # a constant 0.
 # @endverbatim
 #
 # @author Matthew G.\ Reuter
 # @date September 2014

import subprocess

## @cond

process = subprocess.Popen('../../molstat-fitter', stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
output = process.communicate( \
'SymmetricNonresonant\n' \
'symmetric-nonresonant.dat\n')

# make sure no errors were reported
assert(output[1] == '')

# check the output string
assert(output[0] == \
'Resid = 1.599687e+04\n' \
'c=5.9267e+01, d=9.8774e+00, norm=1.0206e+05\n')

## @endcond
