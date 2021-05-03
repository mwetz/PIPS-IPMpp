/* OOQP                                                               *
 * Authors: E. Michael Gertz, Stephen J. Wright                       *
 * (C) 2001 University of Chicago. See Copyright Notification in OOQP */

#ifndef OPTIMIZATIONFACTORY
#define OPTIMIZATIONFACTORY

#include <iostream>
#include "Vector.hpp"

/**
 *  @defgroup AbstractProblemFormulation
 *
 *  Abstract base classes for defining a problem formulation.
 *
 * A quadratic program QP takes the form
 * @code
 * minimize    c'* x + (1/2) * x' * Q * x
 * subject to  A x  = b
 *             C x >= d
 * @endcode
 *
 * However, for many (possibly most) QP's, the matrices in the
 * formulation have structure that may be exploited to solve the
 * problem more efficiently. The AbstractProblemFormulation module
 * contains abstract base classes upon which these specialized
 * formulations are based. The optimality conditions of the simple QP
 * defined above as are follows:
 *
 * @code
 * rQ  = c + Q * x - A' * y - C' * z = 0
 * rA  = A * x - b                   = 0
 * rC  = C * x - s - d               = 0
 * r3  = S * z                       = 0
 * s, z >= 0
 * @endcode
 *
 * Where rQ, rA, rC and r3 newly defined quantities known as residual
 * vectors and x, y, z and s are variables of used in the solution of
 * the QPs.  
 * @{
 */
class Problem;

class Residuals;

class AbstractLinearSystem;

class Variables;

/**
 * Creates a compatible set of components representing a problem formulation specialized by structure.
 */
class ProblemFactory {
public:
   /** create x shaped vector */
   [[nodiscard]] virtual Vector<double>* make_primal_vector() const = 0;
   /** create dual A shaped vector */
   [[nodiscard]] virtual Vector<double>* make_equalities_dual_vector() const = 0;
   /** create dual C shaped vector */
   [[nodiscard]] virtual Vector<double>* make_inequalities_dual_vector() const = 0;
   /** create a rhs vector for the augmented system */
   [[nodiscard]] virtual Vector<double>* make_right_hand_side() const = 0;

   /** create the Residuals class for the relevant formulation */
   virtual Residuals* make_residuals(Problem& problem) = 0;

   /** creates the LinearSystem class for the relevant formulation */
   virtual AbstractLinearSystem* make_linear_system(Problem& problem) = 0;

   /** creates the Variables class for the relevant formulation */
   virtual Variables* make_variables(Problem& problem) = 0;

   virtual ~ProblemFactory() = default;

protected:
   /** number of elements in x */
   long long nx{0};

   /** number of rows in A and b including linking rows (sFactory..) */
   long long my{0};

   /** number of rows in C including linking rows */
   long long mz{0};

   ProblemFactory() = default;
   ProblemFactory(int nx_, int my_, int mz_) : nx(nx_), my(my_), mz(mz_) {};
};

//@}
#endif


