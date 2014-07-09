#ifndef SPINECIRCLECENTER_H
#define SPINECIRCLECENTER_H

#include "vnl/vnl_least_squares_cost_function.h"
#include <vector>


using namespace std;

class SPINECircleCenter : public vnl_least_squares_function
{
public:
    SPINECircleCenter();
    virtual void f(vnl_vector< double > const &x, vnl_vector< double > &fx);

    void SetPoints(vector< vnl_vector<double> > p){
        this->CirclePoints = p;
    }
private:
    vector<vnl_vector< double > > CirclePoints;

};

#endif // SPINECIRCLECENTER_H
