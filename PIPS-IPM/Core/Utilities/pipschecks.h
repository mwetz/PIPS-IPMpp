/*
 * pipschecks.h
 *
 *  Created on: 23.02.2018
 *      Author: bzfrehfe
 */

#ifndef PIPS_IPM_CORE_UTILITIES_PIPSCHECKS_H_
#define PIPS_IPM_CORE_UTILITIES_PIPSCHECKS_H_

#include "DistributedVector.h"
#include "DistributedMatrix.h"
#include "SimpleVector.h"
#include "pipsdef.h"
#include <vector>

// is the permuation vector valid?
bool permutationIsValid(const Permutation& perm);

// are the columns of the given sub-matrix ordered?
bool subMatrixIsOrdered(const int* rowptr, const int* colidx, int rowstart, int rowend);

// compute residual norms for Ax=rhs with A in CSR with 1-indexing (Fortran)
void computeFortranCSRMatResidualNorms(const int* rowptr, const int* colidx, const double* vals, /*const*/ SimpleVector<double>& rhs,
      /*const*/ SimpleVector<double>& x, double& res_norm2, double& res_nrmInf, double& sol_inf, double& mat_max);

#endif /* PIPS_IPM_CORE_UTILITIES_PIPSCHECKS_H_ */
