#include"model_n.h"
#include<iostream>
using namespace std;

int main(){
    ya::Model temp(R"(C:\Users\wzcin\CLionProjects\yaRenderer\models\diablo3_pose\diablo3_pose.obj)");
    cout<<temp.nverts()<<endl;


    return 0;
}