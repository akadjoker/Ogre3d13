/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
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

#ifndef __BaseApplication_h_
#define __BaseApplication_h_


#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreLogManager.h>


#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>



//#define OGRE_STATIC_LIB;
//#define OGRE_STATIC_GL
#define OGRE_STATIC_BSPSceneManager
#define OGRE_STATIC_ParticleFX

//#ifdef OGRE_USE_PCZ
//#define OGRE_STATIC_PCZSceneManager
//#define OGRE_STATIC_OctreeZone

#define OGRE_STATIC_OctreeSceneManager

#include "OgreStaticPluginLoader.h"

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
        CameraMan(Ogre::Camera* cam);

       	virtual void setCamera(Ogre::Camera* cam)
		{
			mCamera = cam;
		}

		virtual Ogre::Camera* getCamera()
		{
			return mCamera;
		}
        /**
        Sets the target we will revolve around. Only applies for orbit style.
        */
        virtual void setTarget(Ogre::SceneNode* target);

        Ogre::SceneNode* getTarget()
        {
            return mTarget;
        }

        /**
        Sets the spatial offset from the target. Only applies for orbit style.
        */
        void setYawPitchDist(const Ogre::Radian& yaw, const Ogre::Radian& pitch, Ogre::Real dist);

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

        /**
        Sets the movement style of our camera man.
        */
        virtual void setStyle(CameraStyle style);

        CameraStyle getStyle()
        {
            return mStyle;
        }


        void manualStop();

        void frameRendered(const Ogre::FrameEvent& evt);


        void setFixedYaw(bool fixed)
        {
            mYawSpace = fixed ? Ogre::Node::TS_PARENT : Ogre::Node::TS_LOCAL;
        }
        void mouseWheelRolled(int value);

        void MouseLook(float xrel, float yrel);


        void Advance(Ogre::Vector3 accel,float timeSinceLastFrame)
        {


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

                //if (mVelocity != Ogre::Vector3::ZERO) mCamera->translate(mVelocity * timeSinceLastFrame);
                if (mVelocity != Ogre::Vector3::ZERO) mCamera->move(mVelocity * timeSinceLastFrame);
        }
        void MoveForward(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel += mCamera->getDirection();
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveBack(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel -= mCamera->getDirection();
                Advance(accel,timeSinceLastFrame);
            }
        }

        void MoveUp(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel += mCamera->getUp();
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveDown(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel -= mCamera->getUp();
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveLeft(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
               accel -= mCamera->getRight();
                Advance(accel,timeSinceLastFrame);
            }
        }
        void MoveRight(float timeSinceLastFrame)
        {

           if (mStyle == CS_FREELOOK)
            {
                Ogre::Vector3 accel = Ogre::Vector3::ZERO;
                accel += mCamera->getRight();
                Advance(accel,timeSinceLastFrame);
            }
        }


        void setPivotOffset(const Ogre::Vector3& offset);
    protected:
        Ogre::Real getDistToTarget();
        Ogre::Node::TransformSpace mYawSpace;
        Ogre::Camera* mCamera;
        CameraStyle mStyle;
        Ogre::SceneNode* mTarget;
        bool mOrbiting;
        bool mMoving;
        bool mZooming;
        Ogre::Real mTopSpeed;
        Ogre::Vector3 mVelocity;
        bool mFastMove;
        Ogre::Vector3 mOffset;
    };

class BaseApplication : public Ogre::FrameListener
{
public:
    BaseApplication(void);
    virtual ~BaseApplication(void);

    virtual void go(void);

public:
void writeRenderToFile(const Ogre::String& filename);
void TakeScreenshot(const char *fileName);


protected:
    virtual bool setup();
    virtual bool configure(void);
    virtual void chooseSceneManager(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);



    virtual void createViewports(void);
    virtual void setupResources(void);
    virtual void createResourceListener(void);
    virtual void loadResources(void);

    virtual void createScene(void) = 0; // Override me!
    virtual void destroyScene(void) ;

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);// Override me!
    virtual bool frameStarted(const Ogre::FrameEvent& evt);
    virtual bool frameEnded(const Ogre::FrameEvent& evt);

    virtual bool OnRender();
    virtual bool OnUpdate(float dt);

    virtual bool keyPressed(const KeyboardEvent& evt) { return false;}
    virtual bool keyReleased(const KeyboardEvent& evt) { return false; }
    virtual bool touchMoved(const TouchFingerEvent& evt) { return false; }
    virtual bool touchPressed(const TouchFingerEvent& evt) { return false; }
    virtual bool touchReleased(const TouchFingerEvent& evt) { return false; }
    virtual bool mouseMoved(const MouseMotionEvent& evt) { return false; }
    virtual bool mouseWheel(float value) { return false; }
    virtual bool mousePressed(const MouseButtonEvent& evt) { return false; }
    virtual bool mouseReleased(const MouseButtonEvent& evt) { return false; }
    virtual bool textInput(const char*) { return false; }
    virtual bool axisMoved(const AxisEvent& evt) { return false; }


    virtual void windowMoved(Ogre::RenderWindow* rw) {}
    virtual void windowResized(Ogre::RenderWindow* rw) ;
    virtual bool windowClosing(Ogre::RenderWindow* rw) { return true; }
    virtual void windowClosed(Ogre::RenderWindow* rw) ;
    virtual void windowFocusChange(Ogre::RenderWindow* rw) {}



    Ogre::Root*                 mRoot;
    Ogre::Camera*               mCamera;
    Ogre::SceneManager*         mSceneMgr;
    Ogre::RenderWindow*         mWindow;
    CameraMan*    camera;
    bool mShutDown;

    Ogre::StaticPluginLoader plugins;

    //SDL_GLContext gl_context;
    SDL_Window* window;



private:



void pollEvents();
void InputEvent(const SDL_Event& event);
Ogre::RenderWindow* createWindow(const Ogre::String& name, Ogre::uint32 w, Ogre::uint32 h, Ogre::NameValuePairList miscParams);




};

//---------------------------------------------------------------------------

#endif // #ifndef __BaseApplication_h_

//---------------------------------------------------------------------------

