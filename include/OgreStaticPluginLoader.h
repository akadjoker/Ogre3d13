/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    StaticPluginLoader.h
Description: Utility class to load plugins statically
-----------------------------------------------------------------------------
*/

#ifndef __StaticPluginLoader_H__
#define __StaticPluginLoader_H__

#include "Ogre.h"
#include "OgreConfigFile.h"

// Static plugin headers
#include "OgreOctreePlugin.h"
#include "OgreParticleFXPlugin.h"
#include "OgreBspSceneManagerPlugin.h"
#include "OgreGLPlugin.h"
//#include "OgreGLESPlugin.h"


//#  include "OgrePCZPlugin.h"
//#  include "OgreOctreeZonePlugin.h"


namespace Ogre
{
	/** Utility class for loading some plugins statically.
	@remarks
		When loading plugins statically, you are limited to loading plugins
		that are known about at compile time. You should define preprocessor
		symbols depending on which plugins you want to load - the symbol being
		OGRE_STATIC_<pluginname>, with pluginname being the usual name of the
		plugin DLL (no file extension, no debug suffix, and without the Plugin_
		or RenderSystem_ prefix.)
	*/
	class StaticPluginLoader
	{
	protected:
		Ogre::OctreePlugin* mOctreePlugin;
		Ogre::ParticleFXPlugin* mParticleFXPlugin;
		Ogre::BspSceneManagerPlugin* mBSPPlugin;
		Ogre::GLPlugin* mGLPlugin;

		//Ogre::PCZPlugin* mPCZPlugin;
		//Ogre::OctreeZonePlugin* mOctreeZonePlugin;


#ifdef OGRE_STATIC_GLES
		Ogre::GLESPlugin* mGLESPlugin;
#endif

	public:
		StaticPluginLoader() {}

		/** Load all the enabled plugins against the passed in root object. */
		void load()
		{
			Root& root  = Ogre::Root::getSingleton();
			mGLPlugin = OGRE_NEW Ogre::GLPlugin();
			root.installPlugin(mGLPlugin);


//			mGLESPlugin = OGRE_NEW Ogre::GLESPlugin();
//			root.installPlugin(mGLESPlugin);



			mParticleFXPlugin = OGRE_NEW Ogre::ParticleFXPlugin();
			root.installPlugin(mParticleFXPlugin);
			mBSPPlugin = OGRE_NEW Ogre::BspSceneManagerPlugin();
			root.installPlugin(mBSPPlugin);
			mOctreePlugin = OGRE_NEW Ogre::OctreePlugin();
			root.installPlugin(mOctreePlugin);


/*
			mPCZPlugin = OGRE_NEW Ogre::PCZPlugin();
			root.installPlugin(mPCZPlugin);
			mOctreeZonePlugin = OGRE_NEW Ogre::OctreeZonePlugin();
			root.installPlugin(mOctreeZonePlugin);
			*/
		}


		void unload()
		{
			// don't unload plugins, since Root will have done that. Destroy here.
//			OGRE_DELETE mOctreeZonePlugin;
//			OGRE_DELETE mPCZPlugin;

			OGRE_DELETE mBSPPlugin;
			OGRE_DELETE mParticleFXPlugin;
			OGRE_DELETE mOctreePlugin;
			OGRE_DELETE mGLPlugin;

//			OGRE_DELETE mGLESPlugin;

		}

	};

}

#endif
