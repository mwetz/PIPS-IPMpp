#ifndef DATAQPSTOCH
#define DATAQPSTOCH

#include "QpGenData.h"
#include "StochSymMatrix.h"
#include "SparseSymMatrix.h"
#include "StochGenMatrix.h"
#include "StochVector.h"
#include "DoubleMatrixHandle.h"
#include "pipschecks.h"
#include <vector>

class sTree;
class LinearAlgebraPackage;
class QpGenVars;

class sData : public QpGenData {
 public:
  /** constructor that makes data objects of the specified dimensions */
  sData( sTree* tree);

  /** constructor that sets up pointers to the data objects that are
      passed as arguments */
  sData( sTree* stochNode,
	 OoqpVector * c, SymMatrix * Q,
	 OoqpVector * xlow, OoqpVector * ixlow, long long nxlow,
	 OoqpVector * xupp, OoqpVector * ixupp, long long nxupp,
	 GenMatrix * A, OoqpVector * bA,
	 GenMatrix * C,
	 OoqpVector * clow, OoqpVector * iclow, long long mclow,
	 OoqpVector * cupp, OoqpVector * ciupp, long long mcupp,
	 bool exploit2Links = false);

  std::vector<sData*> children;
  void AddChild(sData* child);
  sTree* stochNode;

 public:
  long long nxlow, nxupp, mclow, mcupp;

  std::vector<unsigned int> getLinkVarsPermInv();
  std::vector<unsigned int> getLinkConsEqPermInv();
  std::vector<unsigned int> getLinkConsIneqPermInv();

  int getLocalnx();
  int getLocalmy();
  int getLocalmyl();
  int getLocalmz();
  int getLocalmzl();
  int getLocalSizes(int& nx, int& my, int& mz);
  int getLocalSizes(int& nx, int& my, int& mz, int& myl, int& mzl);

  int getLocalNnz(int& nnzQ, int& nnzB, int& nnzD);
  int getN0LinkVars() {return n0LinkVars;}
  // returns upper bound on number of non-zeroes in Schur complement
  int getSchurCompMaxNnz();
  bool with2Links() {return use2Links;};
  SparseSymMatrix* createSchurCompSymbSparseUpper();

  SparseSymMatrix& getLocalQ();
  SparseGenMatrix& getLocalCrossHessian();
  SparseGenMatrix& getLocalA();
  SparseGenMatrix& getLocalB();
  SparseGenMatrix& getLocalF();
  SparseGenMatrix& getLocalC();
  SparseGenMatrix& getLocalD();
  SparseGenMatrix& getLocalG();


  void printLinkVarsStats();
  void printLinkConsStats();


  void sync();

 public:
  virtual double objectiveValue( QpGenVars * vars );
  virtual void createScaleFromQ();
  virtual void datainput() {};

  virtual ~sData();

 protected:
  void createChildren();
  void destroyChildren();

 private:
  int n0LinkVars;
  const static int nLinkStats = 21;
  const static double minStructuredLinksRatio = 0.5;
  static std::vector<unsigned int> get0VarsRightPermutation(const std::vector<int>& linkVarsNnzCount);
  static std::vector<unsigned int> getAscending2LinkPermutation(std::vector<int>& linkStartBlocks, size_t nBlocks);

  // nnz in Schur complement signified by given vector
  static int getSchurCompMaxNnz(const std::vector<int>& linkStartBlocks, const std::vector<int>& linkStartBlockLengths);

  // number of sparse 2-link rows
  static int n2linksRows(const std::vector<int>& linkStartBlockLengths);

  static std::vector<int> get2LinkLengthsVec(const std::vector<int>& linkStartBlocks, size_t nBlocks);

  bool use2Links;
  std::vector<int> linkVarsNnz;
  std::vector<int> linkStartBlocksA;
  std::vector<int> linkStartBlocksC;
  std::vector<int> linkStartBlockLengthsA;
  std::vector<int> linkStartBlockLengthsC;
  std::vector<unsigned int> linkVarsPermutation;
  std::vector<unsigned int> linkConsPermutationA;
  std::vector<unsigned int> linkConsPermutationC;

  void init2LinksData(bool exploit2links);
  void permuteLinkingVars();
  void permuteLinkingCons();
};


#endif
