//maxmin finder
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

int main () {
double a,b,c;
double max=0.0;
double min=10.0;
double dev;
double devarr[17228][3];
string line;
int btemp;
int count=0;
vector<int> beams; 

ifstream reader("NodalDisplacements.Dat");
ofstream newfile("Nodaldeformations.Dat");
ifstream beamkeys("beamkeys.Dat");

while(getline(beamkeys,line)) {
sscanf(line.c_str(),"%d",&btemp);
beams.push_back(btemp);
}
cout<<beams.size()<<endl;
getline(reader,line);
getline(reader,line);
newfile<<line<<endl;
while(getline(reader,line)){
	if(line.find("v   ")==0){
	  sscanf(line.c_str(),"%*s %lf %lf %lf",&a,&b,&c);
			  dev=sqrt(a*a+b*b+c*c);
			  newfile<<a<<" "<<b<<" "<<c<<endl;
			  devarr[count][0]=a;
			  devarr[count][1]=b;
			  devarr[count][2]=c;
			  if(dev<min) {min= dev;}
			  if(dev>max) {max= dev;}
			  count++;
	  }
	if(line.find(" #nodes:")==0) {
			  for(int i=0;i<beams.size();i++){
			  newfile<<devarr[beams[i]][0]<<" "<<devarr[beams[i]][1]<<" "<<devarr[beams[i]][2]<<endl;
			  }
			  newfile<<line<<endl;
			  count=0;
	  }
}
newfile<<" #nodes:";
reader.close();
newfile.close();
cout<<"max ="<<max<<endl;
cout<<"min ="<<min<<endl;
return 0;
}