/*
  Matrix class, cpp file
  Author: Yi Hong, yihong@cs.unc.edu
  Date: 05/22/14
*/

#include "matrix.h"

matrix::matrix()
{
	row = 0;
	col = 0;
}

matrix::matrix(int r, int c)
{
	row = r;
	col = c;
	array = new double*[row];
	for(int i=0; i<row; i++)
	{
		array[i] = new double[col];
	}
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			array[i][j] = 0;
		}
	}
}

matrix::matrix(const matrix& m)
{
	row = m.row;
	col = m.col;
	array = new double*[row];
	for(int i=0; i<row; i++)
	{
		array[i] = new double[col];
	}
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			array[i][j] = m.array[i][j];
		}
	}
}

void matrix::copyMatrix(const matrix& m)
{
	if(row == 0 && col == 0)
	{
		row = m.row;
		col = m.col;
		array = new double*[row];
		for(int i=0; i<row; i++)
		{
			array[i] = new double[col];
		}
	}
	if(row == m.row && col == m.col)
	{
		for(int i=0; i<row; i++)
		{
			for(int j=0; j<col; j++)
			{
				array[i][j] = m.array[i][j];
			}
		}
	}
	else cerr << "Sizes do not match." << endl;
}


matrix::~matrix()
{
	for(int i=0; i<row; i++)
	{
		delete[] array[i];
	}
	delete[] array;
}

void matrix::setElement(int r, int c, double e)
{
	array[r][c] = e;
}

double matrix::getElement(int r, int c)
{
	return array[r][c];
}

int matrix::setColumn(int c, const double a[])
{
	if(c < 0 || c >= col)
	{
		cerr << "Outside of the index range." << endl;
		return 0;
	}
	for(int i=0; i<row; i++)
	{
		array[i][c] = a[i];
	}
	return 1;
}

int matrix::getColumn(int c, double a[])
{
	if(c < 0 || c >= col)
	{
		cerr << "Outside of the index range." << endl;
		return 0;
	}
	for(int i=0; i<row; i++)
	{
		a[i] = array[i][c];
	}
	return 1;
}

int matrix::setRow(int r, const double a[])
{
	if(r < 0 || r >= row)
	{
		cerr << "Outside of the index range." << endl;
		return 0;
	}
	for(int i=0; i<col; i++)
	{
		array[r][i] = a[i];
	}
}

int matrix::getRow(int r, double a[])
{
	if(r < 0 || r >= row)
	{
		cerr << "Outside of the index range." << endl;
		return 0;
	}
	for(int i=0; i<col; i++)
	{
		a[i] = array[r][i];
	}
	return 1;
}

matrix matrix::getColumns(int nNum, const int c[])
{
	matrix subMatrix(row, nNum);
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<nNum; j++)
		{
			subMatrix.array[i][j] = array[i][c[j]];
		}
	}
	return subMatrix;
}

matrix matrix::getRows(int nNum, const int c[])
{
	matrix subMatrix(nNum, col);
	for(int i=0; i<nNum; i++)
	{
		for(int j=0; j<col; j++)
		{
			subMatrix.array[i][j] = array[c[i]][j];
		}
	}
	return subMatrix;
}

matrix& matrix::operator+=(const matrix& m)
{
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			array[i][j] += m.array[i][j];
		}
	}
	return *this;
}

matrix matrix::operator+(const matrix& m)
{
	if(row != m.row || col != m.col)
	{
		cerr << "Matrix sizes do not match.";
		return (*this);
	}
	matrix new_mat(*this);
	return (new_mat += m);
}

matrix& matrix::operator-=(const matrix& m)
{
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			array[i][j] -= m.array[i][j];
		}
	}
	return (*this);
}

matrix matrix::operator-(const matrix& m)
{
	if(row != m.row || col != m.col)
	{
		cerr << "Matrix sizes do not match." << endl;
		return (*this);
	}
	matrix new_mat(*this);
	return (new_mat -= m);
}

matrix& matrix::operator*=(const double v)
{
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			array[i][j] *= v;
		}
	}
	return (*this);
}

matrix matrix::operator*(const double v)
{
	matrix new_mat(*this);
	return (new_mat*=v);
}

void matrix::minAtEachRow(double minValue[])
{
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			if(j == 0 || minValue[i] > array[i][j]) 
				minValue[i] = array[i][j];
		}
	}
}

void matrix::maxAtEachRow(double maxValue[])
{
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			if(j == 0 || maxValue[i] < array[i][j])
				maxValue[i] = array[i][j];
		}
	}
} 

void matrix::minAtEachColumn(double minValue[])
{
	for(int j=0; j<col; j++)
	{
		for(int i=0; i<row; i++)
		{
			if(i == 0 || minValue[j] > array[i][j])
				minValue[j] = array[i][j];
		}
	}
}

void matrix::maxAtEachColumn(double maxValue[])
{
	for(int j=0; j<col; j++)
	{
		for(int i=0; i<row; i++)
		{
			if(i == 0 || maxValue[j] < array[i][j])
				maxValue[j] = array[i][j];
		}
	}
}

void matrix::print()
{
	cout << row << " X " << col << endl;
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			cout << array[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
