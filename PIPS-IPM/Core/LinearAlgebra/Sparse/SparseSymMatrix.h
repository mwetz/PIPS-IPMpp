/* OOQP                                                               *
 * Authors: E. Michael Gertz, Stephen J. Wright                       *
 * (C) 2001 University of Chicago. See Copyright Notification in OOQP */

#ifndef SPARSESYMMATRIX_H
#define SPARSESYMMATRIX_H

#include "DoubleMatrix.h"

#include "SparseStorage.h"
#include "Vector.hpp"
#include "SmartPointer.h"
#include "SparseSymMatrixHandle.h"

class SparseGenMatrix;

/** Represents sparse symmetric matrices stored in
 *  row-major Harwell-Boeing format.
 *  @ingroup SparseLinearAlgebra
 */
class SparseSymMatrix : public SymMatrix {
   SparseStorageHandle mStorage;
public:
   SparseSymMatrix();
   SparseSymMatrix(const SparseSymMatrix& mat);

   SparseSymMatrix(int size, int nnz, bool isLower = true);
   SparseSymMatrix(int size, int nnz, int krowM[], int jcolM[], double M[], int deleteElts = 0, bool isLower = true);
   SparseSymMatrix(SparseStorage* m_storage, bool is_lower_);

   SparseStorage& getStorageRef() { return *mStorage; }
   [[nodiscard]] const SparseStorage& getStorageRef() const { return *mStorage; }
   SparseStorageHandle getStorageHandle() { return mStorage; }
   [[nodiscard]] SparseStorageHandle getStorageHandle() const { return mStorage; }

   // is lower part of matrix stored? (otherwise upper part is stored)
   const bool isLower;

   int* krowM() { return mStorage->krowM; }
   int* jcolM() { return mStorage->jcolM; }
   double* M() { return mStorage->M; }

   [[nodiscard]] const int* krowM() const { return mStorage->krowM; }
   [[nodiscard]] const int* jcolM() const { return mStorage->jcolM; }
   [[nodiscard]] const double* M() const { return mStorage->M; }

   [[nodiscard]] int isKindOf(int type) const override;

   void putSparseTriple(const int irow[], int len, const int jcol[], const double A[], int& info) override;
   void fromGetDense(int row, int col, double* A, int lda, int rowExtent, int colExtent) const override;
   void fromGetSpRow(int row, int col, double A[], int lenA, int jcolA[], int& nnz, int colExtent, int& info) const override;

   void symmetricScale(const Vector<double>& vec) override;
   void columnScale(const Vector<double>& vec) override;
   void rowScale(const Vector<double>& vec) override;
   void scalarMult(double num) override;

   void symAtPutSpRow(int col, const double A[], int lenA, const int jcolA[], int& info) override;

   virtual void symPutZeroes();

   void getSize(long long& m, long long& n) const override;
   void getSize(int& m, int& n) const override;

   [[nodiscard]] long long size() const override;

   void getDiagonal(Vector<double>& vec) const override;
   void setToDiagonal(const Vector<double>& vec) override;
   void diagonal_add_constant_from(int from, int length, double value) override;
   void diagonal_set_to_constant_from(int from, int length, double value) override;

   void symAtPutSubmatrix(int destRow, int destCol, const DoubleMatrix& M, int srcRow, int srcCol, int rowExtent, int colExtent) override;

   virtual void mult(double beta, double y[], int incy, double alpha, const double x[], int incx) const;
   virtual void transMult(double beta, double y[], int incy, double alpha, const double x[], int incx) const;

   void mult(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const override;

   void transMult(double beta, Vector<double>& y, double alpha, const Vector<double>& x) const override;

   [[nodiscard]] double abmaxnorm() const override;
   [[nodiscard]] double abminnormNonZero(double tol) const override;

   void writeToStream(std::ostream& out) const override;
   void writeNNZpatternToStreamDense(std::ostream& out) const;
   void writeToStreamDense(std::ostream& out) const override;
   void writeToStreamDenseRow(std::ostream& out, int row) const override;

   void atPutDiagonal(int idiag, const Vector<double>& v) override;
   void atAddDiagonal(int idiag, const Vector<double>& v) override;

   void fromGetDiagonal(int idiag, Vector<double>& v) const override;

   /** The actual number of structural non-zero elements in this sparse
    *  matrix. This includes so-called "accidental" zeros, elements that
    *  are treated as non-zero even though their value happens to be zero.
    */
   [[nodiscard]] int numberOfNonZeros() const { return mStorage->numberOfNonZeros(); }

   /** Reduce the matrix to lower triangular */
   void reduceToLower();

   void deleteEmptyRowsCols(const Vector<int>& nnzVec);

   void deleteZeroRowsCols(int*& new2orgIdx);

   void getSparseTriplet_c2fortran(int*& irn, int*& jcn, double*& val) const;

   void getSparseTriplet_fortran2fortran(int*& irn, int*& jcn, double*& val) const;

   virtual SparseGenMatrix* shaveSymLeftBottom(int n_vars);

   ~SparseSymMatrix() override = default;

   [[nodiscard]] SymMatrix* clone() const override;
};

#endif