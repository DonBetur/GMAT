//$Header$
//------------------------------------------------------------------------------
//                              TrajPlotCanvas
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares TrajPlotCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef TrajPlotCanvas_hpp
#define TrajPlotCanvas_hpp

#include "gmatwxdefs.hpp"
#include "MdiGlPlotData.hpp"
#include "TextTrajectoryFile.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

class TrajPlotCanvas: public wxGLCanvas
{
public:
   TrajPlotCanvas(wxWindow *parent, const wxWindowID id = -1,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, SolarSystem *ss = NULL,
                  long style = 0, const wxString& name = wxT("TrajPlotCanvas"));
   ~TrajPlotCanvas();

   // initialization
   bool InitGL();

   // getters
   float GetDistance() {return mAxisLength;}
   bool  GetDrawWireFrame() {return mDrawWireFrame;}
   bool  GetDrawEqPlane() {return mDrawEqPlane;}
   bool  GetDrawEcPlane() {return mDrawEcPlane;}
   bool  GetDrawEcLine() {return mDrawEcLine;}
   unsigned int GetEqPlaneColor() {return mEqPlaneColor;}
   unsigned int GetEcPlaneColor() {return mEcPlaneColor;}
   unsigned int GetEcLineColor() {return mEcLineColor;}
   int GetGotoBodyId() {return mCenterViewBody;}
   
   // setters
   void SetDistance(float dist) {mAxisLength = dist;}
   void SetDrawWireFrame(bool flag) {mDrawWireFrame = flag;}
   void SetDrawEqPlane(bool flag) {mDrawEqPlane = flag;}
   void SetDrawEcPlane(bool flag) {mDrawEcPlane = flag;}
   void SetDrawEcLine(bool flag) {mDrawEcLine = flag;}
   void SetEqPlaneColor(unsigned int color) {mEqPlaneColor = color;}
   void SetEcPlaneColor(unsigned int color) {mEcPlaneColor = color;}
   void SetEcLineColor(unsigned int color) {mEcLineColor = color;}
   
   // events
   void OnPaint(wxPaintEvent &event);
   void OnSize(wxSizeEvent &event);
   void OnMouse(wxMouseEvent &event);

   // view
   bool IsInitialized();

   // actions
   void UpdatePlot();
   void ClearPlot();
   void ShowDefaultView();
   void ZoomIn();
   void ZoomOut();
   void DrawWireFrame(bool flag);
   void DrawEqPlane(bool flag);
   void DrawEcPlane(bool flag);
   void DrawEcLine(bool flag);
   void DrawInNewCoordSystem(CoordinateSystem *cs);
   void GotoStdBody(int bodyId);
   void GotoOtherBody(const wxString &bodyName);
   
   // data
   int  ReadTextTrajectory(const wxString &filename);
   void UpdateSpacecraft(const Real &time, const RealArray &posX,
                         const RealArray &posY, const RealArray &posZ,
                         const UnsignedIntArray &olor);
   
   // body
   void AddBody(const wxArrayString &bodyNames,
                const UnsignedIntArray &bodyColors);
   


private:
   
   static const int MAX_DATA = 20000;
   static const int LAST_STD_BODY_ID = 10;
   static const int MAX_COORD_SYS = 10;
   static const std::string BODY_NAME[GmatPlot::MAX_BODIES];
   static const unsigned int UNINIT_TEXTURE = 999;
   static const float MAX_ZOOM_IN = 3700.0;
   static const float RADIUS_ZOOM_RATIO = 2.2;
   static const float DEFAULT_DIST = 30000.0;
   
   TextTrajectoryFile *mTextTrajFile;
   TrajectoryArray mTrajectoryData;

   // Data members used by the mouse
   GLfloat m_fStartX, m_fStartY;

   // Actual params of the window
   GLfloat m_fLeftPos, m_fRightPos, m_fBottomPos, m_fTopPos;

   // Camera rotations
   GLfloat m_fCamRotationX, m_fCamRotationY, m_fCamRotationZ;

   // Camera translations
   GLfloat m_fCamTransX, m_fCamTransY, m_fCamTransZ;
    
   // draw option
   float mAxisLength;
   bool mDrawWireFrame;
   bool mDrawEqPlane;
   bool mDrawEcPlane;
   bool mDrawEclipticPlane;
   bool mDrawEcLine;
   bool mDrawSpacecraft;

   // coordinate sytem conversion
   bool mNeedSpacecraftConversion;
   CoordinateConverter mCoordConverter;
   
   // color
   unsigned int mEqPlaneColor;
   unsigned int mEcPlaneColor;
   unsigned int mEcLineColor;
   
   // texture
   GLuint mBodyTextureIndex[GmatPlot::MAX_BODIES];
   GLuint mScTextureIndex[GmatPlot::MAX_SCS];
   bool mUseTexture;
   
   // rotating
   bool mRotateXy;
   bool mRotateAboutXaxis;
   bool mRotateAboutYaxis;
   bool mRotateAboutZaxis;
   bool mRotateEarthToEnd;
   double lastLongitudeD;
   
   // zooming
   int   mLastMouseX;
   int   mLastMouseY;
   float mZoomAmount;
   float mMaxZoomIn;
   float mBodyMaxZoomIn[GmatPlot::MAX_BODIES];
   
   // initialization and limit
   bool mInitialized;
   bool mDdataCountOverLimit;
   int  mNumData;

   // time
   double mTime[MAX_DATA];

   // spacecraft
   int   mScCount;
   float mScGciPos[GmatPlot::MAX_SCS][MAX_DATA][3];
   float mScTempPos[GmatPlot::MAX_SCS][MAX_DATA][3];
   unsigned int mScTrajColor[GmatPlot::MAX_SCS][MAX_DATA];
   float mScRadius;
   GLuint mGlList;
   
   // solar system
   SolarSystem *mSolarSystem;
   
   // coordinate system
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mCoordSystemList[MAX_COORD_SYS];
   CoordinateSystem *mCurrCoordSystem;
   
   short mCurrViewFrame;
   short mCurrBody;
   int   mCenterViewBody;
   
   // earth
   float mEarthRadius;
   float mEarthGciPos[MAX_DATA][3];
   float mEarthTempPos[MAX_DATA][3];
   
   // bodies
   //std::string mBodyName[GmatPlot::MAX_BODIES];
   bool  mBodyInUse[GmatPlot::MAX_BODIES];
   bool  mBodyHasData[GmatPlot::MAX_BODIES];
   float mBodyRadius[GmatPlot::MAX_BODIES];
   float mBodyGciPos[GmatPlot::MAX_BODIES][MAX_DATA][3];
   float mTempBodyPos[GmatPlot::MAX_BODIES][MAX_DATA][3];
   short mPivotBodyIndex[GmatPlot::MAX_BODIES];
   int   mOtherBodyCount;
   
   // view
   wxSize mCanvasSize;
   GLfloat m_fViewLeft;
   GLfloat m_fViewRight;
   GLfloat m_fViewTop;
   GLfloat m_fViewBottom;
   GLfloat m_fViewNear;
   GLfloat m_fViewFar;
   float mDefaultViewX;
   float mDefaultViewY;
   float mDefaultViewZ;
   float mDefaultViewDist;
   float mCurrViewX;
   float mCurrViewY;
   float mCurrViewZ;
   float mCurrViewDist;
    
   // initialization
   bool LoadGLTextures();

   // view objects
   void SetupWorld();
   void SetProjection();
   void ComputeView(GLfloat fEndX, GLfloat fEndY);
   void ChangeView(float viewX, float viewY, float viewZ);
   //float viewDistance);
   void ChangeProjection(int width, int height, float axisLength);
    
   // drawing objects
   void DrawPicture();
   void DrawEarth();
   void DrawEarthOrbit();
   void DrawOtherBody(int bodyIndex);
   void DrawOtherBodyOrbit(int bodyIndex);
   void DrawSpacecraft(UnsignedInt scColor);
   void DrawSpacecraftOrbit();
   void DrawEquatorialPlane();
   void DrawEclipticPlane(); //loj: 1/31/05 Added
   void DrawEarthSunLine();  //loj: 1/31/05 Added
   
   // for body
   int GetStdBodyId(const std::string &name);

   // for coordinate sytem
   //CoordinateSystem* CreateCoordSystem();
   bool ConvertSpacecraftData();
   
   // for copy
   void CopyVector3(float to[3], double from[3]);
   void CopyVector3(float to[3], float from[3]);
   void CopyVector3(double to[3], double from[3]);
   void CopyVector3(double to[3], float from[3]);
   
   DECLARE_EVENT_TABLE();
};

#endif
