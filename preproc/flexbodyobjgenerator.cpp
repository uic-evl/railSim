	#include <iostream>
	#include <fstream>
	#include <string>
	#include <map>
	#include <vector>
	#include <sstream>
	#include <cmath>

	using namespace std;

	int main () {
	string path;
	bool read=true;
	bool flag=false;
	string line;
	double high=3.76*pow(10.0,-5);
	double low=0.0;
	double gap=(high-low)/20.0;
	int colorcat;
	double dev;

	  vector<double> position; 
	  vector<double> tempos; 
	  vector<double> deviation;

	  int timestep=0;
	  vector<int> elements;
	  int elementcount=0;
	  int count=0;
	  int nodenumber;
	  double x,y,z;
	  double a,b,c;
	  double distortion;
	  int type,n1,n2,n3,n4,n5,n6,n7,n8;
	//  string mainpath="";
	  
	  ifstream reader("NLIST.lis");
	   while ( getline(reader,line) ) {
	  if(line.length()<10){read=false;}
	  if (read){
	  count++;
	  sscanf(line.c_str(),"%*d %lf %lf %lf %*f %*f %*f",&x,&y,&z);
	//  cout<<x<<"	"<<y<<"		"<<z<< endl;
	//  cin.get();
	  position.push_back(x);
	  position.push_back(y);
	  position.push_back(z);
	  tempos.push_back(x);
	  tempos.push_back(y);
	  tempos.push_back(z);
	  deviation.push_back(1.0);
			}
	  if(line.find("   NODE")==0){read=true;}
	  }

	  int numnodes=count;
	  reader.close();


	  ifstream reader1("ELIST.lis");
	  read=false; 
	  while ( getline(reader1,line) ) {
	   
	   sscanf(line.c_str(),"%d %*d %d %*d %*d %*d %d %d %d %d %d %d %d %d %d",&nodenumber,&type,&n1,&n2,&n3,&n4,&n5,&n6,&n7,&n8);
	   if(nodenumber==elementcount+1){
	   elementcount++;
	   elements.push_back(type);
	   elements.push_back(n1);
	   elements.push_back(n2);
	   elements.push_back(n3);
	   elements.push_back(n4);
	   elements.push_back(n5);
	   elements.push_back(n6);
	   elements.push_back(n7);
	   elements.push_back(n8);
		}
	   }

	  cout<<"data loaded"<<endl;
	  cout<<"node count "<<count<<endl;
	  cout<<"element count "<<elementcount<<endl;
	  reader1.close();




	  // Generate obj files
	  int objcounter=0;
	  cout<<"building objects: "<<endl;
	  ifstream reader2("NodalDisplacements.Dat");

	  while(!flag){
	  getline(reader2,line);
	  if(line.find(" #nodes:")==0){flag=true;}
	  }


	  while(getline(reader2,line)){
	  
	  stringstream path;
	  path<<"shape"<<objcounter<<".obj";
	  ofstream newfile(path.str().c_str());
	  newfile<<"mtllib newmaterial.mtl"<<endl;
	  newfile<<"# nodes"<<endl;
	  cout<<"constructing object "<<objcounter<<endl;
	   cout<<"adding nodes"<<endl;
	   int j=0;
		  while(line.find(" #nodes:")!=0){
		  getline(reader2,line);
		  if(line.find("v   ")==0){
		  sscanf(line.c_str(),"%*s %lf %lf %lf",&a,&b,&c);
				  tempos[j*3]=position[j*3]+a;
				  tempos[j*3+1]=position[j*3+1]+b;
				  tempos[j*3+2]=position[j*3+2]+c;
				  newfile<<"v "<<tempos[j*3]<<" "<<tempos[j*3+1]<<" "<<tempos[j*3+2]<<endl;
				  dev=sqrt(a*a+b*b+c*c);
				  deviation[j]=dev;
			j++;
		  }
		  }
	  
	  newfile<<"v 81.6750 3.7360 -1.1100";    
		
	  newfile<<"# texture coords"<<endl;
	  newfile<<"vt 1.0000 1.0000 0.0000"<<endl;
	  newfile<<"vt 0.0000 1.0000 0.0000"<<endl;
	  newfile<<"vt 0.0000 0.0000 0.0000"<<endl;
	  newfile<< "vt 1.0000 0.0000 0.0000"<<endl;
	  count=numnodes;
	  cout<<"adding elements"<<endl;
	  newfile<<"# elements"<<endl;
	  int prevelement=7;
	 // newfile<<"usemtl mat"<<endl;


	  for(int j=0;j<elementcount;j++){	 

	  
	   

	   //render beam elements by extruding them into z axis into polygons
	 
		if (elements[j*9]>=1 && elements[j*9]<=3){

		 n1=(elements[j*9+1]-1)*3;
		 n2=(elements[j*9+2]-1)*3;
		 distortion=deviation[(elements[j*9+1]-1)]+deviation[(elements[j*9+2]-1)];
		 
		 colorcat=floor((distortion-low)/gap);
		 colorcat=min(19,colorcat);
	/*	 if(colorcat>7){
		 cout<<distortion<<endl;
		 cin.get();
		 }*/
		 newfile<<"usemtl mat"<<colorcat<<endl;
		 
		 if(elements[j*9]==3){  
		
			//sleepers
			//top right
			   newfile<<"v "<<tempos[n1]+0.1<<" "<<tempos[n1+1]<<" "<<tempos[n1+2]+0.05<<endl;
	   
			   newfile<<"v "<<tempos[n2]+0.1<<" "<<tempos[n2+1]<<" "<<tempos[n2+2]+0.05<<endl;

			   //bottom right
			   newfile<<"v "<<tempos[n1]+0.1<<" "<<tempos[n1+1]<<" "<<tempos[n1+2]-0.15<<endl;
	   
			   newfile<<"v "<<tempos[n2]+0.1<<" "<<tempos[n2+1]<<" "<<tempos[n2+2]-0.15<<endl;

			   //top left
			   newfile<<"v "<<tempos[n1]-0.1<<" "<<tempos[n1+1]<<" "<<tempos[n1+2]+0.05<<endl;
	   
			   newfile<<"v "<<tempos[n2]-0.1<<" "<<tempos[n2+1]<<" "<<tempos[n2+2]+0.05<<endl;

			   //bottom left

			   newfile<<"v "<<tempos[n1]-0.1<<" "<<tempos[n1+1]<<" "<<tempos[n1+2]-0.15<<endl;
	   
			   newfile<<"v "<<tempos[n2]-0.1<<" "<<tempos[n2+1]<<" "<<tempos[n2+2]-0.15<<endl;
		
	   
			  // newfile<<"usemtl sleepers"<<endl;
			   newfile<<"f "<<count+1<<"/1 "<<count+2<<"/2 "<<count+4<<"/3 "<<count+3<<"/4"<<endl;
			   newfile<<"f "<<count+3<<"/1 "<<count+4<<"/2 "<<count+8<<"/3 "<<count+7<<"/4"<<endl;
			   newfile<<"f "<<count+5<<"/1 "<<count+7<<"/2 "<<count+8<<"/3 "<<count+6<<"/4"<<endl;
			   newfile<<"f "<<count+1<<"/1 "<<count+5<<"/2 "<<count+6<<"/3 "<<count+2<<"/4"<<endl;
			   newfile<<"f "<<count+1<<"/1 "<<count+3<<"/2 "<<count+7<<"/3 "<<count+5<<"/4"<<endl;
			   newfile<<"f "<<count+2<<"/1 "<<count+6<<"/2 "<<count+8<<"/3 "<<count+4<<"/4"<<endl;
	   
			   count+=8;
		 
		 
		 } else {

		//rails

		//A  
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]+0.04<<" "<<tempos[n1+2]+0.0<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]+0.04<<" "<<tempos[n2+2]+0.0<<endl;

		//B
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]-0.04<<" "<<tempos[n1+2]+0.0<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]-0.04<<" "<<tempos[n2+2]+0.0<<endl;

		//C
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]-0.05<<" "<<tempos[n1+2]-0.04<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]-0.05<<" "<<tempos[n2+2]-0.04<<endl;

		//D
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]-0.03<<" "<<tempos[n1+2]-0.05<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]-0.03<<" "<<tempos[n2+2]-0.05<<endl;

		//E
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]-0.03<<" "<<tempos[n1+2]-0.15<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]-0.03<<" "<<tempos[n2+2]-0.15<<endl;

		//F
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]-0.1<<" "<<tempos[n1+2]-0.2<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]-0.1<<" "<<tempos[n2+2]-0.2<<endl;

		//G
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]+0.1<<" "<<tempos[n1+2]-0.2<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]+0.1<<" "<<tempos[n2+2]-0.2<<endl;

		//H
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]+0.03<<" "<<tempos[n1+2]-0.15<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]+0.03<<" "<<tempos[n2+2]-0.15<<endl;

		//I
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]+0.03<<" "<<tempos[n1+2]-0.05<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]+0.03<<" "<<tempos[n2+2]-0.05<<endl;

		//J
		newfile<<"v "<<tempos[n1]<<" "<<tempos[n1+1]+0.05<<" "<<tempos[n1+2]-0.04<<endl;
		newfile<<"v "<<tempos[n2]<<" "<<tempos[n2+1]+0.05<<" "<<tempos[n2+2]-0.04<<endl;


	//	newfile<<"usemtl rails"<<endl;
		for(int ucount=0;ucount<=16;ucount+=2){
		newfile<<"f "<<count+ucount+1<<"/1 "<<count+ucount+3<<"/2 "<<count+ucount+4<<"/3 "<<count+ucount+2<<"/4"<<endl;
		}
		newfile<<"f "<<count+19<<"/1 "<<count+1<<"/2 "<<count+2<<"/3 "<<count+20<<"/4"<<endl;
		count+=20;

			}
	   
		}




	 if (elements[j*9]>=4 && elements[j*9]<=6){
	/*
			if(prevelement!=elements[j*9]){
			switch (elements[j*9]) {
				   case 7:
					   newfile<<"usemtl mat"<<endl;
					   break;
				   case 8:
					   newfile<<"usemtl green"<<endl;
					   break;
				   case 9:
					   newfile<<"usemtl blue"<<endl;
					   break;
				   }
			prevelement=elements[j*9];
			}*/

		 distortion=deviation[(elements[j*9+1]-1)]+deviation[(elements[j*9+4]-1)]+deviation[(elements[j*9+3]-1)]+deviation[(elements[j*9+2]-1)];
		  colorcat=floor((distortion-low)/gap);
				colorcat=min(19,colorcat);
		 newfile<<"usemtl mat"<<colorcat<<endl;

		   newfile<<"f "<<elements[j*9+1]<<"/1 "<<elements[j*9+4]<<"/2 "<<elements[j*9+3]<<"/3 "<<elements[j*9+2]<<"/4"<<endl;
		   
		 distortion=deviation[(elements[j*9+5]-1)]+deviation[(elements[j*9+6]-1)]+deviation[(elements[j*9+7]-1)]+deviation[(elements[j*9+8]-1)];
			 colorcat=floor((distortion-low)/gap);
			 colorcat=min(19,colorcat);
		 newfile<<"usemtl mat"<<colorcat<<endl;

		   newfile<<"f "<<elements[j*9+5]<<"/1 "<<elements[j*9+6]<<"/2 "<<elements[j*9+7]<<"/3 "<<elements[j*9+8]<<"/4"<<endl;
		   
		 distortion=deviation[(elements[j*9+1]-1)]+deviation[(elements[j*9+2]-1)]+deviation[(elements[j*9+6]-1)]+deviation[(elements[j*9+5]-1)];
		  colorcat=floor((distortion-low)/gap);
		  colorcat=min(19,colorcat);
		 newfile<<"usemtl mat"<<colorcat<<endl;

		   newfile<<"f "<<elements[j*9+1]<<"/1 "<<elements[j*9+2]<<"/2 "<<elements[j*9+6]<<"/3 "<<elements[j*9+5]<<"/4"<<endl;
		
		distortion=deviation[(elements[j*9+1]-1)]+deviation[(elements[j*9+5]-1)]+deviation[(elements[j*9+8]-1)]+deviation[(elements[j*9+4]-1)];
			 colorcat=floor((distortion-low)/gap);
			 colorcat=min(19,colorcat);
		 newfile<<"usemtl mat"<<colorcat<<endl;
		
		
		   newfile<<"f "<<elements[j*9+1]<<"/1 "<<elements[j*9+5]<<"/2 "<<elements[j*9+8]<<"/3 "<<elements[j*9+4]<<"/4"<<endl;
		
		distortion=deviation[(elements[j*9+2]-1)]+deviation[(elements[j*9+3]-1)]+deviation[(elements[j*9+7]-1)]+deviation[(elements[j*9+6]-1)];
			 colorcat=floor((distortion-low)/gap);
			 colorcat=min(19,colorcat);
		 newfile<<"usemtl mat"<<colorcat<<endl;
		
		   newfile<<"f "<<elements[j*9+2]<<"/1 "<<elements[j*9+3]<<"/2 "<<elements[j*9+7]<<"/3 "<<elements[j*9+6]<<"/4"<<endl;

		 distortion=deviation[(elements[j*9+4]-1)]+deviation[(elements[j*9+8]-1)]+deviation[(elements[j*9+7]-1)]+deviation[(elements[j*9+3]-1)];
			  colorcat=floor((distortion-low)/gap);
			  colorcat=min(19,colorcat);
		 newfile<<"usemtl mat"<<colorcat<<endl;
		   
		   newfile<<"f "<<elements[j*9+4]<<"/1 "<<elements[j*9+8]<<"/2 "<<elements[j*9+7]<<"/3 "<<elements[j*9+3]<<"/4"<<endl;

		  }
	   

		}
	   newfile.close();
	   objcounter++;
	  
	  }
	}
