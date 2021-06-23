//
// Created by nils-christian on 17.06.21.
//

#ifndef PIPSIPMPP_PIPS_READER_H
#define PIPSIPMPP_PIPS_READER_H

#include <memory>
#include "pipsdef.h"

class DistributedInputTree;

class pips_reader {
public:
   pips_reader(const std::string& path_to_problem_, size_t n_blocks_) :
      path_to_problem{path_to_problem_}, n_blocks{n_blocks_}, my_rank{PIPS_MPIgetRank()}{};
   virtual ~pips_reader() = default;

   virtual std::unique_ptr<DistributedInputTree> read_problem() = 0;
protected:
   const std::string path_to_problem{};

   // number of all blocks ! so N + 1
   size_t n_blocks{};
   const int my_rank{};
};


#endif //PIPSIPMPP_PIPS_READER_H