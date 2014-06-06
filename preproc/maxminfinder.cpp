//maxmin finder
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

int main () {
double a,b,c;
double max=0.0;
double min=10.0;
double dev;
string line;
ifstream reader2("Modedeform.Dat");
while(getline(reader2,line)){
if(line.find(" #nodes:")!=0){
	  sscanf(line.c_str(),"%lf %lf %lf",&a,&b,&c);
//				cout<<"	"<<a<<"		"<<b<<"		"<<c<<endl;
			  dev=sqrt(a*a+b*b+c*c);
			  if(dev<min) {min= dev;}
			  if(dev>max) {max= dev;}
	  }
}
cout<<"max ="<<max<<endl;
cout<<"min ="<<min<<endl;
return 0;
}