#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <Ogre.h>
#include <OgreConfigDialog.h>
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreLogManager.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgrePlugin.h>

#include <OgreGLPlugin.h>
#include <OgreParticleFXPlugin.h>
#include <OgreOctreePlugin.h>
#include <OgreBspSceneManagerPlugin.h>
#include <OgreOctreePlugin.h>
#include <OgreSTBICodec.h>

#include <iostream>

using namespace Ogre;
using namespace std;

#define MAX_TOUCH_POINTS              10
#define MAX_KEY_PRESSED_QUEUE         16
#define MAX_CHAR_PRESSED_QUEUE        16

#define KEY_RIGHT           79
#define KEY_LEFT            80
#define KEY_DOWN            81
#define KEY_UP              82
#define KEY_BACKSPACE       42
#define KEY_ENTER           40
#define MOUSE_LEFT_BUTTON     1
#define KEY_DELETE          76
#define KEY_HOME           74
#define KEY_END             77

#define KEY_LEFT_CONTROL  224
#define KEY_LEFT_SHIFT  225
#define KEY_LEFT_ALT  226
#define KEY_LEFT_GUI  227
#define KEY_RIGHT_CONTROL  228
#define KEY_RIGHT_SHIFT  229
#define KEY_RIGHT_ALT  230

#define KEY_A  4
#define KEY_B  5
#define KEY_C  6
#define KEY_D  7
#define KEY_E  8
#define KEY_F  9
#define KEY_G  10
#define KEY_H  11
#define KEY_I  12
#define KEY_J  13
#define KEY_K  14
#define KEY_L  15
#define KEY_M  16
#define KEY_N  17
#define KEY_O  18
#define KEY_P  19
#define KEY_Q  20
#define KEY_R  21
#define KEY_S  22
#define KEY_T  23
#define KEY_U  24
#define KEY_V  25
#define KEY_W  26
#define KEY_X  27
#define KEY_Y  28
#define KEY_Z  29

#define KEY_1  30
#define KEY_2  31
#define KEY_3  32
#define KEY_4  33
#define KEY_5  34
#define KEY_6  35
#define KEY_7  36
#define KEY_8  37
#define KEY_9  38
#define KEY_0  39


struct Keysym
{
    int sym;
    unsigned short mod;
};

struct KeyboardEvent
{
    int type;
    Keysym keysym;
    unsigned char repeat;
};
struct MouseMotionEvent {
    int type;
    int x, y;
    int xrel, yrel;

};
struct MouseButtonEvent {
    int type;
    int x, y;
    unsigned char button;
    unsigned char clicks;
};

struct TouchFingerEvent {
    int type;
    int fingerId;
    float x, y;
    float dx, dy;
};

struct AxisEvent {
    int type;
    int which;
    unsigned char axis;
    short value;
};
struct ButtonEvent {
    int type;
    int which;
    unsigned char button;
};
//---------------------------------------------------------------------------

bool IsKeyPressed(int key);
bool IsAnyKeyPressed();
bool IsKeyDown(int key);
bool IsKeyReleased(int key);
bool IsKeyUp(int key);
int GetKeyPressed(void);
int GetCharPressed(void);
bool IsMouseButtonPressed(int button);
bool IsMouseButtonDown(int button);
bool IsMouseButtonReleased(int button);
bool IsMouseButtonUp(int button);
int GetMouseX(void);
int GetMouseY(void);
float GetMouseWheelMove(void);
Ogre::Vector2 GetMousePosition();

 enum CameraStyle   /// enumerator values for different styles of camera movement
    {
        CS_FREELOOK,
        CS_ORBIT,
        CS_MANUAL
    };


    class  CameraMan
    {
    public:
        CameraMan(Ogre::SceneNode* cam):
        mYawSpace(Ogre::Node::TS_PARENT)
    , mCamera(0)
    , mStyle(CS_MANUAL)
    , mTarget(0)
    , mOrbiting(false)
    , mMoving(false)
    , mTopSpeed(150)
    , mVelocity(Ogre::Vector3::ZERO)

    , mFastMove(false)
    , mOffset(0, 0, 0)
{

    setCamera(cam);
    setStyle(CS_FREELOOK);

}

        void setCamera(Ogre::SceneNode *cam)
        {
        mCamera = cam;
        }

        void setTarget(Ogre::SceneNode *target)
        {
           if (target == mTarget)          return;
          mTarget = target;
        }

        Ogre::SceneNode* getCamera()
        {
            return mCamera;
        }

        void setStyle(CameraStyle style)
        {
            if (mStyle != CS_ORBIT && style == CS_ORBIT)
            {
                setTarget(mTarget ? mTarget : mCamera->getCreator()->getRootSceneNode());
                // fix the yaw axis if requested
                mCamera->setFixedYawAxis(mYawSpace == Ogre::Node::TS_PARENT);
                manualStop();

                // try to replicate the camera configuration
                Ogre::Real dist = getDistToTarget();
                const Ogre::Quaternion& q = mCamera->getOrientation();
                setYawPitchDist(q.getYaw(), q.getPitch(), dist == 0 ? 150 : dist); // enforce some distance
            }
            else if (mStyle != CS_FREELOOK && style == CS_FREELOOK)
            {

                mCamera->setFixedYawAxis(true); // also fix axis with lookAt calls
            }
            else if (mStyle != CS_MANUAL && style == CS_MANUAL)
            {
                manualStop();
            }
            mStyle = style;
            mCamera->setAutoTracking(false);
        }
        Ogre::SceneNode* getTarget()
        {
            return mTarget;
        }

        /**
        Sets the spatial offset from the target. Only applies for orbit style.
        */
        void setYawPitchDist(const Ogre::Radian& yaw, const Ogre::Radian& pitch, Ogre::Real dist)
        {
           //OgreAssert(mTarget, "no target set");
           if (!mTarget) return ;

            mOffset = Ogre::Vector3::ZERO;
            mCamera->setPosition(mTarget->_getDerivedPosition());
            mCamera->setOrientation(mTarget->_getDerivedOrientation());
            mCamera->yaw(yaw);
            mCamera->pitch(-pitch);
            mCamera->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
        }

        /**
        Sets the camera's top speed. Only applies for free-look style.
        */
        void setTopSpeed(Ogre::Real topSpeed)
        {
            mTopSpeed = topSpeed;
        }

        Ogre::Real getTopSpeed()
        {
            return mTopSpeed;
        }



        CameraStyle getStyle()
        {
            return mStyle;
        }

        Ogre::Real getDistToTarget()
        {
            if (!mTarget) return 0;
            Ogre::Vector3 offset = mCamera->getPosition() - mTarget->_getDerivedPosition() - mOffset;
            return offset.length();
        }

        void setPivotOffset(const Ogre::Vector3& pivot)
        {
            if (!mTarget) return ;
            Ogre::Real dist = getDistToTarget();
            mOffset = pivot;
            mCamera->setPosition(mTarget->_getDerivedPosition() + mOffset);
            mCamera->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
        }
            void MouseLook(float xrel, float yrel)
            {
                if (mStyle == CS_ORBIT)
                {
                    mOrbiting  = IsMouseButtonDown(0);
                    mMoving    = IsMouseButtonDown(2);
//                    mZooming   = IsMouseButtonDown(1);


                    Ogre::Real dist = getDistToTarget();

                    if (mOrbiting)   // yaw around the target, and pitch locally
                    {
                        mCamera->setPosition(mTarget->_getDerivedPosition() + mOffset);

                        mCamera->yaw(Ogre::Degree(-xrel * 0.25f), mYawSpace);
                        mCamera->pitch(Ogre::Degree(-yrel * 0.25f));

                        mCamera->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
                        // don't let the camera go over the top or around the bottom of the target
                    }
                    else if (mMoving)  // move the camera along the image plane
                    {
                        Ogre::Vector3 delta = mCamera->getOrientation() * Ogre::Vector3(-xrel, yrel, 0);
                        // the further the camera is, the faster it moves
                        delta *= dist / 1000.0f;
                        mOffset += delta;
                        mCamera->translate(delta);
                    }

                }
                else if (mStyle == CS_FREELOOK)
                {
                    mCamera->yaw(Ogre::Degree(-xrel * 0.15f), Ogre::Node::TS_PARENT);
                    mCamera->pitch(Ogre::Degree(-yrel * 0.15f));
                }


            }

            bool mouseWheelRolled(float value)
            {
                if (mStyle == CS_ORBIT && value != 0)
                {
                    Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
                    mCamera->translate(Ogre::Vector3(0, 0, -value * 0.08f * dist), Ogre::Node::TS_LOCAL);
                }


            }


       void Update()
       {
                 axes = mCamera->getLocalAxes();

       }

        void Advance(Ogre::Vector3 accel , float timeSinceLastFrame)
        {
            if (mStyle == CS_FREELOOK)
            {

                // if accelerating, try to reach top speed in a certain time
                Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
                if (accel.squaredLength() != 0)
                {
                    accel.normalise();
                    mVelocity += accel * topSpeed * timeSinceLastFrame * 10;
                }
                // if not accelerating, try to stop in a certain time
                else mVelocity -= mVelocity * timeSinceLastFrame * 10;

                Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

                // keep camera velocity below top speed and above epsilon
                if (mVelocity.squaredLength() > topSpeed * topSpeed)
                {
                    mVelocity.normalise();
                    mVelocity *= topSpeed;
                }
                else if (mVelocity.squaredLength() < tooSmall * tooSmall)
                    mVelocity = Ogre::Vector3::ZERO;

                if (mVelocity != Ogre::Vector3::ZERO) mCamera->translate(mVelocity * timeSinceLastFrame);
            }
        }
        void MoveForward(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel -= axes.GetColumn(2);
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveBack(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel += axes.GetColumn(2);
                Advance(accel,timeSinceLastFrame);
            }
        }

        void MoveUp(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel += axes.GetColumn(1);
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveDown(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel -= axes.GetColumn(1);
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveLeft(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
               accel -= axes.GetColumn(0);
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveRight(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel += axes.GetColumn(0);
                Advance(accel,timeSinceLastFrame);
            }
        }

        void manualStop()
        {
            if (mStyle == CS_FREELOOK)
            {

                mVelocity = Ogre::Vector3::ZERO;
            }
        }

        void setFixedYaw(bool fixed)
        {
            mYawSpace = fixed ? Ogre::Node::TS_PARENT : Ogre::Node::TS_LOCAL;
        }


    protected:

        Ogre::Node::TransformSpace mYawSpace;
        Ogre::SceneNode* mCamera;
        CameraStyle mStyle;
        Ogre::SceneNode* mTarget;
        bool mOrbiting;
        bool mMoving;
        Ogre::Real mTopSpeed;
        Ogre::Vector3 mVelocity;
        bool mFastMove;
        Ogre::Vector3 mOffset;
        Ogre::Matrix3 axes;
    };


namespace OgreBites {

    /**
        OGRE comes with it's own renderer configuration dialog, which
        applications can use to easily allow the user to configure the
        settings appropriate to their machine. This defines the
        interface to this standard dialog. Because dialogs are inherently
        tied to a particular platform's windowing system, there will be a
        different subclass for each platform.

     * @return pointer to Ogre::ConfigDialog or NULL if not available on the current platform.
     */
    Ogre::ConfigDialog* getNativeConfigDialog();

} /* namespace OgreBites */


class MyFrameListener : public FrameListener {

public:

	// ctor/dtor
	MyFrameListener() { m_timeElapsed = 0.0f; }
	virtual ~MyFrameListener() {}

	// We will provide some meat to this method override
	virtual bool frameStarted(const FrameEvent &evt);

	// We do not need to provide a body for either of these methods, since
	// Ogre provides a default implementation that does just this. However, for
	// the sake of illustration, we'll provide one here.
	virtual bool frameRenderingQueued(const FrameEvent &evt) ;

private:
	float m_timeElapsed;
};


Ogre::Root *mRoot;
Ogre::Camera* mainCamera;
CameraMan*    camera;
Ogre::SceneManager* mSceneMgr;
Ogre::RenderWindow* mWindow;
SDL_Window* window;
bool mShutDown=false;
int exitKey;                    // Default exit key
char currentKeyState[512];      // Registers current frame key state
char previousKeyState[512];     // Registers previous frame key state
int keyPressedQueue[MAX_KEY_PRESSED_QUEUE];     // Input keys queue
int keyPressedQueueCount;       // Input keys queue count
int charPressedQueue[MAX_CHAR_PRESSED_QUEUE];   // Input characters queue
int charPressedQueueCount=0;      // Input characters queue count
char currentButtonState[3];     // Registers current mouse button state
char previousButtonState[3];    // Registers previous mouse button state
float currentWheelMove=0;         // Registers current mouse wheel variation
float previousWheelMove=00;        // Registers previous mouse wheel variation
int mMouseX=0;
int mMouseY=0;



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
void KeyCallback( int key,  int action)
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
  void CharCallback( unsigned int key)
{
    if (charPressedQueueCount < MAX_KEY_PRESSED_QUEUE)
    {
        charPressedQueue[charPressedQueueCount] = key;
        charPressedQueueCount++;
    }
}

  void InputEvent(const SDL_Event& in)
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
              KeyCallback(in.key.keysym.scancode,0);
            }

                break;
           case SDL_TEXTINPUT:
              //  textInput(in.text.text);
            break;

            case SDL_MOUSEMOTION:
            {
            MouseMotionEvent motion;
            motion.x = in.motion.x;
            motion.y = in.motion.y;
            motion.xrel = in.motion.xrel;
            motion.yrel = in.motion.yrel;
            }
             break;

            case SDL_MOUSEBUTTONDOWN:
            {
            MouseButtonEvent button;
            button.x = in.button.x;
            button.y = in.button.y;
            button.button = in.button.button;
            button.clicks = in.button.clicks;
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
            int MouseButton = in.button.button-1;
            currentButtonState[MouseButton] = 0;

            }

                break;
            case SDL_MOUSEWHEEL:
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

void pollEvents()
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

Vector2 previousMousePosition;
bool MyFrameListener::frameStarted(const FrameEvent &evt)
{

    keyPressedQueueCount = 0;
    charPressedQueueCount = 0;
    for (int i = 0; i < 512; i++) previousKeyState[i] = currentKeyState[i];
    for (int i = 0; i < 3; i++) previousButtonState[i] =currentButtonState[i];
    previousWheelMove = currentWheelMove;
    currentWheelMove = 0.0f;

    pollEvents();


    if (mWindow->isClosed() || mShutDown)
    {
     return false;
    }


	m_timeElapsed += evt.timeSinceLastFrame;




Vector2 mousePositionDelta =Vector2::ZERO;
Vector2 mousePosition = GetMousePosition();
mousePositionDelta.x = mousePosition.x - previousMousePosition.x;
mousePositionDelta.y = mousePosition.y - previousMousePosition.y;
previousMousePosition = mousePosition;

     float dt =evt.timeSinceLastFrame;

     camera->Update();

     if (IsMouseButtonDown(0))
      {
        camera->MouseLook(mousePositionDelta.x,mousePositionDelta.y);
        //printf(" %f %f \n",mousePositionDelta.x,mousePositionDelta.y);
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


    mWindow->update();


	return true;
}

bool MyFrameListener::frameRenderingQueued(const FrameEvent &evt)
{


return true;
}


Ogre::GLPlugin* mGLPlugin;
Ogre::ParticleFXPlugin* mParticleFXPlugin;
Ogre::STBIPlugin* mSTBImagePlugin;
Ogre::OctreePlugin* mOctreePlugin;
Ogre::BspSceneManagerPlugin* mBSPPlugin;

void setupResources(void)
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


            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}



Ogre::RenderWindow* createWindow(const Ogre::String& name, Ogre::uint32 w, Ogre::uint32 h, Ogre::NameValuePairList miscParams)
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
   SDL_GL_SetSwapInterval(1); // Enable vsync


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

bool startOgre(void)
{
    charPressedQueueCount=0;
    keyPressedQueueCount=0;
    previousWheelMove = 0.0f;
    currentWheelMove = 0.0f;


    // construct Ogre::Root : no plugins filename, no config filename, using a custom log filename
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


//-------------------------------------------------------------------------------------
    // configure
    // Grab the OpenGL RenderSystem, or exit

    Ogre::RenderSystem* rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    if(!(rs->getName() == "OpenGL Rendering Subsystem"))
    {
        return false; //No RenderSystem found
    }
    // configure our RenderSystem
    rs->setConfigOption("Full Screen", "No");
    rs->setConfigOption("VSync", "No");
    rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit");

    mRoot->setRenderSystem(rs);



        mWindow=createWindow("Ogre3D",1020,780,Ogre::NameValuePairList());
        Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
        Ogre::Root::getSingleton().clearEventTimes();
        //Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);
            //Ogre::LogManager::getSingleton().setMinLogLevel(Ogre::LML_TRIVIAL);
        mRoot->saveConfig();

  //  mWindow = mRoot->initialise(true, "LowLevelOgre Render Window");

/*
     if(mRoot->showConfigDialog(OgreBites::getNativeConfigDialog()))
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

        mRoot->saveConfig();

    } else
    {

    return 0;
    }
*/
    //mSceneMgr = mRoot->createSceneManager("BspSceneManager");
    //mSceneMgr = mRoot->createSceneManager("PCZSceneManager");

    mSceneMgr = mRoot->createSceneManager("OctreeSceneManager");
    //mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    mainCamera = mSceneMgr->createCamera("PlayerCam");
    mainCamera->setPosition(Ogre::Vector3(0,0,80));
    mainCamera->lookAt(Ogre::Vector3(0,0,-300));
    mainCamera->setNearClipDistance(5);

    SceneNode* camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    camNode->attachObject(mainCamera);
    camera = new CameraMan(camNode);
    camera->setStyle(CS_FREELOOK);


    Ogre::Viewport* vp = mWindow->addViewport(mainCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0.4f));
    mainCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));


    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

//-------------------------------------------------------------------------------------
    // Create any resource listeners (for loading screens)
    //createResourceListener();
//-------------------------------------------------------------------------------------
    // load resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
//-------------------------------------------------------------------------------------
    // Create the scene


    MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Plane(Vector3::UNIT_Y, 0), 1000, 1000, 20, 20, true, 1, 6, 6, Vector3::UNIT_Z);

		// create a ground entity from our mesh and attach it to the origin
		Entity* ground = mSceneMgr->createEntity("Ground", "ground");
		ground->setMaterialName("Examples/GrassFloor");
		ground->setCastShadows(false);
		mSceneMgr->getRootSceneNode()->attachObject(ground);


    Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");

    Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    headNode->attachObject(ogreHead);

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);
//-------------------------------------------------------------------------------------
	MyFrameListener listener;
    mRoot->addFrameListener(&listener);

    mRoot->startRendering();
    /*
    while(true)
    {
        // Pump window messages for nice behaviour
//        Ogre::WindowEventUtilities::messagePump();

        // Render a frame
        mRoot->renderOneFrame();



        if(mWindow->isClosed())
        {
            return false;
        }
    }*/

    if (camera) delete camera;
    mRoot->destroySceneManager(mSceneMgr);
    mRoot->queueEndRendering();

    OGRE_DELETE mRoot;
    if (mGLPlugin) OGRE_DELETE mGLPlugin;
    if(mParticleFXPlugin) OGRE_DELETE mParticleFXPlugin;
    if(mSTBImagePlugin) OGRE_DELETE mSTBImagePlugin;
    if(mOctreePlugin) OGRE_DELETE mOctreePlugin;
    if(mBSPPlugin) OGRE_DELETE mBSPPlugin;



    SDL_DestroyWindow(window);



    return true;
}
int main()
{
    SDL_Init(SDL_INIT_EVERYTHING ) ;

  MyFrameListener listener;

	//	root->addFrameListener(&listener);




        try
        {
          startOgre();
        } catch( Ogre::Exception& e ) {
            std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;

        }


    SDL_LogInfo(0,"SDL Quit");
    SDL_Quit();
    return 0;
}
