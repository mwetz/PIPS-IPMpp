/*
 * StochMatrixUtilities.h
 *
 *  Created on: 27.11.2019
 *      Author: bzfkempk
 */
#ifndef PIPS_IPM_CORE_STOCHLINEARALGEBRA_STOCHMATRIXUTILITIES_H_
#define PIPS_IPM_CORE_STOCHLINEARALGEBRA_STOCHMATRIXUTILITIES_H_

#include "SparseGenMatrix.h"
#include "StochGenMatrix.h"
#include "SystemType.h"
#include "pipsport.h"

#include <vector>

inline SparseGenMatrix* getSparseGenMatrixFromStochMat(const StochGenMatrix& sMat, int smat_node, BlockType block_type)
{
   assert( -1 <= smat_node && smat_node < static_cast<int>(sMat.children.size()) );

   if(smat_node == -1)
   {
      if(block_type == BL_MAT)
      {
         assert( sMat.Blmat->isKindOf( kSparseGenMatrix ) );
         return dynamic_cast<SparseGenMatrix*>(sMat.Blmat);
      }
      else 
      {
         assert(block_type == B_MAT);
         assert( sMat.Bmat->isKindOf( kSparseGenMatrix ) );
         return dynamic_cast<SparseGenMatrix*>(sMat.Bmat);
      }
   }
   else
   {
      if(block_type == A_MAT)
      {
         assert( sMat.children[smat_node]->Amat->isKindOf( kSparseGenMatrix ) );
         return dynamic_cast<SparseGenMatrix*>(sMat.children[smat_node]->Amat);
      }
      else if(block_type == B_MAT)
      {
         assert( sMat.children[smat_node]->Bmat->isKindOf( kSparseGenMatrix ) );
         return dynamic_cast<SparseGenMatrix*>(sMat.children[smat_node]->Bmat);
      }
      else if(block_type == BL_MAT)
      {
         assert( sMat.children[smat_node]->Blmat->isKindOf( kSparseGenMatrix ) );
         return dynamic_cast<SparseGenMatrix*>(sMat.children[smat_node]->Blmat);
      }
   }
   return nullptr;
}

#endif /* PIPS_IPM_CORE_STOCHLINEARALGEBRA_STOCHMATRIXUTILITIES_H_ */
