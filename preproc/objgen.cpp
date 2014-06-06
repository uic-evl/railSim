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

	  vector<double> position; 

	  int timestep=0;
	  vector<int> elements;
	  int elementcount=0;
	  int count=0;
	  int nodenumber;
	  double x,y,z;
	  double a,b,c;
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
	  cout<<"building object "<<endl;
	  path="myshape.obj";
	  ofstream newfile(path.c_str());
	  ofstream beamkeys("beamkeys.Dat");
	  newfile<<"# nodes"<<endl;
	  
	   cout<<"adding nodes"<<endl;
	   for(int j=0;j<count;j++){
				  newfile<<"v "<<position[j*3]<<" "<<position[j*3+1]<<" "<<position[j*3+2]<<endl;
		}
	  
/*		
	  newfile<<"# texture coords"<<endl;
	  newfile<<"vt 1.0000 1.0000 0.0000"<<endl;
	  newfile<<"vt 0.0000 1.0000 0.0000"<<endl;
	  newfile<<"vt 0.0000 0.0000 0.0000"<<endl;
	  newfile<< "vt 1.0000 0.0000 0.0000"<<endl;
*/
	  count=numnodes;
	  cout<<"adding elements"<<endl;
	  newfile<<"# elements"<<endl;
	 // newfile<<"usemtl mat"<<endl;


	  for(int j=0;j<elementcount;j++){	 

	   //render beam elements by extruding them into z axis into polygons
	 
		if (elements[j*9]>=1 && elements[j*9]<=3){

		 n1=(elements[j*9+1]-1)*3;
		 n2=(elements[j*9+2]-1)*3;
		 
		 if(elements[j*9]==3){  
		
			//sleepers
			//top right
			   newfile<<"v "<<position[n1]+0.1<<" "<<position[n1+1]<<" "<<position[n1+2]+0.05<<endl;
				beamkeys<<elements[j*9+1]-1<<endl;
				
			   newfile<<"v "<<position[n2]+0.1<<" "<<position[n2+1]<<" "<<position[n2+2]+0.05<<endl;
				beamkeys<<elements[j*9+2]-1<<endl;
				
			   //bottom right
			   newfile<<"v "<<position[n1]+0.1<<" "<<position[n1+1]<<" "<<position[n1+2]-0.15<<endl;
				beamkeys<<elements[j*9+1]-1<<endl;
				
			   newfile<<"v "<<position[n2]+0.1<<" "<<position[n2+1]<<" "<<position[n2+2]-0.15<<endl;
				beamkeys<<elements[j*9+2]-1<<endl;

			   //top left
			   newfile<<"v "<<position[n1]-0.1<<" "<<position[n1+1]<<" "<<position[n1+2]+0.05<<endl;
				beamkeys<<elements[j*9+1]-1<<endl;
				
			   newfile<<"v "<<position[n2]-0.1<<" "<<position[n2+1]<<" "<<position[n2+2]+0.05<<endl;
				beamkeys<<elements[j*9+2]-1<<endl;

			   //bottom left

			   newfile<<"v "<<position[n1]-0.1<<" "<<position[n1+1]<<" "<<position[n1+2]-0.15<<endl;
				beamkeys<<elements[j*9+1]-1<<endl;
				
			   newfile<<"v "<<position[n2]-0.1<<" "<<position[n2+1]<<" "<<position[n2+2]-0.15<<endl;
				beamkeys<<elements[j*9+2]-1<<endl;
		
	   
			  // newfile<<"usemtl sleepers"<<endl;
			   /*
			   newfile<<"f "<<count+1<<"/1 "<<count+2<<"/2 "<<count+4<<"/3 "<<count+3<<"/4"<<endl;
			   newfile<<"f "<<count+3<<"/1 "<<count+4<<"/2 "<<count+8<<"/3 "<<count+7<<"/4"<<endl;
			   newfile<<"f "<<count+5<<"/1 "<<count+7<<"/2 "<<count+8<<"/3 "<<count+6<<"/4"<<endl;
			   newfile<<"f "<<count+1<<"/1 "<<count+5<<"/2 "<<count+6<<"/3 "<<count+2<<"/4"<<endl;
			   newfile<<"f "<<count+1<<"/1 "<<count+3<<"/2 "<<count+7<<"/3 "<<count+5<<"/4"<<endl;
			   newfile<<"f "<<count+2<<"/1 "<<count+6<<"/2 "<<count+8<<"/3 "<<count+4<<"/4"<<endl;
				*/
				
			   newfile<<"f "<<count+1<<" "<<count+2<<" "<<count+4<<" "<<count+3<<endl;
			   newfile<<"f "<<count+3<<" "<<count+4<<" "<<count+8<<" "<<count+7<<endl;
			   newfile<<"f "<<count+5<<" "<<count+7<<" "<<count+8<<" "<<count+6<<endl;
			   newfile<<"f "<<count+1<<" "<<count+5<<" "<<count+6<<" "<<count+2<<endl;
			   newfile<<"f "<<count+1<<" "<<count+3<<" "<<count+7<<" "<<count+5<<endl;
			   newfile<<"f "<<count+2<<" "<<count+6<<" "<<count+8<<" "<<count+4<<endl;
				
				
				
			count+=8;
		 
		 
		 } else {

		//rails

		//A  
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]+0.04<<" "<<position[n1+2]+0.0<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]+0.04<<" "<<position[n2+2]+0.0<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//B
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]-0.04<<" "<<position[n1+2]+0.0<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]-0.04<<" "<<position[n2+2]+0.0<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//C
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]-0.05<<" "<<position[n1+2]-0.04<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]-0.05<<" "<<position[n2+2]-0.04<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//D
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]-0.03<<" "<<position[n1+2]-0.05<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]-0.03<<" "<<position[n2+2]-0.05<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//E
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]-0.03<<" "<<position[n1+2]-0.15<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]-0.03<<" "<<position[n2+2]-0.15<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//F
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]-0.1<<" "<<position[n1+2]-0.2<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]-0.1<<" "<<position[n2+2]-0.2<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//G
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]+0.1<<" "<<position[n1+2]-0.2<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]+0.1<<" "<<position[n2+2]-0.2<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//H
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]+0.03<<" "<<position[n1+2]-0.15<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]+0.03<<" "<<position[n2+2]-0.15<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//I
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]+0.03<<" "<<position[n1+2]-0.05<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]+0.03<<" "<<position[n2+2]-0.05<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;

		//J
		newfile<<"v "<<position[n1]<<" "<<position[n1+1]+0.05<<" "<<position[n1+2]-0.04<<endl;
		newfile<<"v "<<position[n2]<<" "<<position[n2+1]+0.05<<" "<<position[n2+2]-0.04<<endl;
		beamkeys<<elements[j*9+1]-1<<endl;
		beamkeys<<elements[j*9+2]-1<<endl;


	//	newfile<<"usemtl rails"<<endl;
		for(int ucount=0;ucount<=16;ucount+=2){
//		newfile<<"f "<<count+ucount+1<<"/1 "<<count+ucount+3<<"/2 "<<count+ucount+4<<"/3 "<<count+ucount+2<<"/4"<<endl;
		newfile<<"f "<<count+ucount+1<<" "<<count+ucount+3<<" "<<count+ucount+4<<" "<<count+ucount+2<<endl;
			}
//		newfile<<"f "<<count+19<<"/1 "<<count+1<<"/2 "<<count+2<<"/3 "<<count+20<<"/4"<<endl;
		newfile<<"f "<<count+19<<" "<<count+1<<" "<<count+2<<" "<<count+20<<endl;
		count+=20;

			}
	   
		}




	 if (elements[j*9]>=4 && elements[j*9]<=6){
/*
		   newfile<<"f "<<elements[j*9+1]<<"/1 "<<elements[j*9+4]<<"/2 "<<elements[j*9+3]<<"/3 "<<elements[j*9+2]<<"/4"<<endl;
		  
		   newfile<<"f "<<elements[j*9+5]<<"/1 "<<elements[j*9+6]<<"/2 "<<elements[j*9+7]<<"/3 "<<elements[j*9+8]<<"/4"<<endl;

		   newfile<<"f "<<elements[j*9+1]<<"/1 "<<elements[j*9+2]<<"/2 "<<elements[j*9+6]<<"/3 "<<elements[j*9+5]<<"/4"<<endl;
		
		   newfile<<"f "<<elements[j*9+1]<<"/1 "<<elements[j*9+5]<<"/2 "<<elements[j*9+8]<<"/3 "<<elements[j*9+4]<<"/4"<<endl;
		
		   newfile<<"f "<<elements[j*9+2]<<"/1 "<<elements[j*9+3]<<"/2 "<<elements[j*9+7]<<"/3 "<<elements[j*9+6]<<"/4"<<endl;
   
		   newfile<<"f "<<elements[j*9+4]<<"/1 "<<elements[j*9+8]<<"/2 "<<elements[j*9+7]<<"/3 "<<elements[j*9+3]<<"/4"<<endl;
		   
	*/
		   newfile<<"f "<<elements[j*9+1]<<" "<<elements[j*9+4]<<" "<<elements[j*9+3]<<" "<<elements[j*9+2]<<endl;
		  
		   newfile<<"f "<<elements[j*9+5]<<" "<<elements[j*9+6]<<" "<<elements[j*9+7]<<" "<<elements[j*9+8]<<endl;

		   newfile<<"f "<<elements[j*9+1]<<" "<<elements[j*9+2]<<" "<<elements[j*9+6]<<" "<<elements[j*9+5]<<endl;
		
		   newfile<<"f "<<elements[j*9+1]<<" "<<elements[j*9+5]<<" "<<elements[j*9+8]<<" "<<elements[j*9+4]<<endl;
		
		   newfile<<"f "<<elements[j*9+2]<<" "<<elements[j*9+3]<<" "<<elements[j*9+7]<<" "<<elements[j*9+6]<<endl;
   
		   newfile<<"f "<<elements[j*9+4]<<" "<<elements[j*9+8]<<" "<<elements[j*9+7]<<" "<<elements[j*9+3]<<endl;	
		   

		  }
	   

		}
	   cout<<"final nodecount="<<count<<endl;
	   newfile.close();
	   beamkeys.close();
	  
	  }
