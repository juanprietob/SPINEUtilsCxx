/*
  Wfbplot class, cpp file
  Author: Yi Hong, yihong@cs.unc.edu
  Date: 05/22/14
*/

#include "wfbplot.h"

struct dataPair{
	double dep;
	int ind;
};
struct comparator{
	bool operator()(const dataPair& left, const dataPair& right){
	return left.dep > right.dep;
	}
};

wfbplot::wfbplot(const matrix& m)
{
	int i;
	nData = m.row;
	pDim = m.col;
	data.copyMatrix(m);
	x.resize(pDim);
	for(i=0; i<pDim; i++)
	{
		x[i] = (double)i/(pDim-1);
	}
	weight.resize(nData);
	for(i=0; i<nData; i++)
	{
		weight[i] = 1.0/nData;
	}
	factor = 1.5;
	type = "MBD";
}

wfbplot::wfbplot(const matrix& m, const double a[])
{
	int i;
	nData = m.row;
	pDim = m.col;
	data.copyMatrix(m);
	x.resize(pDim);
	for(i=0; i<pDim; i++)
	{
		x[i] = a[i];
	}
	weight.resize(nData);
	for(i=0; i<nData; i++)
	{
		weight[i] = 1.0/nData;
	}
	factor = 1.5;
	type = "MBD";
}

wfbplot::wfbplot(const matrix& m, const double a[], const double b[])
{
	int i;
	nData = m.row;
	pDim = m.col;
	data.copyMatrix(m);
	x.resize(pDim);
	for(i=0; i<pDim; i++)
	{
		x[i] = a[i];
	}

	// weight should be normalized
	weight.resize(nData);
	double sum = 0;
	for(i=0; i<nData; i++)
	{
		weight[i] = b[i];
		sum += weight[i];
	}
	if(sum != 1)
	{
		if(sum == 0) cerr << "The sum of weights is 0." << endl;
		for(i=0; i<nData; i++)
		{
			weight[i] /= sum;
		}
	}
	factor = 1.5;
	type = "MBD";
}

void wfbplot::setData(const matrix& m)
{
	data.copyMatrix(m);
}

void wfbplot::setXaxis(const double a[])
{
	for(int i=0; i<pDim; i++)
	{
		x[i] = a[i];
	}
}

void wfbplot::setWeight(const double a[])
{
	double sum = 0; 
	int i;
	for(i=0; i<nData; i++)
	{
		weight[i] = a[i];
		sum += weight[i];
	}
	// normalize the weight
	if(sum != 1)
	{
		for(i=0; i<nData; i++) weight[i] /= sum;
	}
}

void wfbplot::setFactor(const double a)
{
	factor = a;
}

void wfbplot::setType(const string a)
{
	type = a;
}

int wfbplot::computeDepth(double depth[])
{
	if(type.compare("BD2") == 0)
		return computeDepthBD2(depth);
	if(type.compare("MBD") == 0)
		return computeDepthMBD(depth);
	cerr << type << " , Unknow method" << endl;
	return 0;
}

void wfbplot::sortDepth(const double depth[], int index[])
{
	int i;
	vector<dataPair> items;
	for(i=0; i<nData; i++)
	{
		dataPair tmpPair;
		tmpPair.dep = depth[i];
		tmpPair.ind = i;
		items.push_back(tmpPair);
	}
	sort(items.begin(), items.end(), comparator());
	for(i=0; i<nData; i++)
	{
		index[i] = items[i].ind;
	}
}

void wfbplot::locateCenterRegionAndFences(int index[], int& indCenter, int& indFence)
{
	double sumWeight = 0;
	bool indCenterFlag = false;
	bool indFenceFlag = false;
	for(int i=0; i<nData; i++)
	{
		sumWeight += weight[index[i]];
		if(sumWeight >= 0.993 && !indFenceFlag)
		{
			indFence = i;
			indFenceFlag = true;
		}
		else if(sumWeight >= 0.5 && !indCenterFlag)
		{
			indCenter = i;
			indCenterFlag = true;
		}
	}
}

void wfbplot::computeBoxplot(double med[], double inf[], double sup[], double minBd[], double maxBd[], double depth[])
{
	int indCenter, indFence, i, j;

	// compute and sort depth
	int index[nData];
	computeDepth(depth);
	sortDepth(depth, index);

	// compute inf and sup
	locateCenterRegionAndFences(index, indCenter, indFence);
	data.getRow(index[0], med);
	int indexTmp[indCenter+1];
	for(i=0; i<indCenter+1; i++) indexTmp[i] = index[i];
	matrix matrixCenter = data.getRows(indCenter+1, indexTmp);
	matrixCenter.minAtEachColumn(inf);
	matrixCenter.maxAtEachColumn(sup);

	// compute outliers
	vector<int> indInsider;
	countOutliers(inf, sup, index, indFence, indInsider);
	int nInsider = indInsider.size();
	int indTmp[nInsider];
	for(i=0; i<nInsider; i++) indTmp[i] = indInsider[i];
	matrix matrixInsider = data.getRows(nInsider, indTmp);
	matrixInsider.minAtEachColumn(minBd);
	matrixInsider.maxAtEachColumn(maxBd);
}

void wfbplot::countOutliers(const double inf[], const double sup[], const int index[], const int indFence, vector<int> &indInsider)
{
	int i, j;
	double dist[pDim], upper[pDim], lower[pDim];
	for(i=0; i<pDim; i++) dist[i] = factor * (sup[i] - inf[i]);
	for(i=0; i<pDim; i++) 
	{ 
		upper[i] = sup[i] + dist[i];
		lower[i] = inf[i] - dist[i];
	}
	bool outly[nData];
	int nOutly = 0;
	for(i=0; i<nData; i++)
	{
		outly[i] = false;
		for(j=0; j<pDim; j++)
		{
			if(data.array[i][j] < lower[j] || data.array[i][j] > upper[j])
			{
				outly[i] = true;
				nOutly++;
				break;
			}
		}
	}
	for(i=indFence+1; i<nData; i++) 
	{
		if(outly[index[i]] == false)
		{
			outly[index[i]] = true;
			nOutly++;
		}
	}
	int nInsider = nData - nOutly;
	indInsider.resize(0);
	for(i=0; i<nData; i++)
	{
		if(outly[i] == true) continue;
		indInsider.push_back(i);
	}
}

void wfbplot::computeIndicatorWithinBand(int c1, int c2, double prop[])
{
	int nNum = 2;
	int nInd[2] = {c1, c2};
	matrix mat = data.getRows(nNum, nInd);
	double inf[pDim], sup[pDim], dat[pDim];
	mat.minAtEachColumn(inf);
	mat.maxAtEachColumn(sup);
	for(int i=0; i<nData; i++)
	{
		data.getRow(i, dat);
		int nCount = 1;
		for(int j=0; j<pDim; j++)
		{
			if(dat[j] > sup[j] || dat[j] < inf[j]) 
			{
				nCount = 0;
				break;
			}
		}
		prop[i] = (double)nCount / pDim;
	}	
}

int wfbplot::computeDepthBD2(double depth[])
{
	double sumWeight = 0;
	int i, j, k;
	for(k=0; k<nData; k++)
	{
		depth[k] = 0;
	}
	for(i=0; i<nData-1; i++)
	{
		for(j=i+1; j<nData; j++)
		{
			double prop[nData];
			computeIndicatorWithinBand(i, j, prop);
			double wij = weight[i] * weight[j];
			for(k=0; k<nData; k++) 
			{
				depth[k] += (wij * prop[k]);
			}
			sumWeight += wij;
		}
	}
	for(k=0; k<nData; k++) depth[k] /= sumWeight;
}

void wfbplot::computeProportionWithinBand(int c1, int c2, double prop[])
{
	int nNum = 2;
	int nInd[2] = {c1, c2};
	matrix mat = data.getRows(nNum, nInd);
	double inf[pDim], sup[pDim], dat[pDim];
	mat.minAtEachColumn(inf);
	mat.maxAtEachColumn(sup);
	for(int i=0; i<nData; i++)
	{
		data.getRow(i, dat);
		int nCount = 0;
		for(int j=0; j<pDim; j++)
		{
			if(dat[j] <= sup[j] && dat[j] >= inf[j]) nCount++;
		}
		prop[i] = (double)nCount / pDim;
	}	
}

int wfbplot::computeDepthMBD(double depth[])
{
	double sumWeight = 0;
	int i, j, k;
	for(k=0; k<nData; k++)
	{
		depth[k] = 0;
	}
	for(i=0; i<nData-1; i++)
	{
		for(j=i+1; j<nData; j++)
		{
			double prop[nData];
			computeProportionWithinBand(i, j, prop);
			double wij = weight[i] * weight[j];
			for(k=0; k<nData; k++) 
			{
				depth[k] += (wij * prop[k]);
			}
			sumWeight += wij;
		}
	}
	for(k=0; k<nData; k++) depth[k] /= sumWeight;
}

void wfbplot::print()
{
	int i;
	cout << "data size: " << nData << " x " << pDim << endl;
	data.print();
	cout << "X-axis: " << endl;
	printVector(pDim, x);
	cout << "Weight: " << endl;
	printVector(nData, weight);
	cout << "Method's type: " << type << endl;
	cout << "Factor: " << factor << endl;
}

void wfbplot::printVector(int nNum, double value[])
{
	for(int i=0; i<nNum; i++) cout << value[i] << " ";
	cout << endl;
}

void wfbplot::printVector(int nNum, vector<double> value)
{
	for(int i=0; i<nNum; i++) cout << value[i] << " ";
	cout << endl;
}

void wfbplot::printVector(int nNum, int value[])
{
	for(int i=0; i<nNum; i++) cout << value[i] << " ";
	cout << endl;
}

