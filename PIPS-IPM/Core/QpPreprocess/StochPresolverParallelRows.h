/*
 * StochPresolverParallelRows.h
 *
 *  Created on: 02.05.2018
 *      Author: bzfuslus
 */

#ifndef PIPS_IPM_CORE_QPPREPROCESS_STOCHPRESOLVERPARALLELROWS_H_
#define PIPS_IPM_CORE_QPPREPROCESS_STOCHPRESOLVERPARALLELROWS_H_

#include "StochPresolverBase.h"

#include <boost/unordered_set.hpp>

namespace rowlib
{
   static const double offset_hash_double = 0.127;

    struct rowWithColInd
    {
        int id;
        int offset_nA;
        int lengthA;
        int* colIndicesA;
        double* norm_entriesA;
        int lengthB;
        int* colIndicesB;
        double* norm_entriesB;

        rowWithColInd(int id, int offset, int lenA, int* colA, double* entA, int lenB, int* colB, double* entB)
            : id(id), offset_nA(offset), lengthA(lenA), colIndicesA(colA), norm_entriesA(entA),
              lengthB(lenB), colIndicesB(colB), norm_entriesB(entB)  {}
    };

    bool operator==(rowWithColInd const& a, rowWithColInd const& b);
    std::size_t hash_value(rowWithColInd const& b);

    struct rowWithEntries
    {
       int id;
       int offset_nA;
       int lengthA;
       int* colIndicesA;
       double* norm_entriesA;
       int lengthB;
       int* colIndicesB;
       double* norm_entriesB;

        rowWithEntries(int id, int offset, int lenA, int* colA, double* entA, int lenB, int* colB, double* entB)
            : id(id), offset_nA(offset), lengthA(lenA), colIndicesA(colA), norm_entriesA(entA),
              lengthB(lenB), colIndicesB(colB), norm_entriesB(entB) {}
    };

    bool operator==(rowWithEntries const& a, rowWithEntries const& b);
    std::size_t hash_value(rowWithEntries const& b);
}

class StochPresolverParallelRows : public StochPresolverBase
{
public:
   StochPresolverParallelRows(PresolveData& presData, const sData& origProb);

   ~StochPresolverParallelRows();

   // remove parallel rows
   virtual void applyPresolving();

private:
   /// extension to the pointer set from StochPresolverBase to point to C and A at the same moment rather than
   /// distinguishing between EQUALITY and INEQUALITY constraints
   const SparseStorageDynamic* currCmat;
   const SparseStorageDynamic* currCmatTrans;
   const SparseStorageDynamic* currDmat;
   const SparseStorageDynamic* currDmatTrans;
   const SimpleVector* currNnzRowC;

   // pointers to the normalized and copied matrix blocks
   SparseStorageDynamic* norm_Amat;
   SparseStorageDynamic* norm_Bmat;
   SparseStorageDynamic* norm_Cmat;
   SparseStorageDynamic* norm_Dmat;
   SimpleVector* norm_b;
   SimpleVector* norm_clow;
   SimpleVector* norm_cupp;
   SimpleVector* norm_iclow;
   SimpleVector* norm_icupp;
   SimpleVector* norm_factorC;
   SimpleVector* norm_factorA;

   // data for the nearly parallel row case
   SimpleVector* rowContainsSingletonVariableA;
   SimpleVector* rowContainsSingletonVariableC;
   SimpleVector* singletonCoeffsColParent;
   SimpleVector* singletonCoeffsColChild;
   SimpleVector* normNnzRowA;
   SimpleVector* normNnzRowC;
   SimpleVector* normNnzColParent;
   SimpleVector* normNnzColChild;
   SparseStorageDynamic* norm_AmatTrans;
   SparseStorageDynamic* norm_BmatTrans;
   SparseStorageDynamic* norm_CmatTrans;
   SparseStorageDynamic* norm_DmatTrans;
   SimpleVector* gParentAdaptions;

   // number of rows of the A or B block
   int mA;
   // number of columns of the A or C block
   int nA;

   // unordered set?
   boost::unordered_set<rowlib::rowWithColInd, boost::hash<rowlib::rowWithColInd> > row_support_hashtable;
   boost::unordered_set<rowlib::rowWithEntries, boost::hash<rowlib::rowWithEntries> > row_coefficients_hashtable;

   void setNormalizedPointers(int node);
   void setNormalizedPointersMatrices(int node);
   void setNormalizedPointersMatrixBounds(int node);
   void setNormalizedNormFactors(int node);
   void setNormalizedSingletonFlags(int node);
   void setNormalizedReductionPointers(int node);
   void updateExtendedPointersForCurrentNode(int node);
   void deleteNormalizedPointers(int it);


   void removeSingletonVars();
   void removeEntry(int colIdx, SimpleVector& rowContainsSingletonVar,
         SparseStorageDynamic& matrix, SparseStorageDynamic& matrixTrans, SimpleVector& nnzRow, SimpleVector& nnzCol,
         BlockType block_type);
   double removeEntryInDynamicStorage(SparseStorageDynamic& storage, int row, int col) const;

   void normalizeBlocksRowwise( SystemType system_type, SparseStorageDynamic* a_mat, SparseStorageDynamic* b_mat,
         SimpleVector* cupp, SimpleVector* clow, SimpleVector* icupp, SimpleVector* iclow) const;
   void insertRowsIntoHashtable( boost::unordered_set<rowlib::rowWithColInd, boost::hash<rowlib::rowWithColInd> > &rows,
         SparseStorageDynamic* Ablock, SparseStorageDynamic* Bblock, SystemType system_type, SimpleVector* nnzRow );
   void compareRowsInCoeffHashTable(int& nRowElims, int it);
   bool checkRowsAreParallel( rowlib::rowWithEntries row1, rowlib::rowWithEntries row2);

   void tightenOriginalBoundsOfRow1(SystemType system_type, int node, int rowId1, int rowId2);

   double getSingletonCoefficient(int singleColIdx);
   void tightenBoundsForSingleVar(int singleColIdx, double newxlow, double newxupp);
   bool doNearlyParallelRowCase1(int rowId1, int rowId2, int it);
   bool doNearlyParallelRowCase3(int rowId1, int rowId2, int it);
   void adaptObjective( int colIdx1, int colIdx2, double t, double d, int it);
   void tightenLinkingVarsBounds();

};

#endif /* PIPS_IPM_CORE_QPPREPROCESS_STOCHPRESOLVERPARALLELROWS_H_ */
