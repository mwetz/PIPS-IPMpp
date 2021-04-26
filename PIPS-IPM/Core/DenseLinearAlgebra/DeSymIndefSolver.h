/* OOQP                                                               *
 * Authors: E. Michael Gertz, Stephen J. Wright                       *
 * (C) 2001 University of Chicago. See Copyright Notification in OOQP */

#ifndef DESYMINDEFSOLVER_H
#define DESYMINDEFSOLVER_H

#include "DoubleLinearSolver.h"
#include "DenseSymMatrixHandle.h"
#include "SparseSymMatrix.h"
#include "DenseStorage.h"
#include "pipsport.h"

#include <vector>
#include <memory>

/** A linear solver for dense, symmetric indefinite systems
 * @ingroup DenseLinearAlgebra
 * @ingroup LinearSolvers
 */
class DeSymIndefSolver : public DoubleLinearSolver
{
   public:
      DeSymIndefSolver(const DenseSymMatrix *storage);
      DeSymIndefSolver(const SparseSymMatrix *storage);

      void diagonalChanged(int idiag, int extent) override;
      void matrixChanged() override;

      using DoubleLinearSolver::solve;
      void solve(OoqpVector &vec) override;
      void solve(GenMatrix &vec) override;

      ~DeSymIndefSolver() override = default;

   protected:

      /* in PIPS symmetric matrices will be lower diagonal matrices which makes them upper diagonal in fortran access */
      const char fortranUplo = 'U';

      std::shared_ptr<DenseStorage> mStorage;
      std::vector<double> work;
      std::vector<int> ipiv;

      const SparseSymMatrix* sparseMat{};
};

#endif
