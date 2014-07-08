#include <vtkIterativeClosestPointTransform.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

#include <iostream>

using namespace std;

int main(int argv, char** argc){
	
	string filenamesource = "";

	for(int i = 0; i < argv - 1; i++){
		if(string(argc[i]).compare(-s) == 0){
			filenamesource = string(argc[i]);
		}else if(string(argc[i]).compare(-s) == 0){
			filenametarget = string(argc[i]);
		}
	}	

	vtkSmartPointer<vtkPolyDataReader> sourcereader = vtkSmartPointer<vtkPolyDataReader>::New();
	sourcereader->Set



	vtkSmartPointer<vtkPolyDataReader> sourcereader = vtkSmartPointer<vtkPolyDataReader>::New();



	vtkSmartPointer<vtkIterativeClosestPointTransform> closestpoint = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();




}