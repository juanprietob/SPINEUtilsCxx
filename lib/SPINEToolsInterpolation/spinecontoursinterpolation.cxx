/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SPINEContoursInterpolation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "spinecontoursinterpolation.h"


#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"

#include "vtkContourTriangulator.h"
#include "vtkPolyDataNormals.h"
#include "vtkGenericCell.h"
#include "vtkTriangle.h"

vtkStandardNewMacro(SPINEContoursInterpolation);

using namespace std;

SPINEContoursInterpolation::SPINEContoursInterpolation()
{
    this->MaximumCurveError = 0.1;
    this->ContourLength = 0;
    this->NumberOfPoints = -1;
    AvgNormal = new double[3];
    AvgNormal[0] = 0;
    AvgNormal[1] = 0;
    AvgNormal[2] = 0;
}

//--------------------------------------------------------------------------
SPINEContoursInterpolation::~SPINEContoursInterpolation()
{
    if(AvgNormal){
        delete AvgNormal;
    }
}

//--------------------------------------------------------------------------
int SPINEContoursInterpolation::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints* contourpoints = input->GetPoints();
  vtkSmartPointer<vtkPoints> outpoints = vtkSmartPointer<vtkPoints>::New();


  if(contourpoints->GetNumberOfPoints() < 3){
      vtkErrorMacro("Cannot interpolate line. Contour points < 3")
  }

  for(int i = 0; i < contourpoints->GetNumberOfPoints(); i++){

      double p1[3];
      double p2[3];
      double p3[3];
      double p4[3];
      double pinter[3];


      contourpoints->GetPoint(i, p1);
      this->getDerivative(i, contourpoints, p2);
      vtkMath::MultiplyScalar(p2, 1/2.0);
      vtkMath::Add(p1, p2, p2);

      if(i == contourpoints->GetNumberOfPoints() - 1){
          contourpoints->GetPoint(0, p4);
          this->getDerivative(0, contourpoints, p3);
      }else{
          contourpoints->GetPoint(i+1, p4);
          this->getDerivative(i+1, contourpoints, p3);
      }
      vtkMath::MultiplyScalar(p3, 1/2.0);
      vtkMath::Subtract(p4, p3, p3);

      double stept = 0;
      double distance = 0;

      do{
          stept += 1;
          this->getInterpolatedPoint(p1, p2, p3, p4, pinter, 1/stept);
          distance = sqrt(vtkMath::Distance2BetweenPoints(p1, pinter));
      }while(distance > this->MaximumCurveError);


      for(double t = 0; t < 1.0; t+=1/stept){
          this->getInterpolatedPoint(p1, p2, p3, p4, pinter, t);
          outpoints->InsertNextPoint(pinter[0], pinter[1], pinter[2]);
      }
  }

    //vtkSmartPointer<vtkDataArray> normals = vtkSmartPointer<vtkDoubleArray>::New();
    //normals->SetNumberOfComponents(3);

    vtkSmartPointer<vtkCellArray> outcellarray = vtkSmartPointer<vtkCellArray>::New();
    this->ContourLength = 0;
    for(unsigned i = 0; i < outpoints->GetNumberOfPoints(); i++){
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        line->GetPointIds()->SetId(0, i);

        double p0[3], p1[3];
        //double dp0[3], dp1[3];
        outpoints->GetPoint(i, p0);
        //this->getDerivative(i, outpoints, dp0);

        if(i == outpoints->GetNumberOfPoints() - 1){
        line->GetPointIds()->SetId(1, 0);
        outpoints->GetPoint(0, p1);
        //this->getDerivative(0, outpoints, dp1);
        }else{
          line->GetPointIds()->SetId(1, i+1);
          outpoints->GetPoint(i+1, p1);
          //this->getDerivative(i+1, outpoints, dp1);
        }
        this->ContourLength += sqrt(vtkMath::Distance2BetweenPoints(p0, p1));
        outcellarray->InsertNextCell(line);

    }

    vtkSmartPointer<vtkPolyData> contourpoly = vtkSmartPointer<vtkPolyData>::New();
    contourpoly->SetPoints(outpoints);
    contourpoly->SetLines(outcellarray);

    vtkSmartPointer<vtkContourTriangulator> poly = vtkSmartPointer<vtkContourTriangulator>::New();
    poly->SetInputData(contourpoly);
    poly->SetOutput(output);
    poly->Update();

    vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
    Area = 0;
    for(int i = 0; i < output->GetNumberOfCells(); i++){
      output->GetCell(i, cell);
      vtkIdList* pointids = cell->GetPointIds();
      double p[3][3];
      for(unsigned j = 0; j < pointids->GetNumberOfIds() && j < 3; j++){
        output->GetPoint(pointids->GetId(j), p[j]);
      }
      Area += vtkTriangle::TriangleArea(p[0], p[1], p[2]);
    }

    vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();

    normalGenerator->SetInputConnection(poly->GetOutputPort());
    //normalGenerator->SetOutput(output);
    normalGenerator->ComputePointNormalsOn();
    normalGenerator->ComputeCellNormalsOn();
    normalGenerator->Update();

    AvgNormal[0] = 0;
    AvgNormal[1] = 0;
    AvgNormal[2] = 0;

    vtkDataArray* normals = normalGenerator->GetOutput()->GetPointData()->GetNormals();
    for(int i = 0; i < normals->GetNumberOfTuples(); i++){
        vtkMath::Add(AvgNormal, normals->GetTuple3(i), AvgNormal);
    }
    vtkMath::MultiplyScalar(AvgNormal, 1.0/((double) normals->GetNumberOfTuples()));
    vtkMath::Normalize(AvgNormal);


    return 1;
}

void SPINEContoursInterpolation::getInterpolatedPoint(const double *p1, const double *p2, const double *p3, const double *p4, double *p, double t){

    for(unsigned i = 0; i < 3; i++){
        p[i] = p1[i] * pow(1 - t, 3) + 3 * p2[i] * t * pow(1 - t, 2) + 3 * p3[i] * pow(t, 2) * (1 - t) + p4[i] * pow(t, 3);
    }
}

void SPINEContoursInterpolation::getDerivative(int i, vtkPoints* points, double* dp0){
    double p0[3];
    double p1[3];

    double der = 1/2.0;

    if(i == 0){
        points->GetPoint(points->GetNumberOfPoints() - 1, p0);
    }else{
        points->GetPoint(i-1, p0);
    }
    if(i == points->GetNumberOfPoints() - 1){
        points->GetPoint(0, p1);
    }else{
        points->GetPoint(i+1, p1);
    }

    vtkMath::Subtract(p1, p0, dp0);
    vtkMath::MultiplyScalar(dp0, der);


}

void SPINEContoursInterpolation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

