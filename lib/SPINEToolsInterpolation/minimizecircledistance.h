#ifndef MINIMIZECIRCLEDISTANCE_H
#define MINIMIZECIRCLEDISTANCE_H

#include "vtkPolyData.h"
#include "vnl/vnl_least_squares_cost_function.h"
#include <vector>

class MinimizeCircleDistance : public vnl_least_squares_function
{
public:

    virtual void f(vnl_vector< double > const &x, vnl_vector< double > &fx);

    void SetSource(vtkPolyData *source){
        this->Source = source;
    }

    void SetTarget(vtkPolyData *target){
        this->Target = target;
    }

    MinimizeCircleDistance();

private:
    vtkPolyData* Source;
    vtkPolyData* Target;
};

#endif // MINIMIZECIRCLEDISTANCE_H
