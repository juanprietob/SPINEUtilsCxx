/*
  Wfbplot class, head file
  Author: Yi Hong, yihong@cs.unc.edu
  Date: 05/22/14
*/
#ifndef WFBPLOT_H
#define WFBPLOT_H
#include <string.h>

#include <vector>
#include "matrix.h"
using namespace std;

class wfbplot{
	int nData;
	int pDim;
	matrix data;
	vector<double> x;
	vector<double> weight;
	double factor;
	string type;

public:
	// constructor
	wfbplot(const matrix&);
	// constructor, data, xAxis
	wfbplot(const matrix&, const double a[]);
	// constructor, nData, pDim, data, xAxis, weight
	wfbplot(const matrix&, const double a[], const double b[]);
	// set the data
	void setData(const matrix&);
	// set the x-axis
	void setXaxis(const double a[]);
	// set the weight
	void setWeight(const double a[]);
	// set the factor for boxplot, default 1.5
	void setFactor(const double a);
	// set the method, BD or MBD, default MBD
	void setType(const string a);
	
	// compute five curves, median, inf, sup, minimal boundary and maximal boundary
	void computeBoxplot(double med[], double inf[], double sup[], double minBd[], double maxBd[], double depth[]);

	void print();
	void printVector(int nNum, double value[]);
	void printVector(int nNum, vector<double> value);
	void printVector(int nNum, int value[]);

private: 
	int computeDepth(double depth[]);
	int computeDepthBD2(double depth[]);
	int computeDepthMBD(double depth[]);
	void computeProportionWithinBand(int c1, int c2, double prop[]);
	void computeIndicatorWithinBand(int c1, int c2, double prop[]);
	void sortDepth(const double depth[], int index[]);
	void locateCenterRegionAndFences(int index[], int& indCenter, int& indFence);
	void countOutliers(const double inf[], const double sup[], const int index[], const int indFence, vector<int> &indInsider);
};
#endif // WFBPLOT_H
