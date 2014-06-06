//mode generator
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
string myline;
int btemp;
int count=0;
vector<int> beams; 
bool flag= true;

ifstream reader("FEM_Output_10modes .txt");
ofstream newfile("Modedeform.Dat");
ifstream beamkeys("beamkeys.Dat");

while(getline(beamkeys,myline)) {
sscanf(myline.c_str(),"%d",&btemp);
beams.push_back(btemp);
}
cout<<beams.size()<<endl;
//cin.get();

while(getline(reader,line)){
//cout<<line<<endl;
//cin.get();
if(line.find("Mode   22.")==0){
break;
}
}
newfile<<" #nodes:"<<endl;

while(getline(reader,line)){
//  	  cout<<line<<endl;
//	  cin.get();
	if(line.find("Mode")==0) {
			  for(int i=0;i<beams.size();i++){
			  newfile<<devarr[beams[i]][0]<<" "<<devarr[beams[i]][1]<<" "<<devarr[beams[i]][2]<<endl;
			  }
			  newfile<<" #nodes:"<<endl;
			  count=0;
	  } else {
			  sscanf(line.c_str(),"%*s %*s %lf %lf %lf",&a,&b,&c);
			  dev=sqrt(a*a+b*b+c*c);
//			  cout<< a <<b<<c;
			  newfile<<a<<" "<<b<<" "<<c<<endl;
			  devarr[count][0]=a;
			  devarr[count][1]=b;
			  devarr[count][2]=c;
			  if(dev<min) {min= dev;}
			  if(dev>max) {max= dev;}
			  count++;
	  }
	  
	  
	  
}
reader.close();
newfile.close();
cout<<"max ="<<max<<endl;
cout<<"min ="<<min<<endl;
return 0;
}