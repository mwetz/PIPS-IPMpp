/* PIPS-IPM                                                           *
 * Author:  Cosmin G. Petra                                           *
 * (C) 2012 Argonne National Laboratory. See Copyright Notification.  */

#ifndef PIPSIPMPPINTERFACE_H
#define PIPSIPMPPINTERFACE_H

#include <memory>
#include <MehrotraStrategy.hpp>
#include <InteriorPointMethod.hpp>
#include "DistributedQP.hpp"
#include "DistributedResiduals.hpp"
#include "DistributedVariables.h"
#include "PreprocessFactory.h"
#include "Scaler.h"
#include "Presolver.h"
#include "Postsolver.h"
#include "pipsport.h"
#include "PIPSIPMppOptions.h"

class PIPSIPMppInterface {
public:
   PIPSIPMppInterface(DistributedInputTree* tree, MehrotraHeuristic mehrotra_heuristic, MPI_Comm = MPI_COMM_WORLD, ScalerType scaler_type = SCALER_NONE,
         PresolverType presolver_type = PRESOLVER_NONE, std::string settings = "PIPSIPMpp.opt");

   ~PIPSIPMppInterface() = default;

   void run();

   double getObjective();

   [[nodiscard]] double getFirstStageObjective() const;

   std::vector<double> gatherPrimalSolution();

   std::vector<double> gatherDualSolutionEq();

   std::vector<double> gatherDualSolutionIneq();

   std::vector<double> gatherDualSolutionIneqUpp();

   std::vector<double> gatherDualSolutionIneqLow();

   std::vector<double> gatherDualSolutionVarBounds();

   std::vector<double> gatherDualSolutionVarBoundsUpp();

   std::vector<double> gatherDualSolutionVarBoundsLow();

   [[nodiscard]] std::vector<double> getFirstStagePrimalColSolution() const;

   [[nodiscard]] std::vector<double> getSecondStagePrimalColSolution(int scen) const;

   void postsolveComputedSolution();

   std::vector<double> gatherEqualityConsValues();

   std::vector<double> gatherInequalityConsValues();

   void getVarsUnscaledUnperm();

   void getResidsUnscaledUnperm();
   //more get methods to follow here

private:
   void printComplementarityResiduals(const DistributedVariables& vars) const;

   std::vector<double> gatherFromSolution(SmartPointer<Vector<double> > DistributedVariables::* member_to_gather);

protected:
   DistributedFactory factory;
   std::unique_ptr<PreprocessFactory> preprocess_factory{};

   std::unique_ptr<DistributedQP> presolved_problem{};       // possibly presolved problem
   std::unique_ptr<DistributedQP> dataUnpermNotHier{}; // data after presolve before permutation, scaling and hierarchical data
   std::unique_ptr<DistributedQP> original_problem{};   // original data
   std::unique_ptr<DistributedVariables> variables{};
   std::unique_ptr<DistributedVariables> unscaleUnpermNotHierVars{};
   std::unique_ptr<DistributedVariables> postsolved_variables{};

   std::unique_ptr<DistributedResiduals> residuals{};
   std::unique_ptr<DistributedResiduals> unscaleUnpermNotHierResids{};
   std::unique_ptr<DistributedResiduals> postsolvedResids{};

   std::unique_ptr<Presolver> presolver{};
   std::unique_ptr<Postsolver> postsolver{};
   std::unique_ptr<Scaler> scaler{};
   std::unique_ptr<InteriorPointMethod> solver{};

   MPI_Comm comm = MPI_COMM_NULL;
   const int my_rank = -1;
   int result{-1};
   bool ran_solver = false;
};

#endif // PIPSIPMPPINTERFACE_H