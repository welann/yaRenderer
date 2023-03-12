#include <iostream>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

int main()
{
  Matrix4d a;
  a << 1, 2,3,4,
       3, 4,1,2,
       2,3,1,4,
       2,2,3,4;

Vector4d b;
    b<<1, 2,3,4;

    cout<<a*b<<endl;

}