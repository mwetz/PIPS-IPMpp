/*
 * StochScaler.h
 *
 *  Created on: 18.07.2019
 *      Author: Nils-Christian Kempke
 */

#ifndef PIPS_IPM_CORE_QPPREPROCESS_STOCHSCALER_H_
#define PIPS_IPM_CORE_QPPREPROCESS_STOCHSCALER_H_

#include "QpScaler.h"

class Problem;

class StochScaler : public QpScaler {
public:
   StochScaler(Problem* prob, bool bitshifting);
   ~StochScaler() override = default;

   Variables* get_unscaled_variables(const Variables& vars) const override;
   Residuals* get_unscaled_residuals(const Residuals& resids) const override;
};

#endif /* PIPS_IPM_CORE_QPPREPROCESS_STOCHSCALER_H_ */