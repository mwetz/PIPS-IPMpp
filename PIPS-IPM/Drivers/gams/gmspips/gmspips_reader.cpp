/*
 * gmspips_reader.cpp
 *
 *  Created on: 19.03.2021
 *      Author: bzfkempk
 */

#include "gmspips_reader.hpp"

#include "../../../Core/Interface/PIPSIPMppInterface.hpp"
#include "../../../Core/InteriorPointMethod/TerminationStatus.hpp"

#include <cstring>

const size_t MAX_PATH_LENGHT = 256;

extern "C" typedef int (* FNNZ)(void* user_data, int id, int* nnz);

/* Row-major format */
extern "C" typedef int (* FMAT)(void* user_data, int id, int* krowM, int* jcolM, double* M);

extern "C" typedef int (* FVEC)(void* user_data, int id, double* vec, int len);

extern "C" {

static int gms_rank{0};
static bool allGDX{false};
static char fileName[MAX_PATH_LENGHT];
static char GDXDirectory[MAX_PATH_LENGHT];
static int numBlocks = 0; // TODO make size_t?
FILE* fLog;

#define checkAndAlloc(blk)                                                        \
   if (!blocks[blk])                                                                 \
   {                                                                                 \
      int rc;                                                                        \
      fprintf(fLog,"Block %d read on gms_rank %d\n", blk, gms_rank);                   \
      blocks[blk] = (GMSPIPSBlockData_t*) malloc(sizeof(GMSPIPSBlockData_t));        \
      if ( !allGDX )                                                                 \
      {                                                                              \
         char fname[MAX_PATH_LENGHT];                                                \
         int r = snprintf(fname, MAX_PATH_LENGHT, "%s%d.gdx", fileName, blk);        \
         if( r < 0 )  abort();                                                       \
         rc = readBlock(numBlocks,blk,0,1,fname,&GDXDirectory[0],blocks[blk]);       \
      }                                                                              \
      else                                                                           \
         rc = readBlock(numBlocks,blk,0,1,fileName,&GDXDirectory[0],blocks[blk]);    \
      if (rc) {fprintf(fLog,"Block %d read on gms_rank %d failed rc=%d\n", blk, gms_rank, rc); return rc;} \
   }

#define nCB(nType)                                                   \
   int fsize##nType(void* user_data, int id, int* nnz)                  \
   {                                                                    \
      GMSPIPSBlockData_t** blocks = (GMSPIPSBlockData_t**) user_data;   \
      checkAndAlloc(id);                                                \
      GMSPIPSBlockData_t* blk = blocks[id];                             \
      assert(blk);                                                      \
      *nnz = blk->nType;                                                \
      fprintf(fLog,"nCB blk=%d " #nType " %d\n",id,*nnz);               \
      return 0;                                                         \
   }

nCB(ni)
nCB(mA)
nCB(mC)
nCB(mBL)
nCB(mDL)

#define nnzCB(nnzType)                                               \
   int fnonzero##nnzType(void* user_data, int id, int* nnz)             \
   {                                                                    \
      GMSPIPSBlockData_t** blocks = (GMSPIPSBlockData_t**) user_data;   \
      checkAndAlloc(id);                                                \
      GMSPIPSBlockData_t* blk = blocks[id];                             \
      assert(blk);                                                      \
      *nnz = blk->nnz##nnzType;                                         \
      fprintf(fLog,"nnzCB blk=%d " #nnzType " %d\n",id,*nnz);           \
      return 0;                                                         \
   }

nnzCB(A)
nnzCB(B)
nnzCB(C)
nnzCB(D)
nnzCB(BL)
nnzCB(DL)

#define vecCB(vecType, size)                                         \
   int fvec##vecType(void* user_data, int id, double* vec, int len)     \
   {                                                                    \
      GMSPIPSBlockData_t** blocks = (GMSPIPSBlockData_t**) user_data;   \
      checkAndAlloc(id);                                                \
      GMSPIPSBlockData_t* blk = blocks[id];                             \
      assert(blk);                                                      \
      fprintf(fLog,"vecCB blk=%d " #vecType " len=%d allocLen %d\n",id,blk->size,len); \
      assert(len == blk->size);                                         \
      for( int i = 0; i < len; i++ ) {                                  \
         vec[i] = blk->vecType[i];                                      \
         fprintf(fLog,"  i=%d val=%g\n",i,vec[i]);                      \
      }                                                                 \
      return 0;                                                         \
   }

vecCB(c, ni)
vecCB(xlow, ni)
vecCB(ixlow, ni)
vecCB(xupp, ni)
vecCB(ixupp, ni)
vecCB(ixtyp, ni)
vecCB(b, mA)
vecCB(clow, mC)
vecCB(iclow, mC)
vecCB(cupp, mC)
vecCB(icupp, mC)
vecCB(bL, mBL)
vecCB(dlow, mDL)
vecCB(idlow, mDL)
vecCB(dupp, mDL)
vecCB(idupp, mDL)


#define matCB(mat, mmat)                                                     \
   int fmat##mat(void* user_data, int id, int* krowM, int* jcolM, double* M) \
   {                                                                         \
      assert(krowM && jcolM && M);                                           \
      GMSPIPSBlockData_t** blocks = (GMSPIPSBlockData_t**) user_data;        \
      checkAndAlloc(id);                                                     \
      GMSPIPSBlockData_t* blk = blocks[id];                                  \
      assert(blk);                                                           \
      fprintf(fLog,"matCB blk=%d " #mat " mLen %d nzLen %ld\n",id,blk->m##mmat,blk->nnz##mat); \
      if ( 0==blk->m##mmat )                                                 \
      {                                                                      \
        fprintf(fLog," empty\n"); \
        krowM[0] = 0;                                                        \
        return 0;                                                            \
      }                                                                      \
                                                                             \
      assert(blk->rm##mat);                                                  \
      for( int i = 0; i <= blk->m##mmat; i++ ) {                             \
         krowM[i] = blk->rm##mat[i];                                         \
         fprintf(fLog,"  i=%d krowM=%d\n",i,krowM[i]);                       \
      }                                                                      \
                                                                             \
      for( int k = 0; k < blk->nnz##mat; k++ ) {                             \
         jcolM[k] = blk->ci##mat[k];                                         \
         M[k] = blk->val##mat[k];                                            \
         fprintf(fLog,"  k=%d jcolM=%d M=%g\n",k,jcolM[k],M[k]);             \
      }                                                                      \
      return 0;                                                              \
   }

matCB(A, A)
matCB(B, A)
matCB(C, C)
matCB(D, C)
matCB(BL, BL)
matCB(DL, DL)

int fnonzeroQ(void*, int, int* nnz) {
   *nnz = 0;
   return 0;
}

int fmatQ(void* user_data, int id, int* krowM, int*, double*) {
   GMSPIPSBlockData_t* blk = ((GMSPIPSBlockData_t**) user_data)[id];
   assert(blk);

   for (int i = 0; i <= blk->ni; i++)
      krowM[i] = 0;

   return 0;
}
} /* extern C */


gmspips_reader::gmspips_reader(std::string path_to_problem_, std::string path_to_gams_, size_t n_blocks_) :
   pips_reader(std::move(path_to_problem_), n_blocks_), path_to_gams{std::move(path_to_gams_)} {
   initGMSPIPSIO();

   blocks.resize(n_blocks);

   gms_rank = PIPS_MPIgetRank();

   const std::string gdx_end = ".gdx";
   if (gdx_end.size() <= path_to_problem.size() && std::equal(gdx_end.rbegin(), gdx_end.rend(), path_to_problem.rbegin()))
      allGDX = true;

   assert(path_to_problem.size() < MAX_PATH_LENGHT);
   strcpy(fileName, path_to_problem.c_str());

   assert(path_to_gams.size() < MAX_PATH_LENGHT);
   strcpy(GDXDirectory, path_to_gams.c_str());


   const std::string log_file = log_reading ? "log%d.txt" + gms_rank : "/dev/null";
   fLog = fopen(log_file.c_str(), "w+");
   fprintf(fLog, "PIPS Log for gms_rank %d\n", gms_rank);

   numBlocks = n_blocks;
}

gmspips_reader::~gmspips_reader() {
   for (auto& block : blocks) {
      freeBlock(block);
      free(block);
   }

   fclose(fLog);
}

std::unique_ptr<DistributedInputTree> gmspips_reader::read_problem() {
   FNNZ fsni = &fsizeni;
   FNNZ fsmA = &fsizemA;
   FNNZ fsmC = &fsizemC;
   FNNZ fsmBL = &fsizemBL;
   FNNZ fsmDL = &fsizemDL;
   FNNZ fnnzQ = &fnonzeroQ;
   FNNZ fnnzA = &fnonzeroA;
   FNNZ fnnzB = &fnonzeroB;
   FNNZ fnnzC = &fnonzeroC;
   FNNZ fnnzD = &fnonzeroD;
   FNNZ fnnzBL = &fnonzeroBL;
   FNNZ fnnzDL = &fnonzeroDL;
   FVEC fc = &fvecc;
   FVEC fxlow = &fvecxlow;
   FVEC fixlow = &fvecixlow;
   FVEC fxupp = &fvecxupp;
   FVEC fixupp = &fvecixupp;
   FVEC fixtyp = &fvecixtyp;
   FVEC fb = &fvecb;
   FVEC fclow = &fvecclow;
   FVEC ficlow = &fveciclow;
   FVEC fcupp = &fveccupp;
   FVEC ficupp = &fvecicupp;
   FVEC fbL = &fvecbL;
   FVEC fdlow = &fvecdlow;
   FVEC fidlow = &fvecidlow;
   FVEC fdupp = &fvecdupp;
   FVEC fidupp = &fvecidupp;

   FMAT fA = &fmatA;
   FMAT fB = &fmatB;
   FMAT fC = &fmatC;
   FMAT fD = &fmatD;
   FMAT fBL = &fmatBL;
   FMAT fDL = &fmatDL;
   FMAT fQ = &fmatQ;

   //build the problem tree
   std::unique_ptr<DistributedInputTree::DistributedInputNode> data_root = std::make_unique<DistributedInputTree::DistributedInputNode>(blocks.data(), 0, fsni, fsmA, fsmBL, fsmC, fsmDL, fQ, fnnzQ, fc, fA, fnnzA, fB, fnnzB, fBL, fnnzBL, fb, fbL,
         fC, fnnzC, fD, fnnzD, fDL, fnnzDL, fclow, ficlow, fcupp, ficupp, fdlow, fidlow, fdupp, fidupp, fxlow, fixlow, fxupp, fixupp, fixtyp, false);
   std::unique_ptr<DistributedInputTree> root = std::make_unique<DistributedInputTree>(std::move(data_root));

   for (int blk = 1; blk < numBlocks; blk++) {
      std::unique_ptr<DistributedInputTree::DistributedInputNode> data = std::make_unique<DistributedInputTree::DistributedInputNode>(blocks.data(), blk, fsni, fsmA, fsmBL, fsmC, fsmDL, fQ, fnnzQ, fc, fA, fnnzA, fB, fnnzB, fBL, fnnzBL, fb,
            fbL, fC, fnnzC, fD, fnnzD, fDL, fnnzDL, fclow, ficlow, fcupp, ficupp, fdlow, fidlow, fdupp, fidupp, fxlow, fixlow, fxupp, fixupp, fixtyp, false);

      root->add_child(std::make_unique<DistributedInputTree>(std::move(data)));
   }

   return root;
}

void gmspips_reader::write_solution(PIPSIPMppInterface& solver_instance, const std::string& file_name) const {

   if (solver_instance.termination_status() != TerminationStatus::SUCCESSFUL_TERMINATION) {
      if (my_rank == 0)
         std::cout << "Priting solution although PIPSIPMpp solve did not terminate successfully!\n";
   }

   const double objective = solver_instance.getObjective();
   auto primalSolVec = solver_instance.gatherPrimalSolution();
   auto dualSolEqVec = solver_instance.gatherDualSolutionEq();
   auto dualSolIneqVec = solver_instance.gatherDualSolutionIneq();
   auto dualSolVarBounds = solver_instance.gatherDualSolutionVarBounds();

   auto eqValues = solver_instance.gatherEqualityConsValues();
   auto ineqValues = solver_instance.gatherInequalityConsValues();

   if (PIPS_MPIgetRank() == 0) {
      const int rc = writeSolution(file_name.c_str(), primalSolVec.size(), dualSolEqVec.size(), dualSolIneqVec.size(),
         objective, primalSolVec.data(), dualSolVarBounds.data(), eqValues.data(), ineqValues.data(), dualSolEqVec.data(),
         dualSolIneqVec.data(), path_to_gams.c_str());

      if (0 == rc)
         std::cout << "Solution written to " << file_name << "_sol.gdx\n";
      else if (-1 == rc)
         std::cout << "Could not access " << file_name << ".map\n";
      else
         std::cout << "Other error writing solution: rc=" << rc << "\n";
   }
}
