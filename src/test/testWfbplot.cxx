/*
  Main function for testing weight functional boxplot
  Author: Yi Hong, yihong@cs.unc.edu
  Date: 05/22/14
*/

#include <stdlib.h>
#include "wfbplot.h"
#include "gaussweight.h"
using namespace std;

int main()
{
	cout << "Test weighted functional boxplot" << endl;

	int i, j;
	
	// generate age
	int nData = 30;
	double ageRange[2] = {0, 200};
	double ages[nData];
	for(i=0; i<nData; i++) 
	{
		ages[i] = 30 + (180-30)/30*i;
		cout << ages[i] << " ";
	}
	cout << endl;
	
	// generate data
	int pDim = 101;
	double xAxis[pDim];
	for(i=0; i<pDim; i++)
	{
		xAxis[i] = i*0.01;
	}
	
	matrix data(nData, pDim);
	for(i=0; i<nData; i++)
	{
		for(j=0; j<pDim; j++)
		{
			double value = 500 * (1 + sin(2*3.14*xAxis[j] + 1.57*(i+1)/5.0)) + ages[i]*2.0;
			data.setElement(i, j, value);
		}
	}

	wfbplot wfbTest(data);
	wfbTest.print();

	// built atlas using weighted functional boxplot
	double sigma = 30;
	for(int id=0; id < nData; id++)
	{
		double weight[nData];
		// compute weight
		gaussweight::computeModifiedGaussian( ages, nData, ages[id], sigma, ageRange[0], ageRange[1], weight);
		wfbTest.setWeight(weight);
		wfbTest.printVector(nData, weight);
		double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
		// call weight functional boxplot function
		wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);
		wfbTest.printVector(nData, depth);
		wfbTest.printVector(pDim, med);
		wfbTest.printVector(pDim, inf);
		wfbTest.printVector(pDim, sup);
		wfbTest.printVector(pDim, minBd);
		wfbTest.printVector(pDim, maxBd);
	}
	
	return EXIT_SUCCESS;
}


