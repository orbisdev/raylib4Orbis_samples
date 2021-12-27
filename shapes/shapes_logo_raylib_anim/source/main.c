/*******************************************************************************************
*
*   raylib [shapes] example - raylib logo animation
*
*   This example has been created using raylib 2.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*	Copyright (c) 2020 Antonio Jose Ramos Marquez (@psxdev) adapt sample to orbisdev
********************************************************************************************/

#include <stdio.h>
#include <user_mem.h>
#include <stdlib.h>
#include <string.h>
#include <orbisdev.h>
#include <orbislink.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <signal.h>
#include <debugnet.h>
#include <fcntl.h>
#include <orbisNfs.h>
#include <sqlite3.h>
#include <raylib.h>	

#define ATTR_ORBIS_WIDTH 1920 
#define ATTR_ORBIS_HEIGHT 1080
bool flag=true;
OrbisPadConfig *confPad;
bool l1flag=false;
bool r1flag=false;
int xflag;

void updateController()
{
	int ret;
	unsigned int buttons=0;
	ret=orbisPadUpdate();
	if(ret==0)
	{
		if(orbisPadGetButtonPressed(ORBISPAD_L2|ORBISPAD_R2) || orbisPadGetButtonHold(ORBISPAD_L2|ORBISPAD_R2))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Combo L2R2 pressed\n");
			buttons=orbisPadGetCurrentButtonsPressed();
			buttons&= ~(ORBISPAD_L2|ORBISPAD_R2);
			orbisPadSetCurrentButtonsPressed(buttons);
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L1|ORBISPAD_R1) )
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Combo L1R1 pressed\n");
			buttons=orbisPadGetCurrentButtonsPressed();
			buttons&= ~(ORBISPAD_L1|ORBISPAD_R1);
			orbisPadSetCurrentButtonsPressed(buttons);
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L1|ORBISPAD_R2) || orbisPadGetButtonHold(ORBISPAD_L1|ORBISPAD_R2))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Combo L1R2 pressed\n");
			buttons=orbisPadGetCurrentButtonsPressed();
			buttons&= ~(ORBISPAD_L1|ORBISPAD_R2);
			orbisPadSetCurrentButtonsPressed(buttons);
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L2|ORBISPAD_R1) || orbisPadGetButtonHold(ORBISPAD_L2|ORBISPAD_R1) )
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Combo L2R1 pressed\n");
			buttons=orbisPadGetCurrentButtonsPressed();
			buttons&= ~(ORBISPAD_L2|ORBISPAD_R1);
			orbisPadSetCurrentButtonsPressed(buttons);
		}
		if(orbisPadGetButtonPressed(ORBISPAD_UP) || orbisPadGetButtonHold(ORBISPAD_UP))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Up pressed\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_DOWN) || orbisPadGetButtonHold(ORBISPAD_DOWN))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Down pressed\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_RIGHT) || orbisPadGetButtonHold(ORBISPAD_RIGHT))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Right pressed\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_LEFT) || orbisPadGetButtonHold(ORBISPAD_LEFT))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Left pressed\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_TRIANGLE))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Triangle pressed exit\n");
			flag=0;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_CIRCLE))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Circle pressed\n");            
		}
		if(orbisPadGetButtonPressed(ORBISPAD_CROSS))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Cross pressed\n");
			xflag=1;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_SQUARE))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"Square pressed\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L1))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"L1 pressed %d\n");
			l1flag=1;
			
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L2))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"L2 pressed %d\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_R1))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"R1 pressed\n");
			r1flag=1;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_R2))
		{
			debugNetPrintf(DEBUGNET_DEBUG,"R2 pressed\n");
		}
	}
}

bool initApp()
{
	int ret;
	ret=initOrbisLinkAppVanillaGl();
	if(ret==ORBISLINK_OK)
	{
		ret=orbisLinkLoadModule(SCE_SYSMODULE_INTERNAL_PAD);
		if(ret==ORBISLINK_OK)
		{
			ret=orbisPadInit();
			if(ret==1)
			{
				confPad=orbisPadGetConf();
				sceSystemServiceHideSplashScreen();
				return true;
			}
		}
	}
	return false;
}
void finishApp()
{
	orbisPadFinish();
	finishOrbisLinkApp();
	kill(getpid(), SIGTERM);
	
}
int main(int argc, char *argv[])
{
	flag=initApp();
	if(!flag)
	{
		finishApp();
	}
	
	InitWindow(ATTR_ORBIS_WIDTH,ATTR_ORBIS_HEIGHT,"raylib [shapes] example - raylib logo animation");
	const int screenWidth = ATTR_ORBIS_WIDTH;
    const int screenHeight = ATTR_ORBIS_HEIGHT;

	int logoPositionX = screenWidth/2 - 128;
	int logoPositionY = screenHeight/2 - 128;
	int framesCounter = 0;
	int lettersCount = 0;
	int topSideRecWidth = 16;
	int leftSideRecHeight = 16;
	int bottomSideRecWidth = 16;
	int rightSideRecHeight = 16;
	int state = 0;                  // Tracking animation states (State Machine)
	float alpha = 1.0f;             // Useful for fading
	const char msg1[21] = "powered by orbisdev";
	SetTargetFPS(60);

	while(flag)
	{
		updateController();

		BeginDrawing();
			if (state == 0)                 // State 0: Small box blinking
		{
			framesCounter++;
			if (framesCounter == 120)
			{
				state = 1;
				framesCounter = 0;      // Reset counter... will be used later...
			}
		}
		else if (state == 1)            // State 1: Top and left bars growing
		{
			topSideRecWidth += 4;
			leftSideRecHeight += 4;
			if (topSideRecWidth == 256) state = 2;
		}
		else if (state == 2)            // State 2: Bottom and right bars growing
		{
			bottomSideRecWidth += 4;
			rightSideRecHeight += 4;
			if (bottomSideRecWidth == 256) state = 3;
		}
		else if (state == 3)            // State 3: Letters appearing (one by one)
		{
			framesCounter++;
			if (framesCounter/12)       // Every 12 frames, one more letter!
			{
				lettersCount++;
				framesCounter = 0;
			}
			if (lettersCount >= 10)     // When all letters have appeared, just fade out everything
			{
				alpha -= 0.02f;
				if (alpha <= 0.0f)
				{
					alpha = 0.0f;
					state = 4;
				}
			}
		}
		else if (state == 4)            // State 4: Reset and Replay
		{
			if (xflag)
			{
				framesCounter = 0;
				lettersCount = 0;
				topSideRecWidth = 16;
				leftSideRecHeight = 16;
				bottomSideRecWidth = 16;
				rightSideRecHeight = 16;
				alpha = 1.0f;
				state = 0;          // Return to State 0
				xflag=0;
			}
		}
		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
			ClearBackground(RAYWHITE);
			if (state == 0)
			{
				if ((framesCounter/15)%2) DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);
			}
			else if (state == 1)
			{
				DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
				DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
			}
			else if (state == 2)
			{
				DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
				DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
				DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);
				DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);
			}
			else if (state == 3)
			{
				DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, Fade(BLACK, alpha));
				DrawRectangle(logoPositionX, logoPositionY + 16, 16, leftSideRecHeight - 32, Fade(BLACK, alpha));
				DrawRectangle(logoPositionX + 240, logoPositionY + 16, 16, rightSideRecHeight - 32, Fade(BLACK, alpha));
				DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, Fade(BLACK, alpha));
				DrawRectangle(screenWidth/2 - 112, screenHeight/2 - 112, 224, 224, Fade(RAYWHITE, alpha));
				DrawText(TextSubtext("raylib", 0, lettersCount), screenWidth/2 - 44, screenHeight/2 + 48, 50, Fade(BLACK, alpha));
			}
			else if (state == 4)
			{	
				//draw msg1 text 
				DrawText(msg1, screenWidth/2-262, screenHeight/2-68, 50, BLACK);
			}
		EndDrawing();
	}
	CloseWindow();
	finishApp();
	return 0;
}