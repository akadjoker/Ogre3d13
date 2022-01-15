/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/


#include <iostream>


using namespace std;


#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0



#include "TutorialApplication.h"
/*
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513
*/

using namespace Ogre;
/*

	TerrainGlobalOptions* mTerrainGlobals;
	TerrainGroup* mTerrainGroup;
	bool mPaging=false;
	bool mTerrainsImported=false;
	TerrainPaging* mTerrainPaging;
	PageManager* mPageManager;
	Ogre::Vector3 mTerrainPos=Ogre::Vector3(1000,0,5000);





	void getTerrainImage(bool flipX, bool flipY, Image& img)
	{
		img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		if (flipX)
			img.flipAroundY();
		if (flipY)
			img.flipAroundX();

	}

		void defineTerrain(long x, long y, bool flat = false)
	{
		// if a file is available, use it
		// if not, generate file from import

		// Usually in a real project you'll know whether the compact terrain data is
		// available or not; I'm doing it this way to save distribution size

		if (flat)
		{
			mTerrainGroup->defineTerrain(x, y, 0.0f);
		}
		else
		{
			String filename = mTerrainGroup->generateFilename(x, y);
			if (ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
			{
				mTerrainGroup->defineTerrain(x, y);
			}
			else
			{
				Image img;
				getTerrainImage(x % 2 != 0, y % 2 != 0, img);
				mTerrainGroup->defineTerrain(x, y, &img);
				mTerrainsImported = true;
			}

		}
	}

	void initBlendMaps(Terrain* terrain)
	{
		TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
		TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
		Real minHeight0 = 70;
		Real fadeDist0 = 40;
		Real minHeight1 = 70;
		Real fadeDist1 = 15;
		float* pBlend1 = blendMap1->getBlendPointer();
		for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
		{
			for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
			{
				Real tx, ty;

				blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
				Real height = terrain->getHeightAtTerrainPosition(tx, ty);
				Real val = (height - minHeight0) / fadeDist0;
				val = Math::Clamp(val, (Real)0, (Real)1);
				//*pBlend0++ = val;

				val = (height - minHeight1) / fadeDist1;
				val = Math::Clamp(val, (Real)0, (Real)1);
				*pBlend1++ = val;


			}
		}
		blendMap0->dirty();
		blendMap1->dirty();
		//blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		blendMap0->update();
		blendMap1->update();



	}

	void configureTerrainDefaults(Light* l,Ogre::SceneManager*         mSceneMgr)
	{
		// Configure global
		mTerrainGlobals->setMaxPixelError(8);
		// testing composite map
		mTerrainGlobals->setCompositeMapDistance(3000);
		//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
		//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
		//mTerrainGlobals->setLightMapSize(256);


		//matProfile->setLightmapEnabled(false);
		// Important to set these so that the terrain knows what to use for derived (non-realtime) data
		mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
		mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
		//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
		mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

		// Configure default import settings for if we use imported image
		Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
		defaultimp.terrainSize = TERRAIN_SIZE;
		defaultimp.worldSize = TERRAIN_WORLD_SIZE;
		defaultimp.inputScale = 600;
		defaultimp.minBatchSize = 33;
		defaultimp.maxBatchSize = 65;

		// textures

		defaultimp.layerList.resize(3);
		defaultimp.layerList[0].worldSize = 100;
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.png");
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.png");
		defaultimp.layerList[1].worldSize = 30;
		defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.png");
		defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.png");
		defaultimp.layerList[2].worldSize = 200;
		defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.png");
		defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.png");


	}
*/
//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

Vector2 previousMousePosition;

bool TutorialApplication::OnUpdate(float dt)
{


/*		if (mTerrainGroup->isDerivedDataUpdateInProgress())
		{

			if (mTerrainsImported)
			{
				SDL_LogInfo(0,"Building terrain, please wait...");
			}
			else
			{
				SDL_LogInfo(0,"Updating textures, patience...");
			}

		}
*/
Vector2 mousePositionDelta =Vector2::ZERO;
Vector2 mousePosition = GetMousePosition();
mousePositionDelta.x = mousePosition.x - previousMousePosition.x;
mousePositionDelta.y = mousePosition.y - previousMousePosition.y;
previousMousePosition = mousePosition;

float time =0.12f;


if (IsMouseButtonDown(0))
  {
  camera->MouseLook(mousePositionDelta.x,mousePositionDelta.y);
  }


  if (IsKeyDown(KEY_W))
  {
   camera->MoveForward(dt);
  } else
  if (IsKeyDown(KEY_S))
  {
  camera->MoveBack(dt);
  }

  if (IsKeyDown(KEY_A))
  {
   camera->MoveLeft(dt);
  } else
  if (IsKeyDown(KEY_D))
  {
  camera->MoveRight(dt);
  }

 return true;
}
bool LoadImage(const Ogre::String& texture_name, const Ogre::String& texture_path)
{
	bool image_loaded = false;
	std::ifstream ifs(texture_path.c_str(), std::ios::binary|std::ios::in);
	if (ifs.is_open())
	{
		Ogre::String tex_ext;
		Ogre::String::size_type index_of_extension = texture_path.find_last_of('.');
		if (index_of_extension != Ogre::String::npos)
		{
			tex_ext = texture_path.substr(index_of_extension+1);

			SDL_LogError(0,"Load Image");

			Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(texture_path, &ifs, false));
			Ogre::Image img;
			img.load(data_stream, tex_ext);
			Ogre::TextureManager::getSingleton().loadImage(texture_name,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, img, Ogre::TEX_TYPE_2D, 0, 1.0f);

			img.save("save.png");

			image_loaded = true;
		} else{
		SDL_LogError(0,"NO Im,age");
		}
		ifs.close();
	} else
	{
	SDL_LogError(0,"NO Im,age");
	}
	return image_loaded;
}
//---------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
SDL_LogInfo(0,"Create Scene");

/*
	MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Plane(Vector3::UNIT_Y, 0), 1000, 1000, 20, 20, true, 1, 6, 6, Vector3::UNIT_Z);

		// create a ground entity from our mesh and attach it to the origin
		Entity* ground = mSceneMgr->createEntity("Ground", "ground");
	//	ground->setMaterialName("Examples/GrassFloor");
		ground->setCastShadows(false);
		mSceneMgr->getRootSceneNode()->attachObject(ground);
*/


		MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
		MaterialManager::getSingleton().setDefaultAnisotropy(7);



	//	mSceneMgr->setFog(FOG_LINEAR, ColourValue(0.7, 0.7, 0.8), 0, 10000, 25000);

		LogManager::getSingleton().setLogDetail(LL_BOREME);
/*
		Vector3 lightdir(0.55, -0.3, 0.75);
		lightdir.normalise();


		Light* l = mSceneMgr->createLight("tstLight");
		l->setType(Light::LT_DIRECTIONAL);
		l->setDirection(lightdir);
		l->setDiffuseColour(ColourValue::White);
		l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

		mSceneMgr->setAmbientLight(ColourValue(0.8, 0.8, 0.8));


        mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();

		mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
		mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
		mTerrainGroup->setOrigin(mTerrainPos);

		configureTerrainDefaults(l,mSceneMgr);






		bool blankTerrain = false;

		for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
			for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
				defineTerrain(x, y, blankTerrain);
		// sync load since we want everything in place when we start
		mTerrainGroup->loadAllTerrains(true);


		if (mTerrainsImported)
		{
			TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
			while(ti.hasMoreElements())
			{
				Terrain* t = ti.getNext()->instance;
				initBlendMaps(t);
			}
		}

		mTerrainGroup->freeTemporaryResources();


        mCamera->setPosition(mTerrainPos + Vector3(1683, 50, 2116));
		mCamera->lookAt(Vector3(1963, 50, 1660));
		mCamera->setNearClipDistance(0.1);
		mCamera->setFarClipDistance(50000);

		if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
        {
            mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
        }

        	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
*/
//
	//	mCamera->setPosition(0, 50, 100);


}


void TutorialApplication::destroyScene(void)
{
SDL_LogInfo(0,"Destroy Scene");



}

int main(int argc, char *argv[])

{

    SDL_Init(SDL_INIT_EVERYTHING ) ;
    SDL_LogInfo(0,"SDL Start");
    TutorialApplication* app= new TutorialApplication();

    try
    {
        app->go();
    } catch(Ogre::Exception& e)
    {

        std::cerr << "An exception has occurred: " <<                e.getFullDescription().c_str() << std::endl;


    }

    delete app;


    SDL_LogInfo(0,"SDL Quit");
    SDL_Quit();
    return 0;
}



//---------------------------------------------------------------------------
