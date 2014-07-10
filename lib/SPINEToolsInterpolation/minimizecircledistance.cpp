#include "minimizecircledistance.h"
#include <math.h>
#include <vtkMath.h>

MinimizeCircleDistance::MinimizeCircleDistance()
    : vnl_least_squares_function(1, 1,no_gradient)
{
    this->Source = 0;
    this->Target = 0;
}

void MinimizeCircleDistance::f(vnl_vector< double > const &x, vnl_vector< double > &fx){
    if(this->Source && this->Target){

        double angle = x[0];

        while(angle < 0){
            angle += 2*M_PI;
        }
        while(angle > 2*M_PI){
            angle -= 2*M_PI;
        }

        double delta = angle/(2*M_PI) * (this->Source->GetNumberOfPoints()-1);

        if(angle >= 0 && angle <= 2*M_PI){
            fx[0] = 0;

            for(int i = 0; i < this->Source->GetNumberOfPoints(); i++){
                double sourceindex = delta + i;

                int targetindex = round(i/(this->Source->GetNumberOfPoints()-1) * (this->Target->GetNumberOfPoints()-1));

                double ps[3], ps0[3], ps1[3], pt[3];

                int sourceindexf = floor(sourceindex);
                double sourceindexres = sourceindex-sourceindexf;

                this->Source->GetPoint(sourceindexf, ps0);
                this->Source->GetPoint(sourceindexf+1, ps1);
                vtkMath::MultiplyScalar(ps0, 1-sourceindexres);
                vtkMath::MultiplyScalar(ps1, sourceindexres);
                vtkMath::Add(ps0, ps1, ps);

                this->Target->GetPoint(targetindex, pt);
                fx[0] += vtkMath::Distance2BetweenPoints(pt, ps);
            }
            cout<<"angle="<<angle<<endl;
            cout<<fx<<endl;
        }else{
            fx[0] = 999999999;
        }
    }
}
