/*
  Matrix class, head file
  Author: Yi Hong, yihong@cs.unc.edu
  Date: 05/22/14
*/

#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
using namespace std;

class matrix {

public:
	int row;
	int col;
	double **array;

public: 
	matrix();
	matrix(int, int);
	matrix(const matrix&);
	~matrix();
	void copyMatrix(const matrix&);
	void setElement(int r, int c, double e);
	double getElement(int r, int c);
	int setColumn(int c, const double a[]);
	int getColumn(int c, double a[]);
	int setRow(int r, const double a[]);
	int getRow(int r, double a[]);
	matrix getColumns(int nNum, const int c[]);
	matrix getRows(int nNum, const int  c[]);
	matrix operator+(const matrix&);
	matrix& operator+=(const matrix&);
	matrix operator-(const matrix&);
	matrix& operator-=(const matrix&);
	matrix operator*(const double v);
	matrix& operator*=(const double v);
	void minAtEachRow(double minValue[]);
	void maxAtEachRow(double maxValue[]);
	void minAtEachColumn(double minValue[]);
	void maxAtEachColumn(double maxValue[]);
	void print();
};

#endif // MATRIX_H
