/*
This work is licensed under the Creative Commons Attribution 3.0 United States
License. To view a copy of this license, visit
http://creativecommons.org/licenses/by/3.0/us/ or send a letter to Creative
Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

Copyright (C) 2013 Oak Ridge National Laboratory
*/
/**
 * \file model-asymmetric-resonant.h
 * \brief Prototypes for the asymmetric-coupling, resonant tunneling model.
 *
 * This form is detailed in P.\ D.\ Williams, M.\ G.\ Reuter. J.\ Phys.\ 
 * Chem.\ C \b 117, 5937-5942 (2013).
 *
 * \author Matthew G.\ Reuter
 * \date July 2012, May 2013
 */

/**
 * \brief Computes the residuals using the resonance model function
 *        for symmetric coupling.
 *
 * \param[in] params The {c} parameters from GSL.
 * \param[in] data The data points to be fitted.
 * \param[out] resid The residuals.
 * \return GSL_SUCCESS if no errors.
 */
int asymmetric_coupling_resonance_f(const gsl_vector *params, void *data,
	gsl_vector *resid);

/**
 * \brief Computes the Jacobian using the resonance model function
 *        for symmetric coupling.
 *
 * \param[in] params The {c} parameters from GSL.
 * \param[in] data The data points to be fitted.
 * \param[out] J The Jacobian.
 * \return GSL_SUCCESS if no errors.
 */
int asymmetric_coupling_resonance_df(const gsl_vector *params,
	void *data, gsl_matrix *J);

/**
 * \brief Computes the residuals and the Jacobian using the resonance model
 *        function for symmetric coupling.
 *
 * \param[in] params The {c} parameters from GSL.
 * \param[in] data The data points to be fitted.
 * \param[out] resid The residuals.
 * \param[out] J The Jacobian.
 * \return GSL_SUCCESS if no errors.
 */
int asymmetric_coupling_resonance_fdf(const gsl_vector *params, void *data,
	gsl_vector *resid, gsl_matrix *J);