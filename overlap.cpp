#include <iostream>
#include <vector>
#include <math.h>
#include "overlap.h"
#include <cstdlib>
#include <armadillo>
#include "utils.h"
#include "BasisSet.h"
# define M_PIl          3.141592653589793238462643383279502884

//overlap between pair of gaussians
double gauss_integral(double exp_a,std::array<size_t, 3> cart_a,std::array<double,3> coord_a,
		double exp_b,std::array<size_t, 3> cart_b, std::array<double,3> coord_b)
{
	std::vector<double> integrals;
	for(int i=0;i<3;i++)
	{
		double dist = coord_a[i]-coord_b[i];
		double res = mcmurchie_davidson(exp_a,exp_b,0,cart_a[i],cart_b[i],dist);
		integrals.push_back(res);
	}
	double int_tot = 1.0;
	for (unsigned int i=0;i<integrals.size();i++)
	{
		int_tot*=integrals[i];
	}
	return int_tot*pow(M_PIl/(exp_a+exp_b),1.5);
}

//recursive formula for cartesian overlap distributions
double mcmurchie_davidson(double exp_a, double exp_b, int t, int angmom_a, int angmom_b, double dist)
{
	double p = exp_a + exp_b;
	double q = exp_a*exp_b/p;
	if (t<0 || t>(angmom_a+angmom_b))
	{
		return 0.0;
	}
	else if(angmom_a==angmom_b && angmom_b==0)
	{
		return exp(-q*dist*dist);
	}
	else if(angmom_b==0)
	{
		//decrement angmom_a
		return(
				(1/(2*p))*mcmurchie_davidson(exp_a,exp_b,t-1,angmom_a-1,angmom_b,dist)   -
				(q*dist/exp_a)*mcmurchie_davidson(exp_a,exp_b,t,angmom_a-1,angmom_b,dist) +
				(t+1)*mcmurchie_davidson(exp_a,exp_b,t+1,angmom_a-1,angmom_b,dist));
	}
	else
	{
		//decrement angmom_b
		return(
				(1/(2*p))*mcmurchie_davidson(exp_a,exp_b,t-1,angmom_a,angmom_b-1,dist)+
				(q*dist/exp_b)*mcmurchie_davidson(exp_a,exp_b,t,angmom_a,angmom_b-1,dist)+
				(t+1)*mcmurchie_davidson(exp_a,exp_b,t+1,angmom_a,angmom_b-1,dist));
	}
}

//overlap between two normalized, contracted gaussians
double overlap_integral(Shell a, std::array<size_t,3> cart_a, Shell b,
		std::array<size_t,3> cart_b)
{
	double res = 0.0;
	for(size_t i=0;i<a.num_prims;i++)
	{
		for(size_t j=0;j<b.num_prims;j++)
		{
			res+=a.coeffs[i]*b.coeffs[j]*
			gauss_integral(a.exps[i],cart_a,a.origin,b.exps[j],cart_b,b.origin);
		}
	}
	if (abs(res-0.0091)<1E-5)
	{
		std::cout << "A" << std::endl;
		for (size_t a:cart_a)
			std::cout << a << ",";
		std::cout<< std::endl;
		std::cout << "B" << std::endl;
		for (size_t b:cart_b)
			std::cout << b << ",";
		std::cout<< std::endl;
	}
	return res;
}

void shell_overlap(Shell shell_a, Shell shell_b,arma::subview<double>&sub_mat)
{
	std::vector<std::array<size_t,3>> order_a = libcap_carts_ordering(shell_a);
	std::vector<std::array<size_t,3>> order_b = libcap_carts_ordering(shell_b);
	for(size_t i=0;i<shell_a.num_carts();i++)
	{
		std::array<size_t,3> a_cart = order_a[i];
		for(size_t j=0;j<shell_b.num_carts();j++)
		{
			std::array<size_t,3> b_cart = order_b[j];
			sub_mat(i,j) = overlap_integral(shell_a,a_cart, shell_b, b_cart);
		}
	}
}

void compute_analytical_overlap(BasisSet bs, arma::mat &Smat)
{
	unsigned int row_idx = 0;
	for(auto&shell1:bs.basis)
	{
		unsigned int col_idx = 0;
		for(auto&shell2: bs.basis)
		{
			//view to block of the matrix corresponding to these two pairs of basis functions
			auto sub_mat = Smat.submat(row_idx,col_idx,
					row_idx+shell1.num_carts()-1,col_idx+shell2.num_carts()-1);
            shell_overlap(shell1,shell2,sub_mat);
            col_idx += shell2.num_carts();
		}
		row_idx += shell1.num_carts();
	}
}



