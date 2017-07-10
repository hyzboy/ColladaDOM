/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include "viewer_base.pch.h" //PCH (glext.h via FX.pch.h)

#include <stdlib.h>
#include <string>
#include <sys/paths.h>
							
#include <sys/process.h>
#include <sys/paths.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "FWGLCamControlApplication.h"
#include "FWDisplayInfo.h"
#include <cell/sysmodule.h>
#include <cell/dbgfont.h>

#include "PS3_BrowserUI.h"

#include "../../src/viewer_base.inl"

class COLLADA_Viewer : public FWGLCamControlApplication
{
public:

	COLLADA_Viewer();
	~COLLADA_Viewer(){}
	virtual bool onInit(int argc, char **ppArgv);
	virtual void onShutdown();
	virtual void onRender();
	virtual bool onUpdate();
	virtual void ParseDir(char *path);
	virtual void InitFS(){}
	virtual void checkFS(){}

private:

	std::string resourceName;
	static void loadFunction();

	//this just manipulates the Physics gravity for some reason
	void SixAxis();
	
	//pad data 
	FWInputDevice *mpPad;
	FWInputFilter *mpInputX0;
	FWInputFilter *mpInputY0;
	FWInputFilter *mpInputX1;
	FWInputFilter *mpInputY1;

	//FPS REPORTING
	FWTimeVal mLastTime;
	const static float REPORT_TIME; //5.0
	int frames;
	int framesLastReport;
	double timeElapsed;
	double timeLastReport;
	double timeReport;

	CellDbgFontConsoleId mDbgFontID;
	bool mRunning;
	GLuint m_vertex_buffers,m_color_buffers;

	const char *mCurrentFile;
	BrowserUI Browser;

}app; //instantiate the class
const float COLLADA_Viewer::REPORT_TIME = 5.0;

bool load_ok = false;

COLLADA_Viewer::COLLADA_Viewer()
{
	strcpy(mStartupInfo.mWindowTitle,"COLLADA Viewer");
	strcpy(mStartupInfo.mArgumentsFilename,"fwArgs.txt");

	mStartupInfo.mUseDebugConsole = false;

	//FPS REPORTING
	frames = 0;
	framesLastReport = 0;
	timeElapsed = 0.0;
	timeLastReport = 0.0;
	timeReport = REPORT_TIME;
	mRunning = true;
	mCurrentFile = 0;
}

void COLLADA_Viewer::ParseDir(char *path)
{
	DIR *dir; struct dirent *dent;
	dir = opendir(path); if(dir==0)
	{
		printf("PS3: can't open %s\n",path);
		return;
	}

	char pathname[MAX_PATH];
	char t_pathname[MAX_PATH]; for(;;)
	{
		if((dent=readdir(dir)==0)
		break;

		if(dent->d_type==DT_DIR)
		{
			if(strcmp(dent->d_name,".")==0||strcmp(dent->d_name,"..")==0)
			continue;
			sprintf(pathname,"%s/%s",path,dent->d_name);
			ParseDir(pathname);
		}
		else if(dent->d_type==DT_REG)
		{
			size_t filelen = strlen(dent->d_name);
			if(((dent->d_name[filelen-4]=='.')) 
			&&((dent->d_name[filelen-3]=='d')||(dent->d_name[filelen-3]=='D')) 
			&&((dent->d_name[filelen-2]=='a')||(dent->d_name[filelen-2]=='A'))
			&&((dent->d_name[filelen-1]=='e')||(dent->d_name[filelen-1]=='E')))
			{
				sprintf(pathname,"%s/%s",path,dent->d_name);
				printf("PS3: %s is a collada document\n",pathname);
				sprintf(t_pathname,"%s/%s.raw",path,dent->d_name);
				Browser.AddThumbnail(pathname,t_pathname);
			}
		}
	}

	closedir(dir);
}

bool COLLADA_Viewer::onInit(int argc, char **ppArgv)
{
	FWGLApplication::onInit(argc,ppArgv);

	glClearColor(0.3f,0.3f,0.7f,0);
	glClearDepthf(1.0f);
	glEnable(GL_DEPTH_TEST);

	FWDebugFont::setColor(1,1,1,1);

	InitFS();
	Browser.Init();

	psglLoadShaderLibrary("/app_home/shaders.bin");

	//Initialize the renderer
	RT::Main.Init(RestoreGL);
	RT::Main.UseVBOs = true;
	RT::Main.ShowHiearchy = true;
	InitGL(0,0,1,0.5f);

	cgRTCgcInit();
	ParseDir("/app_home/");

	//Load the target file
	Browser.onUpdate();
	if(argc>0)
	{
		char path_name[1024];
		mCurrentFile = ppArgv[0];
		sprintf(path_name,"%s%s","/app_home/",ppArgv[0]);
		printf(path_name);
		load_ok = RT::Main.Load(path_name);
		if(!load_ok)
		return false;
	}
	else if(Browser.GetSize()>0)
	{
		mCurrentFile = Browser.GetThumbnail(0)->GetDocument();
		load_ok = RT::Main.Load(mCurrentFile);
		if(!load_ok)
		return false;
	}
	//Get the gamepad so we can read it later 
	mpPad = FWInput::getDevice(FWInput::DeviceType_Pad,0);
	if(mpPad==nullptr)
	{
		printf("PS3: Error, couldn't get a pad\n");
		exit(0);
	}

	mpInputX0 = mpPad->bindFilter();
	mpInputX0->setChannel(FWInput::Channel_XAxis_0);
	mpInputX0->setGain(1.0f);
	mpInputX0->setDeadzone(0.3f);

	mpInputY0 = mpPad->bindFilter();
	mpInputY0->setChannel(FWInput::Channel_YAxis_0);
	mpInputY0->setGain(1.0f);
	mpInputY0->setDeadzone(0.3f);

	mpInputX1 = mpPad->bindFilter();
	mpInputX1->setChannel(FWInput::Channel_XAxis_1);
	mpInputX1->setGain(1.0f);
	mpInputX1->setDeadzone(0.3f);

	mpInputY1 = mpPad->bindFilter();
	mpInputY1->setChannel(FWInput::Channel_YAxis_1);
	mpInputY1->setGain(1.0f);
	mpInputY1->setDeadzone(0.3f);


	//initialize debug font library, then open console.
	int ret;
	CellDbgFontConfig cfg;
	memset(&cfg,0,sizeof(CellDbgFontConfig));
	cfg.bufSize = 512;
	cfg.screenWidth = mDispInfo.mWidth;
	cfg.screenHeight = mDispInfo.mHeight;
	ret = cellDbgFontInit(&cfg);
	if(ret!=CELL_OK)
	{
		printf("PS3: cellDbgFontInit() failed %x\n",ret);
		return true;
	}

	CellDbgFontConsoleConfig ccfg;
	memset(&ccfg,0,sizeof(CellDbgFontConsoleConfig));
	ccfg.posLeft = 0.1f;
	ccfg.posTop = 0.6f;
	ccfg.cnsWidth = 16;
	ccfg.cnsHeight = 4;
	ccfg.scale = 1.5f;
	ccfg.color = 0xffffffff;
	mDbgFontID = cellDbgFontConsoleOpen(&ccfg);
	if(mDbgFontID<0)
	{
		printf("PS3: cellDbgFontConsoleOpen() failed %x\n",mDbgFontID);
		return true;
	}

	return true;
}

bool COLLADA_Viewer::onUpdate()
{
	float tolerrance = 0.15f;
	static float buttonTime = 0;
	static float cameraTime = 0;
	bool result = FWGLApplication::onUpdate();

	if(mRunning==false)
	return result;

	FWTimeVal curTime = FWTime::getCurrentTime();

	//execute these control base on time elapse, not frame rate
	if((float)curTime-buttonTime>0.15f)
	{
		buttonTime = curTime;
		if(mpPad->getRawBool(FWInput::Channel_Button_Select))
		{
			ProcessInput('\t'); //RT::Main.SetNextCamera();
			return result;
		}
		else if(mpPad->getRawBool(FWInput::Channel_Button_L3))
		{
			Browser.SetVisible(!Browser.IsVisible());
		}
		else if(mpPad->getRawBool(FWInput::Channel_Button_R3))
		{
			ProcessInput('Q'); //togglewireframe = !togglewireframe;
		}
		else if(mpPad->getRawBool(FWInput::Channel_Button_Square))
		{
			ProcessInput('L'); //togglelighting = !togglelighting;
		}
		else if(mpPad->getRawBool(FWInput::Channel_Button_Cross))
		{
			mCurrentFile = Browser.GetThumbnail(Browser.GetSelection())->GetDocument();
			load_ok = RT::Main.Load(mCurrentFile);
			return result;
		}
		else if(mpPad->getRawBool(FWInput::Channel_Button_Up))
		{
			Browser.SelectPrev();
		}
		else if(mpPad->getRawBool(FWInput::Channel_Button_Down))
		{
			Browser.SelectNext();
		}
	}

	//execute these control base on time elapse, not frame rate
	if((float)curTime-cameraTime>0.05f) 
	{
		cameraTime = curTime;
		//Get the values from the analog sticks
		float conditioned_X_0 = mpPad->getRawFloat(FWInput::Channel_XAxis_0);
		float conditioned_Y_0 = mpPad->getRawFloat(FWInput::Channel_YAxis_0);
		float conditioned_X_1 = mpPad->getRawFloat(FWInput::Channel_XAxis_1);
		float conditioned_Y_1 = mpPad->getRawFloat(FWInput::Channel_YAxis_1);

		if(-tolerrance<conditioned_X_0&&conditioned_X_0<tolerrance) conditioned_X_0 = 0;
		if(-tolerrance<conditioned_Y_0&&conditioned_Y_0<tolerrance) conditioned_Y_0 = 0;
		if(-tolerrance<conditioned_X_1&&conditioned_X_1<tolerrance) conditioned_X_1 = 0;
		if(-tolerrance<conditioned_Y_1&&conditioned_Y_1<tolerrance) conditioned_Y_1 = 0;

		conditioned_X_0 = mpInputX0?-mpInputX0->getFloatValue():0.f;
		conditioned_Y_0 = mpInputY0?-mpInputY0->getFloatValue():0.f;
		conditioned_X_1 = mpInputX1?-mpInputX1->getFloatValue():0.f;
		conditioned_Y_1 = mpInputY1?-mpInputY1->getFloatValue():0.f;

		float multiplier = 10;
		if(conditioned_X_0!=0||conditioned_Y_0!=0)
		RT::Main.Fly(conditioned_X_0*multiplier,conditioned_Y_0*multiplier);		
		if(conditioned_X_1!=0||conditioned_Y_1!=0)		
		RT::Main.Fly(conditioned_X_1*multiplier,conditioned_Y_1*multiplier);
	}

	//zoom in/out
	if(mpPad->getRawBool(FWInput::Channel_Button_L2))
	RT::Main.ZoomIn(-0.005f);	
	else if(mpPad->getRawBool(FWInput::Channel_Button_L1))
	RT::Main.ZoomIn(0.005f);

	if(togglelighting)
	glEnable(GL_LIGHTING);	
	else
	glDisable(GL_LIGHTING);

	//this just manipulates the Physics gravity for some reason
	SixAxis();
	
	enum{ CONS_PUTS_INTERVAL=50 };
	if(0==frames%CONS_PUTS_INTERVAL)
	{
		//2017: ???
	}

	char title[1024];
	const char *selectedfile = Browser.GetThumbnail(Browser.GetSelection())->GetDocument();
	sprintf(title,"%s %s","COLLADA Viewer",mCurrentFile);
	cellDbgFontPuts(0.1f,0.1f,1.0f,0xffffffff,title);
	cellDbgFontPuts(0.1f,0.9f,1.0f,0xffffffff,selectedfile);

	Browser.onUpdate(); return result;
}

void COLLADA_Viewer::onRender()
{
	//base implementation clears screen and sets up camera

	FWGLApplication::onRender();

	if(!load_ok)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		Browser.onRender();
		cellDbgFontPuts(0.5f,0.5f,1.0f,0xff00ffff,"Load Error!");
		cellDbgFontDraw();
		return;
	}

	if(!mRunning)
	return;

	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	RT::Main.Refresh();
	glPopMatrix();
	
	//FPS REPORTING
	glDisable(GL_VSYNC_SCE);
	//get current timing info
	FWTimeVal timeNow = FWTime::getCurrentTime();
	float fElapsedInFrame = float(timeNow-mLastTime);
	mLastTime = timeNow;
	++frames;
	timeElapsed += fElapsedInFrame;
	//report fps at appropriate interval
	if(timeElapsed>=timeReport)
	{
		//printf("PS3: %.2f FPS\n",(frames-framesLastReport)*1.f/(float)(timeElapsed-timeLastReport));
		timeReport += REPORT_TIME;
		timeLastReport = timeElapsed;
		framesLastReport = frames;
	}

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	Browser.onRender();

	cellDbgFontDraw();
}

void COLLADA_Viewer::onShutdown()
{
	FWGLApplication::onShutdown();
	RT::Main.Destroy();

	cellDbgFontConsoleClose(mDbgFontID);
	cellDbgFontExit();
}

void COLLADA_Viewer::SixAxis()
{			  
	#if defined(SPU_BULLET)
	{
		if(!mpPad->getRawBool(FWInput::Channel_Button_Triangle))
		{
			RT::Main.Physics.SetGravity(FX::Float3(0,-10,0));
			return;
		}

		float fl_x,fl_y,fl_z,fl_g;
		fl_x = mpPad->getRawFloat(FWInput::Channel_XAxis_2);
		fl_y = mpPad->getRawFloat(FWInput::Channel_YAxis_2);
		fl_z = mpPad->getRawFloat(FWInput::Channel_ZAxis_2);
		//fl_g = mpPad->getRawFloat(FWInput::Channel_Gyro);
		
		static float dampedX = 0;
		static float dampedY = 0;
		static float dampedZ = 0;

		dampedX = (fl_x+10.f*dampedX)/11.f;
		dampedY = (fl_y+10.f*dampedY)/11.f;
		dampedZ = (fl_z+10.f*dampedZ)/11.f;

		btTransform floorTrans;
		floorTrans.setIdentity();

		btVector3 xAxis(1,0,0);
		btVector3 zAxis(0,0,1);
		btQuaternion rotX(zAxis,dampedX*10.f);
		btQuaternion rotZ(xAxis,-dampedZ*10.f);

		btQuaternion combinedRot = rotZ*rotX;

		btMatrix3x3 orn;
		orn.setRotation(combinedRot);
		btVector3 up = orn.getColumn(1);

		FX::Float3 newG(up.getX(),up.getY(),up.getZ());
		RT::Main.Physics.SetGravity(newG);
		//printf("PS3: new gravity (%f, %f, %f)\n",newG.getX(),newG.getY(),newG.getZ());
	}
	#endif //SPU_BULLET
}