/*
  Gaussweight class
  Author: Yi Hong, yihong@cs.unc.edu
  Date: 05/22/14
*/

#ifndef GAUSSWEIGHT_H
#define GAUSSWEIGHT_H
#include <math.h>
using namespace std;

class gaussweight{

public:
	
	static void computeNormalGaussian(const double x[], int nNum, double c, double sig, double wi[])
	{
		double sumWeight = 0;
		for(int i=0; i<nNum; i++) 
		{
			wi[i] = gaussfn(x[i], c, sig);
			sumWeight += wi[i];
		}
		for(int i=0; i<nNum; i++) wi[i] /= sumWeight;
	}
	
	static void computeModifiedGaussian(const double x[], int nNum, double c, double sig, double bdLow, double bdHigh, double wi[])
	{
		double sumWeight = 0;
		for(int i=0; i<nNum; i++) 
		{
			wi[i] = gaussfn(x[i], c, sig);
			if(x[i] < bdLow + sig)
			{
				wi[i] += gaussfn(2.0*bdLow - x[i], c, sig);
			}
			else if(x[i] > bdHigh - sig)
			{
				wi[i] += gaussfn(2.0*bdHigh - x[i], c, sig);
			}
			sumWeight += wi[i];
		}
		for(int i=0; i<nNum; i++) wi[i] /= sumWeight;
	}

	static double gaussfn(double x, double c, double sig) { 
		return exp(-pow(x-c, 2.0) / (2.0 * pow(sig, 2.0))); }
};

#endif // GAUSSWEIGHT_H