/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
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

#include "BaseApplication.h"
#include <OgreGLPlugin.h>
#include <OgreParticleFXPlugin.h>
#include <OgreOctreePlugin.h>
#include <OgreBspSceneManagerPlugin.h>
#include <OgreOctreePlugin.h>
#include <OgreSTBICodec.h>

int exitKey;                    // Default exit key
char currentKeyState[512];      // Registers current frame key state
char previousKeyState[512];     // Registers previous frame key state
int keyPressedQueue[MAX_KEY_PRESSED_QUEUE];     // Input keys queue
int keyPressedQueueCount;       // Input keys queue count
int charPressedQueue[MAX_CHAR_PRESSED_QUEUE];   // Input characters queue
int charPressedQueueCount;      // Input characters queue count
char currentButtonState[3];     // Registers current mouse button state
char previousButtonState[3];    // Registers previous mouse button state
float currentWheelMove;         // Registers current mouse wheel variation
float previousWheelMove;        // Registers previous mouse wheel variation
int mMouseX;
int mMouseY;

Ogre::GLPlugin* mGLPlugin;
Ogre::ParticleFXPlugin* mParticleFXPlugin;
Ogre::STBIPlugin* mSTBImagePlugin;
Ogre::OctreePlugin* mOctreePlugin;
Ogre::BspSceneManagerPlugin* mBSPPlugin;

CameraMan::CameraMan(Ogre::Camera* cam)
    : mYawSpace(Ogre::Node::TS_PARENT)
    , mCamera(0)
    , mStyle(CS_MANUAL)
    , mTarget(0)
    , mOrbiting(false)
    ,mZooming(false)
    , mMoving(false)
    , mTopSpeed(150)
    , mVelocity(Ogre::Vector3::ZERO)
     , mFastMove(false)
    , mOffset(0, 0, 0)
{

    setCamera(cam);
    setStyle(CS_FREELOOK);
}


void CameraMan::setTarget(Ogre::SceneNode *target)
{
    	if (target != mTarget)
			{
				mTarget = target;
				if(target)
				{
					setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
					mCamera->setAutoTracking(true, mTarget);
				}
				else
				{
					mCamera->setAutoTracking(false);
				}

			}
}

void CameraMan::setYawPitchDist(const Ogre::Radian&  yaw, const Ogre::Radian& pitch, Ogre::Real dist)
{
    	    mCamera->setPosition(mTarget->_getDerivedPosition());
			mCamera->setOrientation(mTarget->_getDerivedOrientation());
			mCamera->yaw(yaw);
			mCamera->pitch(-pitch);
			mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
}

void CameraMan::setStyle(CameraStyle style)
{
    if (mStyle != CS_ORBIT && style == CS_ORBIT)
			{
				setTarget(mTarget ? mTarget : mCamera->getSceneManager()->getRootSceneNode());
				mCamera->setFixedYawAxis(true);
				manualStop();
				setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);

			}
			else if (mStyle != CS_FREELOOK && style == CS_FREELOOK)
			{
				mCamera->setAutoTracking(false);
				mCamera->setFixedYawAxis(true);
			}
			else if (mStyle != CS_MANUAL && style == CS_MANUAL)
			{
				mCamera->setAutoTracking(false);
				manualStop();
			}
			mStyle = style;
}

void CameraMan::manualStop()
{
    if (mStyle == CS_FREELOOK)
    {
        mVelocity = Ogre::Vector3::ZERO;
    }
}



Ogre::Real CameraMan::getDistToTarget()
{
    Ogre::Vector3 offset = mCamera->getPosition() - mTarget->_getDerivedPosition() - mOffset;
    return offset.length();
}

void CameraMan::setPivotOffset(const Ogre::Vector3& pivot)
{
    Ogre::Real dist = getDistToTarget();
    mOffset = pivot;
    mCamera->setPosition(mTarget->_getDerivedPosition() + mOffset);
    mCamera->move(Ogre::Vector3(0, 0, dist));
}

void CameraMan::MouseLook(float xrel, float yrel)
{

            if (mStyle == CS_ORBIT)
			{

                mOrbiting  = IsMouseButtonDown(0);
                mMoving    = IsMouseButtonDown(2);
                mZooming   = IsMouseButtonDown(1);


				Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();

				if (mOrbiting)   // yaw around the target, and pitch locally
				{
					mCamera->setPosition(mTarget->_getDerivedPosition());

					mCamera->yaw(Ogre::Degree(-xrel * 0.25f));
					mCamera->pitch(Ogre::Degree(-yrel * 0.25f));

					mCamera->moveRelative(Ogre::Vector3(0, 0, dist));

					// don't let the camera go over the top or around the bottom of the target
				}
				   else if (mMoving)  // move the camera along the image plane
                {


                    Ogre::Vector3 delta = mCamera->getOrientation() * Ogre::Vector3(-xrel, yrel, 0);
                    // the further the camera is, the faster it moves
                    delta *= dist / 1000.0f;
                    mOffset += delta;
                    mCamera->move(delta);

                }
				else if (mZooming)  // move the camera toward or away from the target
				{
					// the further the camera is, the faster it moves
					mCamera->moveRelative(Ogre::Vector3(0, 0, yrel * 0.004f * dist));
				}
				else if (GetMouseWheelMove() != 0)  // move the camera toward or away from the target
				{
					// the further the camera is, the faster it moves
					mCamera->moveRelative(Ogre::Vector3(0, 0, -GetMouseWheelMove() * 0.0008f * dist));
				}
			}
			else if (mStyle == CS_FREELOOK)
			{
				mCamera->yaw(Ogre::Degree(-xrel * 0.15f));
				mCamera->pitch(Ogre::Degree(-yrel * 0.15f));
				}

}



void CameraMan::mouseWheelRolled(int value)
{
    if (mStyle == CS_ORBIT && value != 0)
    {
        Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
        mCamera->move(Ogre::Vector3(0, 0, -value * 0.08f * dist));
    }

}




//---------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    camera(0),
    mShutDown(false),
    window(0)


{
SDL_LogInfo(0,"*** Initializing Base ***");

    charPressedQueueCount=0;
    keyPressedQueueCount=0;
    previousWheelMove = 0.0f;
    currentWheelMove = 0.0f;
}

//---------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
SDL_LogInfo(0,"*** Destroy Base ***");

destroyScene();

if (camera) delete camera;


// Remove ourself as a Window listener

windowClosed(mWindow);
mRoot->destroySceneManager(mSceneMgr);

mRoot->queueEndRendering();

   OGRE_DELETE mRoot;
    if (mGLPlugin) OGRE_DELETE mGLPlugin;
    if(mParticleFXPlugin) OGRE_DELETE mParticleFXPlugin;
    if(mSTBImagePlugin) OGRE_DELETE mSTBImagePlugin;
    if(mOctreePlugin) OGRE_DELETE mOctreePlugin;
    if(mBSPPlugin) OGRE_DELETE mBSPPlugin;



SDL_DestroyWindow(window);


}


Ogre::RenderWindow* BaseApplication::createWindow(const Ogre::String& name, Ogre::uint32 w, Ogre::uint32 h, Ogre::NameValuePairList miscParams)
{

Ogre::LogManager::getSingletonPtr()->logMessage("*** Create SDL  Window***");


 Ogre::RenderWindow* ret;



 Ogre::RenderSystemList::const_iterator renderers = (mRoot->getAvailableRenderers().begin());

 while(renderers != mRoot->getAvailableRenderers().end())
  {
      Ogre::String rName = (*renderers)->getName();
      SDL_Log("%s",rName.c_str());
        if (rName == "OpenGL Rendering Subsystem")
            break;
        renderers++;
  }

  Ogre::RenderSystem *renderSystem = *renderers;


//  assert( !renderers->empty() ); // we need at least one renderer to do anything useful


  assert( renderSystem ); // user might pass back a null renderer, which would be bad!
  mRoot->setRenderSystem( renderSystem );

   Ogre::String res;
  res  = Ogre::StringConverter::toString(w);
  res += "x";
  res += Ogre::StringConverter::toString(h);

 // renderSystem->setConfigOption( "Video Mode", res);
 // mRoot->getRenderSystem()->setConfigOption( "Full Screen", "No" );
  //mRoot->saveConfig();
  mRoot->initialise(false); // don't create a window
  mRoot->saveConfig();

  window =        SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w,h, SDL_WINDOW_RESIZABLE );
   //gl_context = SDL_GL_CreateContext(window);
     //   SDL_GL_MakeCurrent(window, gl_context);
   SDL_GL_SetSwapInterval(0); // Enable vsync


SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);


      Ogre::NameValuePairList params;

    // for tiny rendersystem
params["sdlwin"] = Ogre::StringConverter::toString(size_t(window));
params["externalWindowHandle"] = Ogre::StringConverter::toString(size_t(wmInfo.info.x11.window));



ret = mRoot->createRenderWindow( name,
                      w,h,
                       false,
                       &params );

ret->setActive(true);


    return ret;
}

//---------------------------------------------------------------------------
void BaseApplication::writeRenderToFile(const Ogre::String& filename)
{
//    Ogre::Image img(Ogre::PF_A8B8G8R8, mWindow->getWidth(), mWindow->getHeight());
//    Ogre::PixelBox pb = img.getPixelBox();
 //   mWindow->copyContentsToMemory(pb, pb);
 //   img.save(filename);
}
void BaseApplication::TakeScreenshot(const char *fileName)
{
 mWindow->writeContentsToFile(fileName);
SDL_LogInfo(0, "SYSTEM: [%s] Screenshot taken successfully", fileName);
}

bool BaseApplication::configure(void)
{



        mWindow=createWindow("Ogre3D",1020,780,Ogre::NameValuePairList());
        Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
        Ogre::Root::getSingleton().clearEventTimes();
        //Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);
            //Ogre::LogManager::getSingleton().setMinLogLevel(Ogre::LML_TRIVIAL);
        mRoot->saveConfig();


        return true;
/*

    // Show the configuration dialog and initialise the system.
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg.
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

        mRoot->saveConfig();

        return true;
    }
    else
    {
        return false;
    }*/
}
//---------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);



}
//---------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,0,80));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);

    camera = new CameraMan(mCamera);


}
//---------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{


    // Set initial mouse clipping size
    windowResized(mWindow);




    mRoot->addFrameListener(this);
}
//---------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{

}
//---------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0.4f));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//---------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("/media/djoker/code/linux/cpp/ogre/samples/Ogre/resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;


            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//---------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{
}
//---------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//---------------------------------------------------------------------------
void BaseApplication::go(void)
{

   if (!setup())
        return;

    mRoot->startRendering();


}
//---------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
    mRoot = new Ogre::Root("");

//load plugins
mGLPlugin = OGRE_NEW Ogre::GLPlugin();
mRoot->installPlugin(mGLPlugin);

mParticleFXPlugin = OGRE_NEW Ogre::ParticleFXPlugin();
mRoot->installPlugin(mParticleFXPlugin);


mSTBImagePlugin = OGRE_NEW Ogre::STBIPlugin();
mRoot->installPlugin(mSTBImagePlugin);

mOctreePlugin =OGRE_NEW Ogre::OctreePlugin();
mRoot->installPlugin(mOctreePlugin);

mBSPPlugin=OGRE_NEW Ogre::BspSceneManagerPlugin();
mRoot->installPlugin(mBSPPlugin);

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
void BaseApplication::pollEvents()
{





            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    //mRoot->queueEndRendering();
                    mShutDown=true;

                    break;
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                     {
                        mWindow->resize(event.window.data1, event.window.data2);
                        }
                    break;
                default:
                    InputEvent(event);

                    break;
                }
            }


            Uint32 mouse_buttons = SDL_GetMouseState(&mMouseX, &mMouseY);
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_ESCAPE])
            {
            //mRoot->queueEndRendering();
            mShutDown=true;
            }
}

static void KeyCallback( int key,  int action)
{

    if (action == 0) currentKeyState[key] = 0;
    else currentKeyState[key] = 1;

    // Check if there is space available in the key queue
    if ((keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == 1))
    {
        // Add character to the queue
        keyPressedQueue[keyPressedQueueCount] = key;
        keyPressedQueueCount++;
    }

}
  static void CharCallback( unsigned int key)
{
    if (charPressedQueueCount < MAX_KEY_PRESSED_QUEUE)
    {
        charPressedQueue[charPressedQueueCount] = key;
        charPressedQueueCount++;
    }
}

  void BaseApplication::InputEvent(const SDL_Event& in)
        {

            switch (in.type)
            {
            case SDL_KEYDOWN:
            {
            if (in.key.repeat) break;
              KeyboardEvent key;
              key.repeat     = in.key.repeat;
              key.keysym.sym = in.key.keysym.sym;
              key.keysym.mod = in.key.keysym.mod;
              keyPressed(key);
              KeyCallback(in.key.keysym.scancode,1);
              CharCallback(in.key.keysym.sym);


            }
            break;
            case SDL_KEYUP:
            {
              KeyboardEvent key;
              key.repeat     = in.key.repeat;
              key.keysym.sym = in.key.keysym.sym;
              key.keysym.mod = in.key.keysym.mod;
              keyReleased(key);
              KeyCallback(in.key.keysym.scancode,0);
            }

                break;
           case SDL_TEXTINPUT:
                textInput(in.text.text);




            break;

            case SDL_MOUSEMOTION:
            {
            MouseMotionEvent motion;
            motion.x = in.motion.x;
            motion.y = in.motion.y;
            motion.xrel = in.motion.xrel;
            motion.yrel = in.motion.yrel;
            mouseMoved(motion);
            }
             break;

            case SDL_MOUSEBUTTONDOWN:
            {
            MouseButtonEvent button;

            button.x = in.button.x;
            button.y = in.button.y;
            button.button = in.button.button;
            button.clicks = in.button.clicks;
            mousePressed(button);
            int MouseButton = in.button.button-1;
            currentButtonState[MouseButton] = 1;


            }

                break;
            case SDL_MOUSEBUTTONUP:

            {
            MouseButtonEvent button;

            button.x = in.button.x;
            button.y = in.button.y;
            button.button = in.button.button;
            button.clicks = in.button.clicks;

            mouseReleased(button);


            int MouseButton = in.button.button-1;
            currentButtonState[MouseButton] = 0;

            }

                break;
            case SDL_MOUSEWHEEL:
                mouseWheel(in.wheel.y);
                previousWheelMove=in.wheel.y;
                break;

            case SDL_FINGERDOWN:
                break;
            case SDL_FINGERUP:
                break;
            case SDL_FINGERMOTION:
                break;
            }
        }
bool BaseApplication::frameStarted(const Ogre::FrameEvent& evt)
{
    keyPressedQueueCount = 0;
    charPressedQueueCount = 0;
    // Register previous keys states
    for (int i = 0; i < 512; i++) previousKeyState[i] = currentKeyState[i];

    // Register previous mouse states
    for (int i = 0; i < 3; i++) previousButtonState[i] =currentButtonState[i];

    // Register previous mouse wheel state
    previousWheelMove = currentWheelMove;
    currentWheelMove = 0.0f;

    pollEvents();

    OnUpdate(evt.timeSinceLastFrame);

  // SDL_LogInfo(0,"start %f %f ",evt.timeSinceLastEvent,evt.timeSinceLastFrame);
   return true;
}

bool BaseApplication::frameEnded(const Ogre::FrameEvent& evt)
{

//SDL_LogInfo(0,"end %f %f ",evt.timeSinceLastEvent,evt.timeSinceLastFrame);

return true;
}
bool BaseApplication::OnRender()
{

}
bool BaseApplication::OnUpdate(float dt)
{
}



//---------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

   if(mShutDown)
        return false;




    OnRender();

  //  SDL_LogInfo(0,"render %f %f ",evt.timeSinceLastEvent,evt.timeSinceLastFrame);
    return true;
}
//---------------------------------------------------------------------------
// Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

}
//---------------------------------------------------------------------------

void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
        rw->removeAllViewports();
    //    mRoot->detachRenderTarget(rw);
            SDL_DestroyWindow(window);
}
//---------------------------------------------------------------------------

bool IsKeyPressed(int key)
{
    bool pressed = false;
    if ((previousKeyState[key] == 0) && (currentKeyState[key] == 1)) pressed = true;
    else pressed = false;

    return pressed;
}
bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();
    if ((key >= 32) && (key <= 126)) keyPressed = true;
    return keyPressed;
}


bool IsKeyDown(int key)
{
    if (currentKeyState[key] == 1) return true;
    else return false;
}


bool IsKeyReleased(int key)
{
    bool released = false;
    if ((previousKeyState[key] == 1) && (currentKeyState[key] == 0)) released = true;
    else released = false;
    return released;
}


bool IsKeyUp(int key)
{
    if (currentKeyState[key] == 0) return true;
    else return false;
}


int GetKeyPressed(void)
{
 int value = 0;

    if (keyPressedQueueCount > 0)
    {
        // Get character from the queue head
        value = keyPressedQueue[0];

        // Shift elements 1 step toward the head.
        for (int i = 0; i < (keyPressedQueueCount - 1); i++)
            keyPressedQueue[i] = keyPressedQueue[i + 1];

        // Reset last character in the queue
        keyPressedQueue[keyPressedQueueCount] = 0;
        keyPressedQueueCount--;
    }
}


int GetCharPressed(void)
{
    int value = 0;

    if (charPressedQueueCount > 0)
    {
            // Get character from the queue head
            value = charPressedQueue[0];

            // Shift elements 1 step toward the head.
            for (int i = 0; i < (charPressedQueueCount - 1); i++)
            charPressedQueue[i] = charPressedQueue[i + 1];

            // Reset last character in the queue
            charPressedQueue[charPressedQueueCount] = 0;
            charPressedQueueCount--;
    }

    return value;
}




// Detect if a mouse button has been pressed once
bool IsMouseButtonPressed(int button)
{
    bool pressed = false;

    if ((currentButtonState[button] == 1) && (previousButtonState[button] == 0)) pressed = true;



    return pressed;
}

// Detect if a mouse button BaseApplication::Is being pressed
bool IsMouseButtonDown(int button)
{
    bool down = false;

    if (currentButtonState[button] == 1) down = true;



    return down;
}

// Detect if a mouse button has been released once
bool IsMouseButtonReleased(int button)
{
    bool released = false;

    if ((currentButtonState[button] == 0) && (previousButtonState[button] == 1)) released = true;



    return released;
}

// Detect if a mouse button BaseApplication::Is NOT being pressed
bool IsMouseButtonUp(int button)
{
    return !IsMouseButtonDown(button);
}

// Returns mouse position X
int GetMouseX(void)
{
return mMouseX;
}
int GetMouseY(void)
{
return mMouseY;
}
float GetMouseWheelMove(void)
{
return previousWheelMove;

}

Ogre::Vector2 GetMousePosition()
{
return Ogre::Vector2(mMouseX,mMouseY);
}
