#pragma once

#include "GLView.h"
#include "irrKlang.h"
namespace Aftr
{
   class Camera;

/**
   \class GLViewNewModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewNewModule : public GLView
{
public:
    irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
    irrklang::ISoundEngine* steroSound = irrklang::createIrrKlangDevice();
    irrklang::ISound* music = 0;
    irrklang::ISoundEffectControl* musicFX;
    std::vector<const char*>soundList = {};
    irrklang::vec3df irrkVec3(Vector vec) {
        return irrklang::vec3df(vec.x, vec.y, vec.z);
    }
    irrklang::vec3df convertVectorToVec3df(const Vector& v) {
        return irrklang::vec3df(v.x, v.y, v.z);
    }
   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNewModuleWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   WO* shinyredcube = nullptr;
   Camera* x;
   WO* cube;
   WO* mainCameraFrustum;

protected:
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate();   
};

/** \} */

} //namespace Aftr
