#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>

#define real float
#define uint unsigned int

struct real2 {
	real2(real a, real b) : x1(a), x2(b){}
	real2(){}

	real2 operator* (real s){
		return real2(this->x1*s, this->x2*s);
	}
	real2 operator+ (real2 a){
		return real2(a.x1+this->x1, a.x2+this->x2);
	}
	real2 operator- (real2 a){
		return real2(a.x1-this->x1, a.x2-this->x2);
	}
	real2 operator- (){
		return real2(-this->x1, -this->x2);
	}
	real2 operator/ (real a){
		return real2(this->x1/a, this->x2/a);
	}
	real length()
	{
		return sqrt(x1*x1+x2*x2);
	}

	real x1;
	real x2;
};

real dot(real2 a, real2 b)
{
	return a.x1*b.x1 + a.x2*b.x2;
}

real c = 10.0;

real ci(real i)
{
	return pow(c,(i-1.0)); 
}

real fc(real2 x)
{
	return x.x1*x.x1 + x.x2*x.x2*c;
}

real2 grad_fc(real2 x)
{
	return real2(2.0*x.x1, 2.0*x.x2*c);
}

real gc(real2 x)
{
	return 1.0 - exp(-fc(x));
}

real2 grad_gc(real2 x)
{
	return real2(2.0*x.x1*exp(-fc(x)), c*2.0*x.x2*exp(-fc(x)));
}

std::vector<real2> simple_grad_descent(
		real2 start, real alpha, real tolerance, real2 (*evalgrad)(real2))
{
	// x_{k+1} = x_k - alpha*grad_f(x_k)
	
	std::vector<real2> steps;
	steps.resize(1);
	steps[0] = start;

	real2 x = start;
	real2 xn;
	int counter = 0;
	do{
		xn = x - evalgrad(x)*alpha;
		if((x-xn).length() < tolerance)
			counter++;
		else
			counter=0;
		x = xn;
		steps.push_back(x);
	}while(counter < 10);

	return steps;
}

std::vector<real2>  functionvalue_functionevaluations;
int alternative = 0;

std::vector<real2> stepsize_adaptation_grad_descent(
		real2 start, real (*func)(real2), real2 (*grad)(real2),
		real tolerance, real ap=1.2, real am=0.5, real ls=0.01)
{
	real alpha = 1.0;
	std::vector<real2> steps;
	functionvalue_functionevaluations.clear();
	steps.resize(1);
	steps[0] = start;
	real2 x = start;
	real2 xn;

	int counter = 0;
	int evalcounter = 0;
	do{
		real2 g = grad(x);
		real2 d = -(g/fabs(g.length()));
		if(alternative)
			d = -g; 
		real2 da = d*alpha;

		while( func(x + da) > func(x) + ls*(dot(g, da)) )
		{
			alpha = am*alpha;
			da = d*alpha;
			evalcounter += 2;
		}
		xn = x + da;
		functionvalue_functionevaluations.push_back(real2(func(xn), real(evalcounter)));
		alpha = ap*alpha;
		if(alternative)
			alpha = 1.0; 
		da = d*alpha;

		//std::cout << fabs(da.length())  << std::endl;
		if(da.length() < tolerance)
			counter++;
		else
			counter=0;
		x = xn;
		steps.push_back(x);
	}while(counter < 10);

	return steps;
}

std::vector<real2> metric_grad_descent(
		real2 start, real alpha, real tolerance, real2 (*grad)(real2))
{
	// x_{k+1} = x_k - inverse(C)*alpha*grad_f(x_k)
	
	std::vector<real2> steps;
	steps.resize(1);
	steps[0] = start;

	real2 x = start;
	real2 xn;
	int counter = 0;
	do{
		real2 d = -grad(x)*alpha; 
		d.x2 /= c; // inverse(C) happens to be diag(1,1/c)
		xn = x + d;
		//std::cout << "distance " << (x-xn).length() << std::endl;
		if((x-xn).length() < tolerance)
			counter++;
		else
			counter=0;
		x = xn;
		steps.push_back(x);
	}while(counter < 10);

	return steps;
}


void print_grid_evaluation_to_file(
		const char* filename, int xd, int yd, real (*eval)(real2))
{
	std::ofstream file;
	file.open(filename);

	file << std::scientific;
	file << "# fc(x) " << std::endl;
	file << "# x1  x2  z " << std::endl;
	for(int i = 0; i<=xd*2; i++)
	{
		for(int j = 0; j<=yd*2; j++)
		{
			real x = -1.0 + real(i)/real(xd);
			real y = -1.0 + real(j)/real(yd);
			file << x << "  " << y << "  " << eval(real2(x,y)) << std::endl;
		}
		file << std::endl;
	}
	file.close();
}

void print_solution_path_to_file(
		const char* filename, std::vector<real2> xs, real (*func)(real2))
{
	std::ofstream file;
	file.open(filename);
	file << std::scientific;

	file << "# x1  x2  z " << std::endl;
	for(unsigned long i = 1; i<xs.size(); i++)
	{
		real2 x = xs[i];
		file 
			<< x.x1 << "  "
			<< x.x2 << "  " 
			<< func(x) << std::endl;
	}
	file.close();
}

int main()
{
	int xd = 20;
	int yd = 20;
	std::vector<real2> results;

	/* simple fixed stepsize gradient descent */
	print_grid_evaluation_to_file("fc_data.txt", xd, yd, fc);
	results = simple_grad_descent(real2(1.0,1.0), 0.07, 0.05, grad_fc);
	print_solution_path_to_file("fc_solution_simple.txt", results, fc);

	print_grid_evaluation_to_file("gc_data.txt", xd, yd, gc);
	results = simple_grad_descent(real2(1.0,1.0), 0.07, 0.05, grad_gc);
	print_solution_path_to_file("gc_solution_simple.txt", results, gc);

	/* $ make run; make simple_plots
	 * for results */

	/* gradient descent with backtracking */
	/* (a) */
	alternative = 0;
	results = stepsize_adaptation_grad_descent(real2(1.0,1.0), fc, grad_fc, 0.01);
	print_solution_path_to_file("fc_solution_backtracking.txt", results, fc);
	print_solution_path_to_file("fc_no_alternative_value_over_eval.txt", 
			functionvalue_functionevaluations, fc);
	/* sort of (b) already in here */
	alternative = 1;
	stepsize_adaptation_grad_descent(real2(1.0,1.0), fc, grad_fc, 0.01);
	print_solution_path_to_file("fc_with_alternative_value_over_eval.txt", 
			functionvalue_functionevaluations, fc);

	alternative = 0;
	results = stepsize_adaptation_grad_descent(real2(1.0,1.0), gc, grad_gc, 0.01);
	print_solution_path_to_file("gc_solution_backtracking.txt", results, gc);
	print_solution_path_to_file("gc_no_alternative_value_over_eval.txt", 
			functionvalue_functionevaluations, gc);
	alternative = 1; 
	stepsize_adaptation_grad_descent(real2(1.0,1.0), gc, grad_gc, 0.0001);
	print_solution_path_to_file("gc_with_alternative_value_over_eval.txt", 
			functionvalue_functionevaluations, gc);

	/* $ make run; make backtracking_plots; make function_value_eval_plots
	 * for results */

	/* (b) not sure how to plot all possible/interesting values for the 
	 * backtracking stop criterion 'ls'.
	 * ls controls our expectation for the next x in direction d*alpha
	 * to be at least by factor of 1/ls better than a simple step in direction 
	 * and length of the gradient.
	 *
	 * so the smaller ls is, the better we want our next step to be, but at the 
	 * same time we have to evaluate f(x) more often until we find a stepsize
	 * that suits our expextations.
	 *
	 * if we choose ls to be big (like 1.0), it will take less function evaluations 
	 * to find an acceptable stepsize, but may result in much more itertions to find
	 * the minimum, since we lowered our qualitiy standards for each step.
	 */

	/* (c) , but i am not going to count the number of grad(f(x)) evaluations.
	 * just look at the plots of the solution */
	results = metric_grad_descent(real2(1.0,1.0), 0.05, 0.001, grad_fc);
	print_solution_path_to_file("fc_solution_metric.txt", results, fc);

	results = metric_grad_descent(real2(1.0,1.0), 0.05, 0.000001, grad_gc);
	print_solution_path_to_file("gc_solution_metric.txt", results, gc);
	
	/* $ make run; make metric_plots
	 * for results */
	/* the metric takes longer than the simple strategy.
	 * i must be missing something here */


	return 0;
}
