//Lyle Hubbard

// This file includes a few concepts that are more advanced.
// Operator overloading, extending STL classes,  friends, returns of references
// among other things


#include <cmath>

#include <iostream>
#include <vector>
#include <random>

using std::vector;
using std::cout;
using std::endl;


class Vec : public vector<double>
{
private:
  vector<double> vec;
  int dim;

public:

  Vec();
  Vec(const Vec & other);
  Vec(const int dim);

  friend double operator * (const Vec & A, const Vec & B);
  friend Vec    operator + (const Vec & A, const Vec & B);
  friend Vec    operator - (const Vec & A, const Vec & B);

  //Note, vector division isnt well defined : consider a scalar multiplication of 1/ x * Vec if you want Vec/x

  Vec& operator *= (const double o);
  Vec& operator += (const Vec & other);
  Vec& operator -= (const Vec & other);
  
  Vec & operator = (const Vec & other);  

  friend Vec operator * (const double s, const Vec & v);
  friend Vec operator * (const Vec & v, const double s); //scalar multiplication

  friend std::ostream & operator << (std::ostream & os, const Vec & A);
};


std::default_random_engine gen;
std::normal_distribution<double> standard_normal(0.0, 1.0);






Vec::Vec(void) : vector<double>()
{
}

Vec::Vec(const Vec & other) : dim(other.dim)
{
  vector<double>::operator =(other);
}

Vec::Vec(const int dim) : vector<double>(dim), dim (dim)
{
}


Vec
operator +(const Vec & A, const Vec & B)
{
  Vec temp(A.dim);

  if (A.dim != B.dim)
    return temp; // have to refresh myself on exceptions in C++;

  cout << "vector addition between " << A << "\nand " << B << endl;
  
  for (int i = 0; i < A.dim; ++i)
    temp[i] = A[i] + B[i];
  
  return temp;
}


Vec 
operator - (const Vec & A, const Vec & B)
{
  Vec temp (A.dim);

  if (A.dim != B.dim) 
    return temp;
  cout << "vector subtraction between " << A << "\nand " << B << endl;

  for (int i = 0; i < A.dim; ++i)
    temp[i] = A[i] - B[i];
  
  return temp;
}


Vec  &
Vec::operator = (const Vec & other)
{
  vector<double>::operator =(other);
  dim = other.dim;
  return *this;
}


Vec & 
Vec::operator *= (const double s)
{
  for (int i = 0; i < this->dim; i++) 
   (*this)[i] = (*this)[i] * 5;

  return *this;
}


Vec &
Vec::operator += (const Vec & other)
{
  if (this->dim != other.dim)
    return *this;

  auto iter_other = other.cbegin();
  for (auto iter_this = this->begin();
       iter_this != this->end(); 
       ++iter_this, ++iter_other)
  {
    (*iter_this) += (*iter_other);
  }
  return *this;

}


Vec&
Vec::operator -= (const Vec & other)
{
  if (this->dim != other.dim) 
    return *this;
  
  auto iter_other = other.cbegin();
  for (auto iter_this = this->begin();
    iter_this != this->end(); 
    ++iter_this, ++iter_other)
  {
    (*iter_this) -= (*iter_other);
  }
  
  return *this;
}


std::ostream&
operator << (std::ostream & os, const Vec& A)
{
  os << "<";
  for (auto iter = A.cbegin(); iter != A.end(); ++iter)
  {
    os << (*iter) << ", "; // theres a better way to do this
  }
  os << ">" ;
  return os;
}


Vec 
operator * (const Vec & other, const double s)
{
 return s * other;
}


Vec 
operator * (const double s, const Vec & other)
{
  Vec temp; 
  
  for (int i = 0; i < other.dim; i++)
    temp.push_back(s * other[i]);
  return temp;
}

double
operator * (const Vec & A, const Vec & B)
{

  double dot = 0.0;

  if (A.dim != B.dim)
      return -1;
  cout << "vector inner product between A " << A << "\nand " << B << "\n(dot)" << endl;
  // note here how we arent using the begin, nor an integer for indexing: cbegin returns a const iterator, disallowing reassignment or statechange
  for (auto iterA = A.cbegin(), iterB = B.cbegin(); 
       iterA != A.end();
       ++iterA, ++iterB) 
  {
    dot += (*iterA) * (*iterB);
  }

  return dot;
}


/*
class LinearBase
{

private:
    Vec params;
    int DoF; // Degrees of Freedom, i.e. |regressors|
    double bias;

public:
  LinearBase(const int DoF);
  double operator() (const Vec & X) const; // 
  double L1Norm() const;
  double eval(const Vec &) const;
  double der();
};


double
randomN(void)
{
  return standard_normal(gen);
}


LinearBase::LinearBase(const int dof) : params(dof), dof(dof)
{
  bias = randomN();
  for (int i = 0; i < dof; ++i)
    params.push_back(randomN());
}




double 
LinearBase::L1Norm(void) const
{
  double accum = 0.0;
  for (auto iter = this->cbegin(); iter != this->end; ++iter)
    accum += (*iter);
  return accum;
}




double 
LinearBase::eval(const Vec & X) const
{
  return this->params * X + this->bias; // inner product with weights
}


double
LinearBase::operator() (const Vec & X) const
{
  return eval(X);
}

class LinearLasso: public LinearBase
{
private:
  double alpha;
public:
  LinearLasso(const int dof, const double alpha) : LinearBase(dof), alpha(alpha)
  {
    }
  double regularize() const;
  double der() const;
  double eval(const Vec &) const;
};


double
LinearLasso::eval(const Vec & X) const
{

  double err = LinearBase::eval(X);
  double total_err = reg(err);
  return total_err;
}


double
LinearLasso::regularize(const double err) const
{
  double accum = 0.0;
  for (auto iter = this->params.cbegin(); iter != this->params.end(); ++iter)
    accum += abs((*iter));
  return err + alpha * accum;
}



void
sign(const Vec & params, Vec & writeTo)
{

  for (auto iter = params.cbegin(); iter != params.end(); ++iter)
    writeTo.push_back(signbit(*iter));
} 




double
LinearLasso::der(void) const
{
  Vec derivative(dof);
  return alpha * sign(params, derivative);
}


class LinearRidge: public LinearBase
{
private:
  double alpha;
public:
  LinearRidge(const int dof, const double alpha) : LinearBase(dof), alpha(alpha){}
  double regularize() const;
  double der() const;
};


class LinearElasticNet: public LinearRidge, public LinearLasso
{
private: 
  double alpha;
  double beta;
public:
  LinearElasticNet(const int dof, const double alpha, const double beta) : 
    LinearLasso(dof, alpha), LinearRidge(dof, alpha) , beta(beta) 
  { }



  operator () (const vector<double> X) const; // calls eval
  double eval(const vector<double> X)  const; //  
  double regularize() const;
};









double
LinearRidge::regularize(const double err) const
{
  double reg_loss = err + alpha * (this->params * this.params) / 2;
  return reg_loss;
}

double 
LinearRidge::der(void) const
{
  return alpha * LinearBase::L1Norm();
}

*/
void
init(Vec & v, const int n_items)
{
  for (int i = 0; i < n_items; ++i)
  {
    double x = standard_normal(gen);
    cout << x << " ";
    v[i] = x;
  }

  cout << endl;
}

int main()
{
  Vec v1(9), v2(9);
  init(v1, 9);
  init(v2, 9);
  double outDot = v1 * v2;
  Vec outSub = v1 - v2;
  Vec outAdd = v1 + v2;
  Vec outScalarMul = 2.0 * v1;

  cout << "dot product from A.T B : " << outDot << endl;
  cout << "vector sum from A + B : " <<  outAdd << endl;
  cout << "vector subtraction from A - B : " <<  outSub << endl;
  cout << "vector scalar multiplication from 2.0 * A : " <<  outScalarMul << endl;

  v1 *= 5.0;
  cout << "v1 * 5.0 " << v1 << endl;
  return 0;      
}
