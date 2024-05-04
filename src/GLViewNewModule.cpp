#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "aftrUtilities.h"
#include <irrKlang.h>
#include <NetMsgCreateWO.h>
#include <NetMsg.h>
#include <new.h>
#include "NetMessengerStreamBuffer.h"
#include <NetMessengerClient.h>
using namespace Aftr;
using namespace irrklang;

GLViewNewModule* GLViewNewModule::New(const std::vector< std::string >& args)
{
    GLViewNewModule* glv = new GLViewNewModule(args);
    glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
    glv->onCreate();
    return glv;
}


GLViewNewModule::GLViewNewModule(const std::vector< std::string >& args) : GLView(args)
{
    //Initialize any member variables that need to be used inside of LoadMap() here.
    //Note: At this point, the Managers are not yet initialized. The Engine initialization
    //occurs immediately after this method returns (see GLViewNewModule::New() for
    //reference). Then the engine invoke's GLView::loadMap() for this module.
    //After loadMap() returns, GLView::onCreate is finally invoked.

    //The order of execution of a module startup:
    //GLView::New() is invoked:
    //    calls GLView::init()
    //       calls GLView::loadMap() (as well as initializing the engine's Managers)
    //    calls GLView::onCreate()

    //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
    //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
    //At this point, all the managers are initialized. That is, the engine is fully initialized.

    if (this->pe != NULL)
    {
        //optionally, change gravity direction and magnitude here
        //The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
    //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewNewModule::~GLViewNewModule()
{
    //Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
    GLView::updateWorld(); //Just call the parent's update world first.
    //If you want to add additional functionality, do it after
    //this call.
}


void GLViewNewModule::onResizeWindow(GLsizei width, GLsizei height)
{
    GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewNewModule::onMouseDown(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseDown(e);
}


void GLViewNewModule::onMouseUp(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseUp(e);
}


void GLViewNewModule::onMouseMove(const SDL_MouseMotionEvent& e)
{
    GLView::onMouseMove(e);
}

NetMessengerClient* client;
void GLViewNewModule::onKeyDown(const SDL_KeyboardEvent& key)
{

    GLView::onKeyDown(key);
    if (key.keysym.sym == SDLK_0)
        this->setNumPhysicsStepsPerRender(1);

    /*if (key.keysym.sym == SDLK_1)
    {

    }
    if (key.keysym.sym == SDLK_2)
    {

    }*/
}


void GLViewNewModule::onKeyUp(const SDL_KeyboardEvent& key)
{
    GLView::onKeyUp(key);
}


void Aftr::GLViewNewModule::loadMap()
{
    this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
    this->actorLst = new WorldList();
    this->netLst = new WorldList();
    std::string listenPortStr = ManagerEnvironmentConfiguration::getVariableValue("NetServerTransmissionPort");

    client = NetMessengerClient::New("127.0.0.1", listenPortStr);
    ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
    ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
    ManagerOpenGLState::enableFrustumCulling = false;
    Axes::isVisible = true;
    this->glRenderer->isUsingShadowMapping(true); //set to TRUE to enable shadow mapping, must be using GL 3.2+

    this->cam->setPosition(15, 40, 10);

    std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
    std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
    std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
    std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");


    std::string blimp(ManagerEnvironmentConfiguration::getSMM() + "/models/Aircraft/A10/A10_ScaledActualSize/a10.obj");
    WO* wo1 = WO::New(blimp, Vector(1, 1, 1), MESH_SHADING_TYPE::mstSMOOTH);
    wo1->setPosition(Vector(10, 15, 25));
    wo1->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    wo1->setLabel("Blimp");
    wo1->upon_async_model_loaded([wo1]() {
        ModelMeshSkin& spiderSkin = wo1->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
        spiderSkin.setAmbient(aftrColor4f(0.2f, 0.5f, 0.8f, 1.0f));
        spiderSkin.setDiffuse(aftrColor4f(0.2f, 0.5f, 0.8f, 1.0f));
        spiderSkin.setSpecular(aftrColor4f(0.2f, 0.5f, 0.8f, 1.0f));
        spiderSkin.setSpecularCoefficient(10);
        });
    worldLst->push_back(wo1);



    std::string DME(ManagerEnvironmentConfiguration::getSMM() + "/models/Buildings/building31.3DS");
    WO* wo2 = WO::New(DME, Vector(0.05, 0.05, 0.05), MESH_SHADING_TYPE::mstFLAT);
    wo2->setPosition(Vector(5, 80, 5));
    wo2->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    wo2->setLabel("DME");
    worldLst->push_back(wo2);

    std::string Runway(ManagerEnvironmentConfiguration::getSMM() + "/models/road26x10.wrl");
    WO* wo3 = WO::New(Runway, Vector(10, 5, 1), MESH_SHADING_TYPE::mstFLAT);
    wo3->setPosition(Vector(10, 5, 0));
    wo3->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    wo3->setLabel("Runway");
    worldLst->push_back(wo3);


    //SkyBox Textures readily available
    std::vector< std::string > skyBoxImageNames; //vector to store texture paths
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
    skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg");
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
    //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

    {
        //Create a light
        float ga = 0.1f; //Global Ambient Light level for this module
        ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
        WOLight* light = WOLight::New();
        light->isDirectionalLight(true);
        light->setPosition(Vector(0, 0, 100));
        //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
        //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
        light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
        light->setLabel("Light");
        worldLst->push_back(light);
    }

    {
        //Create the SkyBox
        WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
        wo->setPosition(Vector(0, 0, 0));
        wo->setLabel("Sky Box");
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        worldLst->push_back(wo);
    }

    {
        ////Create the infinite grass plane (the floor)
        WO* wo = WO::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
        wo->setPosition(Vector(0, 0, 0));
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        wo->upon_async_model_loaded([wo]()
            {
                ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
                grassSkin.getMultiTextureSet().at(0).setTexRepeats(5.0f);
                grassSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
                grassSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
                grassSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
                grassSkin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
            });
        wo->setLabel("Grass");
        worldLst->push_back(wo);
    }

    //{
    //   //Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
    //   WO* wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
    //   ((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
    //   wo->setPosition( Vector(0,0,0) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   //Create the infinite grass plane that uses NVIDIAPhysX(the floor)
    //   WO* wo = WONVStaticPlane::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
    //   wo->setPosition( Vector( 0, 0, 0 ) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 ).getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   //Create the infinite grass plane (the floor)
    //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
    //   wo->setPosition( Vector( 0, 0, 50.0f ) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
    //   wo->setPosition( Vector( 0, 0.5f, 75.0f ) );
    //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    //   wo->setLabel( "Grass" );
    //   worldLst->push_back( wo );
    //}

    //{
    //   WO* wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue( "sharedmultimediapath" ) + "/models/sphereRp5.wrl", Vector( 1.0f, 1.0f, 1.0f ), mstSMOOTH );
    //   wo->setPosition( 0, 0, 100.0f );
    //   wo->setLabel( "Sphere" );
    //   this->worldLst->push_back( wo );
    //}

    //{
    //   WO* wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
    //   ((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
    //   ((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
    //   wo->setPosition( Vector( 20, 20, 20 ) );
    //   wo->setLabel( "Paladin" );
    //   worldLst->push_back( wo );
    //   actorLst->push_back( wo );
    //   netLst->push_back( wo );
    //   this->setActor( wo );
    //}
    //
    //{
    //   WO* wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
    //   wo->setPosition( Vector( 20, 10, 20 ) );
    //   wo->isVisible = false; //hide the WOHuman's bounding box
    //   ((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
    //   wo->setLabel( "Human Cyborg" );
    //   worldLst->push_back( wo );
    //   actorLst->push_back( wo ); //Push the WOHuman as an actor
    //   netLst->push_back( wo );
    //   this->setActor( wo ); //Start module where human is the actor
    //}

    //{
    //   //Create and insert the WOWheeledVehicle
    //   std::vector< std::string > wheels;
    //   std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
    //   wheels.push_back( wheelStr );
    //   wheels.push_back( wheelStr );
    //   wheels.push_back( wheelStr );
    //   wheels.push_back( wheelStr );
    //   WO* wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
    //   wo->setPosition( Vector( 5, -15, 20 ) );
    //   wo->setLabel( "Car 1970s Beater" );
    //   ((WOODE*)wo)->mass = 200;
    //   worldLst->push_back( wo );
    //   actorLst->push_back( wo );
    //   this->setActor( wo );
    //   netLst->push_back( wo );
    //}

    //Make a Dear Im Gui instance via the WOImGui in the engine... This calls
    //the default Dear ImGui demo that shows all the features... To create your own,
    //inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).
    //std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");



   /* WO* wo2 = WO::New(wheeledCar, Vector(3, 3, 3), MESH_SHADING_TYPE::mstSMOOTH);
    wo2->setPosition(Vector(0, 10, 2));
    wo2->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    wo2->setLabel("Car");
    worldLst->push_back(wo2);*/

    //// Variables for translation along x, y, z axes
    //float translationX = 0.0f;
    //float translationY = 0.0f;
    //float translationZ = 0.0f;

    //// Initial positions of the objects
    //Vector initialPos1 = wo1->getPosition();
    //Vector initialPos2 = wo2->getPosition();

    // Flag to toggle between global and relative coordinate systems
    WOImGui* gui = WOImGui::New(nullptr);
    gui->setLabel("My Gui");

    soundEngine->setRolloffFactor(2.0f);

    irrklang::ISoundSource* sound2 = soundEngine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getSMM() + "/sounds/laser.wav").c_str());
    soundEngine->addSoundSourceAlias(sound2, "sound2");
    soundList.push_back("sound2");

    float prevTranslationX = 0.0f;
    float prevTranslationY = 0.0f;
    float prevTranslationZ = 0.0f;

    // Variables for translation along x, y, z axes
    float translationX = 0.0f;
    float translationY = 0.0f;
    float translationZ = 0.0f;

    // Initial positions of the objects
    Vector initialPos1 = wo1->getPosition();
    Vector initialPos2 = wo2->getPosition();

    float azimuth = 0.0f;
    float elevation = 0.0f;
    float errorTolerance = 0.0f;

    // Flag to toggle between global and relative coordinate systems
    bool useGlobalCoords = true;

    gui->subscribe_drawImGuiWidget(
        [this, gui, wo1, wo3, &translationX, &translationY, &translationZ, &useGlobalCoords, &initialPos1, &initialPos2, &prevTranslationX, &prevTranslationY, &prevTranslationZ, sound2]() // Lambda function capturing 'this', 'gui', 'wo', and previous rotation angles by reference
        {
            ImGui::ShowDemoWindow(); // Displays the default ImGui demo
            WOImGui::draw_AftrImGui_Demo(gui); // Displays a small Aftr Demo
            ImPlot::ShowDemoWindow(); // Displays the ImPlot demo

            // Dropdown for selecting the object


            WO* selectedWo = wo1;
            WO* runway = wo3;

            static bool useGlobalCoords;

            // Sliders for x, y, and z axes translation

            float azimuth = 0.0f;
            float elevation = 0.0f;
            float errorTolerance = 0.0f;





            // Rotation controls for x, y, and z axes
            static float xRotation = 0.0f; // Initial x-axis rotation
            static float yRotation = 0.0f; // Initial y-axis rotation
            static float zRotation = 0.0f; // Initial z-axis rotation

            static float prevXRotation = 0.0f;
            static float prevYRotation = 0.0f;
            static float prevZRotation = 0.0f;

            std::string buttonText = static_cast<std::string> ("Current Coordinate: ");
            std::string systemLabel;
            if (useGlobalCoords) {
                systemLabel = "Global Coordinates";
            }
            else {
                systemLabel = "Relative Coordinates";
            }
            buttonText = buttonText + systemLabel;
            if (ImGui::Button(buttonText.c_str())) {
                useGlobalCoords = !useGlobalCoords;
            }

            // Slider for x-axis rotation
            if (ImGui::SliderAngle("X Rotation", &xRotation, -360.0f, 360.0f)) {
                if (xRotation != prevXRotation) {
                    selectedWo->rotateAboutGlobalX(xRotation - prevXRotation);
                    prevXRotation = xRotation;
                }
            }

            // Slider for y-axis rotation
            if (ImGui::SliderAngle("Y Rotation", &yRotation, -360.0f, 360.0f)) {
                if (yRotation != prevYRotation) {
                    selectedWo->rotateAboutGlobalY(yRotation - prevYRotation);
                    prevYRotation = yRotation;
                }
            }

            // Slider for z-axis rotation
            if (ImGui::SliderAngle("Z Rotation", &zRotation, -360.0f, 360.0f)) {
                if (zRotation != prevZRotation) {
                    selectedWo->rotateAboutGlobalZ(zRotation - prevZRotation);
                    prevZRotation = zRotation;
                }
            }
            static float translationSpeed = 1.0f;
            static float translation[3] = { 0.0f, 0.0f, 0.0f };
            static float lastTranslation[3] = { 0.0f, 0.0f, 0.0f };

            if (ImGui::SliderFloat("Move in X Direction", &translation[0], -100.0f, 100.0f)) {
                if (selectedWo) {
                    float deltaTranslationX = translation[0] - lastTranslation[0];
                    Vector transVector(deltaTranslationX * translationSpeed, 0.0f, 0.0f);
                    if (!useGlobalCoords) {
                        Mat4 Matrixrotate = selectedWo->getDisplayMatrix();
                        transVector = Matrixrotate * transVector;
                    }
                    selectedWo->moveRelative(transVector);
                    lastTranslation[0] = translation[0];
                }
            }
            if (ImGui::SliderFloat("Move in Y Direction", &translation[1], -100.0f, 100.0f)) {
                if (selectedWo) {
                    float deltaTranslationY = translation[1] - lastTranslation[1];
                    Vector transVector(0.0f, deltaTranslationY * translationSpeed, 0.0f);
                    if (!useGlobalCoords) {
                        Mat4 Matrixrotate = selectedWo->getDisplayMatrix();
                        transVector = Matrixrotate * transVector;
                    }
                    selectedWo->moveRelative(transVector);
                    lastTranslation[1] = translation[1];
                }
            }

            if (ImGui::SliderFloat("Move in Z Direction", &translation[2], -100.0f, 100.0f)) {
                if (selectedWo) {
                    float deltaTranslationZ = translation[2] - lastTranslation[2];
                    Vector transVector(0.0f, 0.0f, deltaTranslationZ * translationSpeed);
                    if (!useGlobalCoords) {
                        Mat4 Matrixrotate = selectedWo->getDisplayMatrix();
                        transVector = Matrixrotate * transVector;
                    }
                    selectedWo->moveRelative(transVector);
                    lastTranslation[2] = translation[2];
                }
            }
            bool buttonhit = false;
            bool manuallySendPulse = false; // Initially, the checkbox is unchecked

            if(ImGui::Button("Send Pulses")){
                // If the checkbox is checked, perform the action
                // Increment the current song index, wrapping around if necessary
                // Stop any previously playing sounds and play the selected background track
                soundEngine->stopAllSounds();
                soundEngine->play2D(sound2, true);
        }
                    Vector objectPosition = selectedWo->getPosition();

                    // Get the position of the camera
                    Vector cameraPosition = runway->getPosition();

                    // Calculate the direction vector from the camera to the object
                    VectorT<float> direction = (objectPosition - cameraPosition);
                    // Calculate the azimuth angle (angle in the x-y plane)
                    azimuth = atan2(direction.y, direction.x) * 180.0 / M_PI;

                    // Calculate the elevation angle (angle in the x-z plane)
                    elevation = atan2(direction.z, sqrt(direction.x * direction.x + direction.y)) * 10;

                    float desiredAzimuth = 45.0f; // Example desired azimuth angle in degrees
                    float desiredElevation = 1.0f; // Example desired elevation angle in degrees

                    // Calculate the actual azimuth and elevation angles
                    float azimuthDifference = fabs(azimuth - desiredAzimuth);
                    float elevationDifference = fabs(elevation - desiredElevation);

                    // Combine the absolute differences (you can use a sum or maximum)
                    float combinedDifference = azimuthDifference + elevationDifference;

                    // Set the combined difference as the error tolerance
                    errorTolerance = combinedDifference;

                    ImGui::Text("Azimuth: %.2f degrees", azimuth);
                    ImGui::Text("Elevation: %.2f degrees", elevation);
                    ImGui::Text("Error Tolerance: %.2f", errorTolerance);

                    if (ImGui::Button("Stop Pulses")) {
                        // If the checkbox is checked, perform the action
                        // Increment the current song index, wrapping around if necessary
                        // Stop any previously playing sounds and play the selected background track
                        soundEngine->stopAllSounds();
                    }
            // Reset button
            if (ImGui::Button("Reset Position")) {
                wo1->setPosition(Vector(10, 15, 25));
                
            }






        }
    );

    this->worldLst->push_back(gui);

    createNewModuleWayPoints();
}

void GLViewNewModule::createNewModuleWayPoints()
{
    // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
    WayPointParametersBase params(this);
    params.frequency = 5000;
    params.useCamera = true;
    params.visible = true;
    WOWayPointSpherical* wayPt = WOWayPointSpherical::New(params, 3);
    wayPt->setPosition(Vector(50, 0, 3));
    worldLst->push_back(wayPt);
}










