#include "spinecirclecenter.h"

#include <iostream>
using namespace std;

SPINECircleCenter::SPINECircleCenter()
: vnl_least_squares_function(3, 3,no_gradient)
{
}

void SPINECircleCenter::f(vnl_vector< double > const &x, vnl_vector< double > &fx){

    for(unsigned i = 0; i < this->CirclePoints.size(); i++){
        vnl_vector< double > a = (this->CirclePoints[i] - x);
        vnl_vector< double > b;
        if(i == this->CirclePoints.size()-1){
            b = (this->CirclePoints[0] - x);
        }else{
            b = (this->CirclePoints[i+1] - x);
        }
        fx[i] = a.magnitude() - b.magnitude();
    }
    cout<<fx<<endl;

}
