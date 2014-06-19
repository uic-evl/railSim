/********************************************************************************
RailSim Project 
Author:
Vaibhav Govilkar vaibhavgovilkar88@gmail.com
********************************************************************************/
#include <omega.h>
#include <cyclops/cyclops.h>
#include <omegaToolkit.h>

#include <iostream>
#include <fstream>

#include<osg/PolygonMode>
#include<osg/Point>
#include<osg/ClipNode>
#include<osg/ClipPlane>
#include<osgUtil/Optimizer>

using namespace omega;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;
using namespace cyclops;
using namespace std;			//Needed for file reading, but there might be an OmegaLib way
using namespace omegaOsg;
// The name of the script to launch automatically at startup
String sDefaultScript = "";


///////////////////////////////////////////////////////////////////////////////////////////////////
// Classes

// Findgeometry class used to extract the geometry from an osg Node using the NodeVisitor
class Findgeometry : public osg::NodeVisitor
{
public:
	//Set NodeVisitor mode to traverse all children in constructor initialization list
	Findgeometry(): osg::NodeVisitor(TRAVERSE_ALL_CHILDREN){}
	osg::Geometry* mygeometry;
	
	//define virtual method apply with Geode parameter to set Findgeometry member- mygeometry. Which gives access to the required geometry in the osgnode.
	virtual void apply(osg::Geode& node)
	{
		mygeometry=node.getDrawable(0)->asGeometry();
	}
};

//Object loader class to load a geometry into the scene from a file
class objloader : public Entity
{
private: 
	
	osg::Geometry* myGeometry;

public:

	osg::Geometry* getGeometry(){
	return myGeometry;
	}

	//Constructor loads the object using a vector of vertices and a vector of faces previously loaded from a file.
	objloader(SceneManager* scene, Vector<osg::Vec3 *> &vertices, Vector<int *> &quadfaces):Entity(scene){
	//Create a new Geode, geometry to put in it and a vertex array.
	osg::Geode* node = new osg::Geode();
	osg::Geometry* modeGeometry = new osg::Geometry();
	osg::Vec3Array* modeVertices = new osg::Vec3Array;
	
	//Fill the vertex array and dump it into the geometry
	for(int i=0; i<vertices.size();i++){
	modeVertices->push_back(*vertices[i]);
	}
	modeGeometry->setVertexArray(modeVertices);

	//Create new drawable quadrilateral element to generate primitives to assemple inot a geometry 
	osg::DrawElementsUInt* modeFaces = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

	for(int i=0; i<quadfaces.size();i++){
	modeFaces->push_back(quadfaces[i][0]-1);
	modeFaces->push_back(quadfaces[i][1]-1);
	modeFaces->push_back(quadfaces[i][2]-1);
	modeFaces->push_back(quadfaces[i][3]-1);
	modeGeometry->addPrimitiveSet(modeFaces);

	modeFaces=new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	}
	myGeometry=modeGeometry;

	//add geometry to the geode
	node->addDrawable(modeGeometry);

	//initialize the geode into the scene
	initialize(node);
	
	}
};


class OmegaViewer: public EngineModule, public IMenuItemListener
{
public:
	//Constructor
	OmegaViewer();

	//Pointer to the SceneManger.  This is impt bc it controlls the scene: camera, entities, etc.
	SceneManager* sceneMngr;						

	//Pointers to the entities in the SceneManger.  These pointers will be assigned later for ease of use.	
	objloader* modeshape;		
	Entity* wheelSetEntity;	
	Entity* frameEntity;
	Findgeometry *modegeometry;

	virtual void initialize();
	virtual void update(const UpdateContext& context);
	virtual void handleEvent(const Event& evt);
	virtual void onMenuItemEvent(MenuItem* mi);

	//setup methods for camera behavior
	void toggleCameraController( );
	void camTrans( Vector3f pos );
	void camRot( Vector3f pitchYawRoll );
	void camRot( Quaternion orientation );
	void camDefault();
	Quaternion getcamRot();
	Vector3f getcamTrans();

	//the data sets
	void initializeData();
	bool loadData( String configAttr , Vector<float> &dataVector , bool pos );
	bool loaddeform( String configAttr , Vector< Ref<osg::Vec3Array > > &dataVector);
	bool loadobj( String configAttr , Vector<osg::Vec3 *> &vertices, Vector<int *> &quadfaces);
	bool loadcolors( String configAttr , osg::Vec3Array* &);

	//updating a specific entity
	void updateEntity( Entity* entity , vector<float> pos , vector<float> rot, int curTimeStep );
	void updateCamera( vector<float> pos , vector<float> rot, int curTimeStep );

	//animation data
	Vector<float> wheel_PVec;
	Vector<float> frame_PVec;
	Vector<float> wheel_RVec;
	Vector<float> frame_RVec;

	//Shape deformation data - for simulation as well as modeshapes
	Vector< Ref<osg::Vec3Array> > def_data;
	Vector< Ref<osg::Vec3Array> > mdef_data;
	 osg::Vec3Array* col_data;
	 Vector<osg::Vec3 *> vertices;
	 Vector<int *> quadfaces;



	//animation variables

	float shapetime;
	int mcount;
	int mode;
	float animationTimer;	//the amount of time the animation will run for in milliseconds
	float animationPadding;	
	float curTime;			//The current time in milliseconds
	int numTimeSteps;		//The total number of timesteps of data pts in the files
	int curTimeStep;		//The current time step in the current rendering loop as index into array 
	int prevTimeStep;		//The last time step since the rendering loop was called

	//scene flags
	bool isAnimating;		//toggle for if animations is happening or just still image
	bool isKeyFraming;		//toggle that user is "stepping through" the time steps by presssing a key 
	bool isFreeFly;			//Toggle to allow uses to move around using the d-pad and not locked down to a certain view 
	int polymode;
	
	//camera control
	int campos;				//camera position toggle
	Quaternion currot;		//camera rotation
	Vector3f curpos;		//camera postion
	Vector3f scale;

	//scanner 
	int xclip;
	int mag;
	osg::Uniform* clipX;

	//magnifier uniform for modeshape deformation-exaggeration
	osg::Uniform* magnifier;

	//a boolean uniform to inform the fragment shader wheather the application is running the simulation or the mode visualization 
	osg::Uniform* mymode;

	//Vector math with custom interpolation functions for camera motion
	Vector3f Lerp(float t, Vector3f start,Vector3f end);
	Quaternion toquaternion(const Vector3f& yawPitchRoll); 
	Quaternion Slerp(float t,Quaternion start,Quaternion end);

	//Menu
	MenuManager *myMenuManager;
	MenuItem *simulation;
	MenuItem *modemenu[10];
	MenuItem *magslider;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
OmegaViewer::OmegaViewer(): EngineModule("OmegaViewer"), magnifier(NULL),mymode(NULL)
{
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//  read in the data files that are associated with this rigid body simultaion
//	This might be a custom func depending on the file formate given... over time the DSL should give a standard format
void OmegaViewer::initializeData()
{
	String data;
	
	OsgModule::instance()->setCompileGLObjects(false);
	//Flag to dertemine if you were able to get all the data you wanted.
	bool gotAllData = true;

	//Set the number of time steps to the max integar value ot begin with.
	numTimeSteps = INT_MAX;
	
	//Look at /omegaLib/data/railsim.cfg, "wheelPos" should be present under teh config:{...} section
	data = "config/wheelPos";
	//call loadData by passing in:
	//		the data string
	//		array to store the data
	//		flag to determine if it is position data.  Tuer:pos			False:rot
	//returns whether it was successful or not
	gotAllData = loadData( data , wheel_PVec , true);
	//if not successful, use owarn to print out an error.
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);

	//Same all the way down for the other 5 data sets
	data = "config/wheelRot";
	gotAllData = loadData( data , wheel_RVec , false);
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);

	data = "config/framePos";
	gotAllData = loadData( data , frame_PVec, true);
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);

	data = "config/frameRot";
	gotAllData = loadData( data , frame_RVec , false);
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);

	data = "config/nodedeform";
	gotAllData = loaddeform( data , def_data );
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);
	
	data = "config/modedeform";
	gotAllData = loaddeform( data , mdef_data );
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);

	data = "config/modeobject";
	gotAllData = loadobj( data , vertices, quadfaces );
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);

	data = "config/colors";
	gotAllData = loadcolors( data , col_data );
	if(!gotAllData ) ofwarn("!Some data failed to load %1%" , %data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//   Load a specific data file
//call loadData by passing in:
//		the data string
//		array to store the data
//		flag to determine if it is position data.  Tuer:pos			False:rot
//returns whether it was successful or not

bool OmegaViewer::loadcolors( String configAttr ,  osg::Vec3Array* &colors ){
//flag to determine success
	bool successful = false;

	//get a reference of :: /omegaLib/data/railsim.cfg from the SystemManger within OmegaLib
	Config* cfg = SystemManager::instance()->getAppConfig();

	//Check to make sure the "configAttr" passed in is part of this config file 
	if(cfg->exists(configAttr))
	{
		//maps the relative path to the absolute path
		String mapName = String((const char*)cfg->lookup(configAttr));

		//Instantiates a data manager
		DataManager* dm = SystemManager::instance()->getDataManager();
		
		//Uses the dataManager to get the data within the config file bbase on the absolute path 
		DataInfo cfgInfo = dm->getInfo(mapName);

		//if the data is not crap
		if(!cfgInfo.isNull())
		{
			//Parsese the file
			String line;								//this will contain the data read from the file
			ifstream myfile (cfgInfo.path.c_str());		//opening the file.
			if (myfile.is_open())						//if the file is open
			{
				colors= new osg::Vec3Array;
				while (! myfile.eof() )					//while the end of file is NOT reached
				{
					getline (myfile,line);				 //get one line from the file
					int x;
					sscanf( line.c_str() , "%d" , &x); //get desired data
					switch(x){
					case 2:
						colors->push_back(osg::Vec3(1.0f, 0.0f, 0.0f) );
						break;
					case 3:
						colors->push_back(osg::Vec3(0.0f, 1.0f, 0.0f) );
						break;
					case 4:
						colors->push_back(osg::Vec3(0.0f, 0.0f, 1.0f) );
						break;
					case 5:
						colors->push_back(osg::Vec3(1.0f, 0.0f, 1.0f) );
						break;
					case 6:
						colors->push_back(osg::Vec3(1.0f, 1.0f, 0.0f) );
					}

					//cout << x << ","<< y << ","<< z << "," << w << endl; //and output it
				}
				myfile.close();							//closing the file
				cout<<"color array size="<<colors->size()<<endl;
			}
			else 
			{
				ofwarn("Failed to load unable to open file : %1%", %mapName);	//if the file is not open output
				successful = false;
			}
		}
		else
		{
			ofwarn("Failed to load %1% data not found %1%", %mapName);
			successful = false;
		}
		successful = true;
	}
	else
	{
		ofwarn("Failed to load %1% config not found... " , %configAttr);
		successful = false;
	}

	if ( successful )
	{
		//the data set with the smallest number of timesteps determines the scope of the animation
		ofmsg("Loading data: %1% ... Success!!!", %configAttr);
	}
	return successful;

}

bool OmegaViewer::loadData( String configAttr , Vector<float> &dataVector , bool pos )
{
	//flag to determine success
	bool successful = false;

	//get a reference of :: /omegaLib/data/railsim.cfg from the SystemManger within OmegaLib
	Config* cfg = SystemManager::instance()->getAppConfig();

	//Check to make sure the "configAttr" passed in is part of this config file 
	if(cfg->exists(configAttr))
	{
		//maps the relative path to the absolute path
		String mapName = String((const char*)cfg->lookup(configAttr));

		//Instantiates a data manager
		DataManager* dm = SystemManager::instance()->getDataManager();
		
		//Uses the dataManager to get the data within the config file bbase on the absolute path 
		DataInfo cfgInfo = dm->getInfo(mapName);

		//if the data is not crap
		if(!cfgInfo.isNull())
		{
			//Parsese the file
			String line;								//this will contain the data read from the file
			ifstream myfile (cfgInfo.path.c_str());		//opening the file.
			if (myfile.is_open())						//if the file is open
			{
				while (! myfile.eof() )					//while the end of file is NOT reached
				{
					getline (myfile,line);				 //get one line from the file
					float x , y , z , w;
					if( pos) sscanf( line.c_str() , "%*s %f %*s %f %*s %f %*s %*s %f" , &x , &y, &z, &w ); //get desired data
					else sscanf( line.c_str() , "%f %f %f %f" , &x , &y, &z, &w ); //get desired data
					dataVector.push_back (x);
					dataVector.push_back (y);
					dataVector.push_back (z);
					dataVector.push_back (w);
					//cout << x << ","<< y << ","<< z << "," << w << endl; //and output it
				}
				myfile.close();							//closing the file
			}
			else 
			{
				ofwarn("Failed to load unable to open file : %1%", %mapName);	//if the file is not open output
				successful = false;
			}
		}
		else
		{
			ofwarn("Failed to load %1% data not found %1%", %mapName);
			successful = false;
		}
		successful = true;
	}
	else
	{
		ofwarn("Failed to load %1% config not found... " , %configAttr);
		successful = false;
	}

	if ( successful )
	{
		//the data set with the smallest number of timesteps determines the scope of the animation
		numTimeSteps = min( numTimeSteps , (int) ((dataVector.size() - 1) / 4.0) );
		ofmsg("Loading data: %1% ... Success!!!", %configAttr);
	}
	return successful;
}



bool OmegaViewer::loaddeform( String configAttr , Vector< Ref<osg::Vec3Array> > &dataVector)
{
	//flag to determine success
	bool successful = false;

	//get a reference of :: /omegaLib/data/railsimmodeshapes.cfg from the SystemManger within OmegaLib
	Config* cfg = SystemManager::instance()->getAppConfig();

	//Check to make sure the "configAttr" passed in is part of this config file 
	if(cfg->exists(configAttr))
	{
		//maps the relative path to the absolute path
		String mapName = String((const char*)cfg->lookup(configAttr));

		//Instantiates a data manager
		DataManager* dm = SystemManager::instance()->getDataManager();
		
		//Uses the dataManager to get the data within the config file bbase on the absolute path 
		DataInfo cfgInfo = dm->getInfo(mapName);

		//if the data is not crap
		if(!cfgInfo.isNull())
		{
			//Parsese the file
			String line;								//this will contain the data read from the file
			ifstream myfile (cfgInfo.path.c_str());		//opening the file.
			if (myfile.is_open())						//if the file is open
			{	
				osg::Vec3Array* temp = new osg::Vec3Array; 
				getline(myfile,line);
				while (getline (myfile,line))					//while the end of file is NOT reached
				{
					if(line.find(" #nodes:")!=0) {
				//get one line from the file
					float a,b,c;
					sscanf(line.c_str(),"%f %f %f",&a,&b,&c);
					temp->push_back(osg::Vec3(a,b,c));
				} else { 
//					for(int i=0;i<15552;i++){
//					for(int i=0;i<4032;i++){
//
//					temp->push_back(0.0f);
//					}
					dataVector.push_back(temp); 
					temp = new osg::Vec3Array; 
				}
				}

				
				cout<<"deform data size: " << dataVector[5]->size()<<endl;
			//	cin.get();
				myfile.close();							//closing the file
			}
			else 
			{
				ofwarn("Failed to load unable to open file : %1%", %mapName);	//if the file is not open output
				successful = false;
			}
		}
		else
		{
			ofwarn("Failed to load %1% data not found %1%", %mapName);
			successful = false;
		}
		successful = true;
	}
	else
	{
		ofwarn("Failed to load %1% config not found... " , %configAttr);
		successful = false;
	}

	if ( successful )
	{
		//the data set with the smallest number of timesteps determines the scope of the animation
//		numTimeSteps = min( numTimeSteps , (int) ((dataVector.size() - 1) / 4.0) );
		ofmsg("Loading data: %1% ... Success!!!", %configAttr);
	}
	return successful;
}




bool OmegaViewer::loadobj( String configAttr , Vector <osg::Vec3 *> &vertices, Vector<int *> &quadfaces)
{
	//flag to determine success
	bool successful = false;

	//get a reference of :: /omegaLib/data/railsimmodeshapes.cfg from the SystemManger within OmegaLib
	Config* cfg = SystemManager::instance()->getAppConfig();

	//Check to make sure the "configAttr" passed in is part of this config file 
	if(cfg->exists(configAttr))
	{
		//maps the relative path to the absolute path
		String mapName = String((const char*)cfg->lookup(configAttr));

		//Instantiates a data manager
		DataManager* dm = SystemManager::instance()->getDataManager();
		
		//Uses the dataManager to get the data within the config file bbase on the absolute path 
		DataInfo cfgInfo = dm->getInfo(mapName);

		//if the data is not crap
		if(!cfgInfo.isNull())
		{

			//Parsese the file
			String line;								//this will contain the data read from the file
			ifstream myfile (cfgInfo.path.c_str());		//opening the file.
		
			if (myfile.is_open())						//if the file is open
			{	
				while (getline (myfile,line))					//while the end of file is NOT reached
				{
					if(line.find("v ")==0) {
				//get one line from the file					
					float a,b,c;
					sscanf(line.c_str(),"%*s %f %f %f",&a,&b,&c);
					osg::Vec3* temp = new osg::Vec3(a,b,c);
					vertices.push_back(temp);
					}
					
					if(line.find("f ")==0) {
					//get one line from the file
					int a,b,c,d;
					sscanf(line.c_str(),"%*s %d %d %d %d",&a,&b,&c,&d);
					int *face = new int[4];
					face[0]=a;
					face[1]=b;
					face[2]=c;
					face[3]=d;
					quadfaces.push_back(face);
//					cout<<quadfaces[quadfaces.size()-1][0]<<"  "<<quadfaces[quadfaces.size()-1][1]<<"	"<<quadfaces[quadfaces.size()-1][2]<<"  "<<quadfaces[quadfaces.size()-1][3]<<endl;
//					cin.get();
					} 
		
				}
				myfile.close();							//closing the file
			}
			else 
			{
				ofwarn("Failed to load unable to open file : %1%", %mapName);	//if the file is not open output
				successful = false;
			}
		
		}
		else
		{
			ofwarn("Failed to load %1% data not found %1%", %mapName);
			successful = false;
		}
		successful = true;
	}
	else
	{
		ofwarn("Failed to load %1% config not found... " , %configAttr);
		successful = false;
	}

	if ( successful )
	{
		//the data set with the smallest number of timesteps determines the scope of the animation
//		numTimeSteps = min( numTimeSteps , (int) ((dataVector.size() - 1) / 4.0) );
		ofmsg("Loading data: %1% ... Success!!!", %configAttr);
	}
	return successful;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void OmegaViewer::initialize()
{
#ifdef OMEGA_USE_PYTHON
	omegaToolkitPythonApiInit();
	cyclopsPythonApiInit();
#endif
	//To deal with clustering
	//Instanciate an instance of ScenenManager
	sceneMngr = SceneManager::createAndInitialize();

	// Load scene
	Config* cfg = SystemManager::instance()->getAppConfig();
	Setting& sCfg = cfg->lookup("config");
	String sceneFileName = Config::getStringValue("scene", sCfg, "");
	sceneMngr->loadScene(sceneFileName);

	//~~~~~~ Form context to scene nodes that represent the .objs
	//go into the sceneMangr to find the entity 0 as defined by
	// 	/omegalib/data/railsim/test/railSim.scene under the heading <Entities>
	//  Assign the pointers to entities so you do not have to constantly look them up later
	
	wheelSetEntity = dynamic_cast<AnimatedObject*>(getEngine()->getScene()->getChild("wheel"));
	if(!wheelSetEntity) owarn("Wheel not loaded");
	else omsg("Wheel loaded");

	frameEntity = dynamic_cast<AnimatedObject*>(getEngine()->getScene()->getChild("frame"));
	if(!frameEntity) owarn("Frame not loaded");
	else omsg("Frame loaded");
	
	//go get the data
	initializeData();

//  WheelSetEntity->setEffect("textured -v border");
//	frameEntity->setEffect("textured -v border");
	
//	multimodel shape trial.
	/*
	modeshape= AnimatedObject::fromEntity(sceneMngr->getEntityByName("modeshape"));
	if(!modeshape) owarn("modeshape not loaded");
	else omsg("modeshape loaded");
	modeshape->setCurrentModelIndex(0);		
	*/
	
	
	modeshape= new objloader(sceneMngr,vertices,quadfaces) ;

	modeshape->setName("modeshape");	 

	mcount=0;
	shapetime=0.0;
	
	
	mode=0;

//	ProgramAsset* prog= sceneMngr->getOrCreateProgram("Colored-clip","cyclops/common/Colored-clip.vert","cyclops/common/Colored-clip.frag");
	ProgramAsset* progsim= sceneMngr->getOrCreateProgram("Textured-border","railSim/Textured.vert","railSim/Textured-border.frag");
	ProgramAsset* progmode= sceneMngr->getOrCreateProgram("Textured-modes","railSim/Texturedmode.vert","railSim/Textured-border.frag");
	modeshape->setEffect("Textured-border -s 0 -t -g 0");

	progsim->program->addBindAttribLocation("deformation",6);
	progmode->program->addBindAttribLocation("deformation",6);
	progmode->program->addBindAttribLocation("coloring",7);


	cout<<"number of vertices: "<<modeshape->getGeometry()->getVertexArray()->getNumElements()<<endl;
	


	
	//Animation var init
	curTime = 0;
	animationTimer = 10.0;
	animationPadding = 1.5;
	prevTimeStep = 0;
	curTimeStep = 0;
	polymode=0;

	//init the camera
	camDefault();
	curpos=Vector3f( 20,-10,20 );
	currot=toquaternion(Vector3f( 20.0, 0.0 , 0.0 ));
	campos=2;
	scale=Vector3f(1.0,1.0,1.0);


	//load menu

	myMenuManager = MenuManager::createAndInitialize();
	Menu* menu = myMenuManager->createMenu("menu");
	myMenuManager->setMainMenu(menu);

	simulation = menu->addItem(MenuItem::Checkbox);
	simulation->setText("Simulation");
	simulation->setChecked(true);
	simulation->setListener(this);


	for(int i=0;i<10;i++){
	modemenu[i]=menu->addItem(MenuItem::Checkbox);
	string str="Modeshape :";
	str+=char('0'+i);
	modemenu[i]->setText(str);
	modemenu[i]->setChecked(false);
	modemenu[i]->setListener(this);
	}

	magslider=menu->addItem(MenuItem::Slider);
	magslider->getSlider()->setTicks(150);

	//scanner
	xclip=0;
	mag=0;


	//init the lighting
	cyclops::Light* l = new Light(sceneMngr);
	l->setEnabled(true);
	l->setPosition(18, 0, 10);
	l->setColor(Color(1.0f, 1.0f, 1.0f));
	l->setAmbient(Color(0.0f, 0.0f, 0.0f));

	cyclops::Light* l1 = new Light(sceneMngr);
	l1->setEnabled(true);
	l1->setPosition(18, 0, -10);
	l1->setColor(Color(1.0f, 1.0f, 1.0f));
	l1->setAmbient(Color(0.0f, 0.0f, 0.0f));

	/*
	cyclops::Light* l2 = new Light(sceneMngr);
	l2->setEnabled(true);
	l2->setPosition(32.0,0.0,0.0);
	l2->setColor(Color(1.0f, 1.0f, 1.0f));
	l2->setAmbient(Color(0.0f, 0.0f, 0.0f));
	
	
	cyclops::Light* l3 = new Light(sceneMngr);
	l3->setEnabled(true);
	l3->setPosition(0.0,0.0,0.0);
	l3->setColor(Color(1.0f, 1.0f, 1.0f));
	l3->setAmbient(Color(0.0f, 0.0f, 0.0f));
	*/

	cyclops::Light* l4 = new Light(sceneMngr);
	l4->setEnabled(true);
	l4->setPosition(18.0,18.0,0.0);
	l4->setColor(Color(1.0f, 1.0f, 1.0f));
	l4->setAmbient(Color(0.0f, 0.0f, 0.0f));
	
	cyclops::Light* l5 = new Light(sceneMngr);
	l5->setEnabled(true);
	l5->setPosition(18.0,-18.0,0.0);
	l5->setColor(Color(1.0f, 1.0f, 1.0f));
	l5->setAmbient(Color(0.0f, 0.0f, 0.0f));
	
 	
	
	// Create skybox
	
	Skybox* sb = new Skybox();
	sb->loadCubeMap("skyboxes/grid4", "png");
	sceneMngr->setSkyBox(sb);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void OmegaViewer::onMenuItemEvent(MenuItem* mi) {
	simulation->setChecked(false);
	for(int i=0;i<10;i++){
		modemenu[i]->setChecked(false);
	}
	

	mi->setChecked(true);

	if(mi==simulation){
		modeshape->setEffect("Textured-border -s 0 -t -g 0");
		mymode->set(true);
		mode=0;
		curTimeStep=0;
		isKeyFraming=false;
	}

	for(int i=0;i<10;i++){
		if(mi==modemenu[i]){
		mode=i+1;
		mymode->set(false);
		modeshape->setEffect("Textured-modes -s 0 -t -g 0");
		modeshape->getGeometry()->setVertexAttribArray(6,mdef_data[i]);
		modeshape->getGeometry()->setVertexAttribBinding (6, osg::Geometry::BIND_PER_VERTEX );
		modeshape->getGeometry()->setVertexAttribArray(7,col_data);
		modeshape->getGeometry()->setVertexAttribBinding (7, osg::Geometry::BIND_PER_VERTEX );		
		curTimeStep=0;
		isKeyFraming=false;
		updateEntity ( wheelSetEntity , wheel_PVec , wheel_RVec, curTimeStep ); 
		updateEntity ( frameEntity , frame_PVec , frame_RVec, curTimeStep );
		break;
		}
	}

	if(mi==magslider && mode>0){
		curTimeStep=mi->getSlider()->getValue();
		isKeyFraming=true;
	}

}

void OmegaViewer::handleEvent(const Event& evt)
{
	//if(evt.getServiceType() == Service::UI) 
	//{
	//	handleUIEvent(evt);
	//}
	//else
	//{
	//	EngineClient::handleEvent(evt);
	//}

	if(evt.isKeyDown('A'))
    {
		//isAnimating = !isAnimating;
		//ofwarn("Animation set to :: %1%", %isAnimating);

		isKeyFraming = false;
		//reverse math to figure out where the keyFrame is time wise
		float curRatio = (float)curTimeStep / (float)numTimeSteps;
		curTime = (curRatio * animationTimer) - animationPadding; 
	}

	else if(evt.isKeyDown('c'))
    {
		isFreeFly = !isFreeFly;
		isKeyFraming = false;
		currot=getcamRot();
		curpos=getcamTrans();
	}
	
	//Set to the Hardcoded origin
	else if(evt.isKeyDown('h'))
    	{
		camTrans( Vector3f( 15.0 , 15.0 , 27.0 ) );
		camRot( Vector3f( 15.0 , 0.0 , 0.0 ) );
		camDefault();
	}
	
	//view 1
	else if(evt.isKeyDown('1'))
    {
		campos=1;	
	}

	//view 2
	else if(evt.isKeyDown('2'))
    {
		campos=2;
	}

	//view 3
	else if(evt.isKeyDown('3'))
    {
		campos=3;
	}
/*
	else if(evt.isButtonDown(Event::Button3)){
		isFreeFly=false;
		currot=getcamRot();
		curpos=getcamTrans();
		if(campos<=3){campos++;} else {campos=1;}
	}
*/
	else if(evt.isButtonDown(Event::Button5)){
	isKeyFraming = false;
		//reverse math to figure out where the keyFrame is time wise
		float curRatio = (float)curTimeStep / (float)numTimeSteps;
		curTime = (curRatio * animationTimer) - animationPadding; 

	}
	
	else if(evt.isButtonDown(Event::Button6)){
		isFreeFly = true;
	}	

	else if(evt.isButtonDown(Event::Button7)){
	
	}


	else if(evt.isButtonDown(Event::ButtonDown)){
//		scale=scale/2;
//		getEngine()->getScene()->setScale(scale);
		if(!myMenuManager->getMainMenu()->isVisible()){
		isFreeFly=false;
		currot=getcamRot();
		curpos=getcamTrans();
		if(campos<3){campos++;} else {campos=1;}
		}
	}

	else if(evt.isButtonDown(Event::ButtonUp)){
//		scale=scale*2;
	//	getEngine()->getScene()->setScale(scale);
		if(!myMenuManager->getMainMenu()->isVisible()){
		isFreeFly=false;
		currot=getcamRot();
		curpos=getcamTrans();
		if(campos>1){campos--;} else {campos=3;}
		}

	}


	else if(evt.isKeyDown('l'))
	{
		xclip++;
	}
	else if(evt.isKeyDown('k'))
	{
		xclip--;
	}


	//Enable Keyframing 
	//Set to the Hardcoded origin
	else if(evt.isKeyDown('o') || evt.isButtonDown(Event::ButtonLeft))
    	{
		isKeyFraming = true;
		if(curTimeStep>0){
		curTimeStep-=1; 
		prevTimeStep = curTimeStep;
		} else {
		curTimeStep=numTimeSteps;
		}
//		if(mcount>0){mcount--;} else {mcount=modeshape->getNumModels();}
	}
	//Enable Keyframing 
	else if(evt.isKeyDown('p') || evt.isButtonDown(Event::ButtonRight))
    	{
		isKeyFraming = true;
		if(curTimeStep<numTimeSteps){
		curTimeStep+=1;
		prevTimeStep=curTimeStep ;
		} else {curTimeStep=0;}
	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//   The rendering loop that is called thousands of times per sec 
//
void OmegaViewer::update(const UpdateContext& context) 
{
//	sceneMngr->getMainLight()->setPosition(getServer()->getDefaultCamera()->getPosition());
//	cout<<context.time<<endl;
	/*
	if(clipX == NULL)
	{
		clipX = new osg::Uniform("unif_ClipX",200.0f);
		osg::StateSet* ss = modeshape->getOsgNode()->getOrCreateStateSet();
		ss->addUniform(clipX);
	}

	if(transmode == NULL)
	{
		transmode = new osg::Uniform("unif_Trans",false);
		osg::StateSet* ss = modeshape->getOsgNode()->getOrCreateStateSet();
		ss->addUniform(transmode);
	}
	*/
//	clipX->set(xclip);
	

	if(!isFreeFly){
	camTrans(curpos);
	camRot(currot);
	} 
	switch(campos){
	case 1:
		
		curpos=Lerp(0.1,curpos,Vector3f( 55.0,0.0,20));
	//	curpos=Lerp(0.1,curpos,hoffset*toquaternion(Vector3f( 0.0, 0.0 , 0.0))+Vector3f( 20,0.0,20));
		currot=Slerp(0.1,currot,  toquaternion(Vector3f( 0.0, 0.0 , 0.0)));
		
		break;
	case 2:
		curpos=Lerp(0.1,curpos,Vector3f( 20,0.0,20));
	//	curpos=Lerp(0.1,curpos,hoffset*toquaternion(Vector3f( 0.0, 0.0 , 0.0))+Vector3f( 20,0.0,20));
		currot=Slerp(0.1,currot, toquaternion( Vector3f( 0.0, 0.0 , 0.0)));
		
		break;
	
	case 3:
		curpos=Lerp(0.1,curpos,Vector3f( 102.0 , 0.0 , 0.0 ));
	//	curpos=Lerp(0.1,curpos,hoffset*toquaternion(Vector3f( 0.0, 0.0 , 0.0))+Vector3f( 20,0.0,20));
		currot=Slerp(0.1,currot, toquaternion(  Vector3f( 90.0 , 90.0 , 0.0 )));
		break;
	}

	/*
	if( !isKeyFraming ){
	if(context.time>shapetime){
			if(mcount<modeshape->getNumModels()){mcount++;} else {mcount=0;}
			modeshape->setCurrentModelIndex(mcount);
			shapetime=context.time+0.05;
	}
	} else {
	    modeshape->setCurrentModelIndex(mcount);
	}
	*/
	//do camrea stuff
	toggleCameraController( );

	//if current time in millisec  is less than the animation time then animate
	if( (curTime < animationTimer && isAnimating) )
	{
		//not keyframing 
		if( !isKeyFraming )
		{
			//determien the ratio of time : current / totat time of animation
			//  % of the animation done in terms of time 
			float ratio = ( curTime + animationPadding) / (animationTimer );
			
			//Determine indea into the array based on this ratio = max num of idex * ratio 
			curTimeStep = (int)(numTimeSteps * ratio);	

			//incr the time based on some dt.
			curTime += context.dt;
		}

		if( curTimeStep > numTimeSteps) curTimeStep = numTimeSteps;



		if(mymode == NULL)
		{
		mymode = new osg::Uniform("unif_Mymode",true);
		osg::StateSet* s1 = modeshape->getOsgNode()->getOrCreateStateSet();
		s1->addUniform(mymode);
		}


		if (mode==0) {
		updateEntity ( wheelSetEntity , wheel_PVec , wheel_RVec, curTimeStep ); 
		updateEntity ( frameEntity , frame_PVec , frame_RVec, curTimeStep );
		if(curTimeStep<def_data.size() && curTimeStep>=0 && curTimeStep!=xclip){
		modeshape->getGeometry()->setVertexAttribArray(6,def_data[curTimeStep]);
		modeshape->getGeometry()->setVertexAttribBinding (6, osg::Geometry::BIND_PER_VERTEX );
		xclip=curTimeStep;
//		cout<<"current step= "<<curTimeStep<<endl; 
		}
		} else {
		if(magnifier == NULL)
		{
		magnifier = new osg::Uniform("unif_Mag",0);
		osg::StateSet* ss = modeshape->getOsgNode()->getOrCreateStateSet();
		ss->addUniform(magnifier);
		}
		
		if(curTimeStep>xclip){
			if(mag<=150){
			mag++;
			} else {mag=0;}
			magnifier->set(mag);

		xclip=curTimeStep;
		}
		
		if(curTimeStep<xclip){
			if(mag>0){
			mag--;
			} else {mag=150;}
			magnifier->set(mag);

		xclip=curTimeStep;
		}

		}

		
/*		  
		if(curTimeStep%4==0){
			
		if(curTimeStep/4<=modeshape->getNumModels()) modeshape->setCurrentModelIndex(curTimeStep/4);
		}
//		if(curTimeStep<=modeshape->getNumModels()) modeshape->setCurrentModelIndex(curTimeStep);
*/
		

	}
	else if( curTime >= animationTimer ) curTime = 0;	//reset the timer bc the animation should be done
	else if( !isAnimating ) return;						//do nothing cause animation has paused

	prevTimeStep = curTimeStep;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Updates a given entity based on the position and rot array and the desired index
void OmegaViewer::updateEntity( Entity* entity , vector<float> pos , vector<float> rot, int curTimeStep ) 
{
	//getting the actual vector index bc ea. quad is x, y, z, w for a time step
	int actualVecIndex = curTimeStep * 4;
	
	//grabbign the position quad
	Vector<float> position;
	position.push_back( pos[actualVecIndex+0]);
	position.push_back( pos[actualVecIndex+1]);
	position.push_back( pos[actualVecIndex+2]);
	position.push_back( pos[actualVecIndex+3]);

	//setting the position of the entity
	entity->setPosition( position[0] , position[1] , position[2] );

	//!!!!! HACK !!!!!//	
	//Data set has a weird rotation
	//Quaternion for a 90 deg X-axis rotation
	
//	Quaternion newAxis = AngleAxis(-Math::HalfPi, Vector3f::UnitX()) ;
	Quaternion rotation;
	rotation.x() = rot[actualVecIndex+0];
	rotation.y() = rot[actualVecIndex+1];
	rotation.z() = rot[actualVecIndex+2];
	rotation.w() = rot[actualVecIndex+3];
		
	//90 degree rotation will turn the object side ways
	//Apply the rotation
//	rotation = newAxis * rotation;
	
	entity->setOrientation( rotation );
}
///////////////////////////////////////////////////////////////////////////////////////////////////


void OmegaViewer::camRot( Vector3f pitchYawRoll )
{
	//ofwarn("Rotating Camera to :: %1%", %pitchYawRoll);
	getEngine()->getDefaultCamera()->setPitchYawRoll(pitchYawRoll * Math::DegToRad);
}

void OmegaViewer::camRot( Quaternion orientation )
{
	//ofwarn("Rotating Camera to :: %1%", %pitchYawRoll);
	getEngine()->getDefaultCamera()->setOrientation(orientation);
}

Quaternion OmegaViewer::getcamRot(){
	Quaternion q=getEngine()->getDefaultCamera()->getOrientation();
//	Vector3f rotation=Vector3f(q.getPitch(),q.getYaw(),q.getRoll());
	return q;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// 
void OmegaViewer::camTrans( Vector3f pos)
{
	//ofwarn("Moving Camera to :: %1%", %pos);
	getEngine()->getDefaultCamera()->setPosition(pos);
}

Vector3f OmegaViewer::getcamTrans()
{
	//ofwarn("Moving Camera to :: %1%", %pos);
	return getEngine()->getDefaultCamera()->getPosition();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 
void OmegaViewer::camDefault()
{
	isAnimating = true;
	isKeyFraming = false;

	//~~~~~~ Setup the camera stuff 
	isFreeFly = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// 
void OmegaViewer::toggleCameraController( )
{
	//enable/disable the free fly
	getEngine()->getDefaultCamera()->setControllerEnabled(isFreeFly);
}

Vector3f  OmegaViewer::Lerp(float t, Vector3f start,Vector3f end)
{
	return start*(1-t)+end*t;
}


Quaternion  OmegaViewer::Slerp(float t,Quaternion start,Quaternion end)
{
	if ( t <= 0.0f )
        {
            return start;
        }
   
        if ( t >= 1.0f )
        {
            return end;
        }

		float coshalftheta = start.dot(end);
		Quaternion c(end);
		
		if ( coshalftheta < 0.0f )
        {
            coshalftheta = -coshalftheta;
            c = Quaternion(-c.w(),-c.x(),-c.y(),-c.z());
        }


		if ( coshalftheta > 0.99f )
            {
				Quaternion result(	(1-t)*start.w()+t*end.w()		,	(1-t)*start.x()+t*end.x()	,	(1-t)*start.y()+t*end.y()		,	(1-t)*start.z()+t*end.z());
                    return result;
            }
		

		float halftheta = acos (coshalftheta);
        	float sintheta = sin (halftheta);
		float a=sin ((1.0f - t) * halftheta);
		float b=sin (t * halftheta);
		Quaternion result(	(a*start.w()+b*end.w())/sin (halftheta)		,	(a*start.x()+b*end.x())/sin (halftheta)	,	(a*start.y()+b*end.y())/sin (halftheta)		,	(a*start.z()+b*end.z())/sin (halftheta));
		
		return result;
}

Quaternion OmegaViewer::toquaternion(const Vector3f& yawPitchRoll) 
{ 
		Quaternion orientation =   AngleAxis(yawPitchRoll[0]*Math::DegToRad, Vector3f::UnitX()) * AngleAxis(yawPitchRoll[1]*Math::DegToRad, Vector3f::UnitY()) * AngleAxis(yawPitchRoll[2]*Math::DegToRad, Vector3f::UnitZ());
		return orientation;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Application entry point
int main(int argc, char** argv)
{
	Application<OmegaViewer> app("railSimmodeshapes");
	app.setName("railSim");
	return omain(app, argc, argv);
}
