/*
 * PreprocessFactory.h
 *
 *  Created on: Dec 30, 2017
 *      Author: Daniel Rehfeldt
 */

#ifndef PIPS_IPM_CORE_QPPREPROCESS_PREPROCESSFACTORY_H_
#define PIPS_IPM_CORE_QPPREPROCESS_PREPROCESSFACTORY_H_

#include "EquiStochScaler.h"
#include "GeoStochScaler.h"
#include "StochPresolver.h"
#include "PreprocessType.h"
#include "StochPostsolver.h"
#include "DistributedProblem.hpp"

class PreprocessFactory {
public:

   static Scaler* make_scaler(const Problem& problem, ScalerType type) {
      switch (type) {
         case ScalerType::SCALER_EQUI_STOCH:
            return new EquiStochScaler(problem, false);
         case ScalerType::SCALER_GEO_STOCH:
            return new GeoStochScaler(problem, false, false);
         case ScalerType::SCALER_GEO_EQUI_STOCH:
            return new GeoStochScaler(problem, true, false);
         default:
            return nullptr;
      }
   };

   static Presolver* make_presolver(DistributedTree* tree, const Problem* data, PresolverType type, Postsolver* postsolver = nullptr) {
      assert(data);
      switch (type) {
         case PresolverType::PRESOLVER_STOCH:
            return new StochPresolver(tree, *data, postsolver);
         default:
            return nullptr;
      }
   };

   // todo : not sure about the factory design here
   static Postsolver* make_postsolver(const Problem* original_problem) {
      return new StochPostsolver(dynamic_cast<const DistributedProblem&>(*original_problem));
   }
};

#endif /* PIPS_IPM_CORE_QPPREPROCESS_PREPROCESSFACTORY_H_ */
