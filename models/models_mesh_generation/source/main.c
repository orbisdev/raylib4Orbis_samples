/*******************************************************************************************
*
*   raylib example - procedural mesh generation
*
*   This example has been created using raylib 1.8 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (Ray San)
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
#define NUM_MODELS  8      // Parametric 3d shapes to generate

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
	
	InitWindow(ATTR_ORBIS_WIDTH,ATTR_ORBIS_HEIGHT,"raylib [models] example - mesh generation");
	Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
	Texture2D texture =LoadTextureFromImage(checked);
    UnloadImage(checked);

    Model models[NUM_MODELS] = { 0 };

	models[0] = LoadModelFromMesh(GenMeshPlane(2, 2, 5, 5));
	models[1] = LoadModelFromMesh(GenMeshCube(2.0f, 1.0f, 2.0f));
    models[2] = LoadModelFromMesh(GenMeshSphere(2, 32, 32));
    models[3] = LoadModelFromMesh(GenMeshHemiSphere(2, 16, 16));
    models[4] = LoadModelFromMesh(GenMeshCylinder(1, 2, 16));
    models[5] = LoadModelFromMesh(GenMeshTorus(0.25f, 4.0f, 16, 32));
    models[6] = LoadModelFromMesh(GenMeshKnot(1.0f, 2.0f, 16, 128));
    models[7] = LoadModelFromMesh(GenMeshPoly(5, 2.0f));
    for (int i = 0; i < NUM_MODELS; i++) models[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
	Camera camera = { { 5.0f, 5.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };

    // Model drawing position
    Vector3 position = { 0.0f, 0.0f, 0.0f };

    int currentModel = 0;

    SetCameraMode(camera, CAMERA_ORBITAL);  // Set a orbital camera mode
	SetTargetFPS(60);

	while(flag)
	{
		updateController();
		UpdateCamera(&camera);      // Update internal camera and our camera
		if (r1flag)
        {
            currentModel++;
            if (currentModel >= NUM_MODELS) currentModel = 0;
            r1flag=0;
        }
        else if (l1flag)
        {
            currentModel--;
            if (currentModel < 0) currentModel = NUM_MODELS - 1;
            l1flag=0;
        }
		BeginDrawing();
			ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawModel(models[currentModel], position, 1.0f, WHITE);

                DrawGrid(10, 1.0);

            EndMode3D();

            DrawRectangle(30, ATTR_ORBIS_HEIGHT-40, 310, 30, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(30, ATTR_ORBIS_HEIGHT-40, 310, 30, Fade(DARKBLUE, 0.5f));
            DrawText("L1/R1  BUTTON to CYCLE PROCEDURAL MODELS", 40, ATTR_ORBIS_HEIGHT-30, 10, BLUE);

            switch(currentModel)
            {
                case 0: DrawText("PLANE", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 1: DrawText("CUBE", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 2: DrawText("SPHERE", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 3: DrawText("HEMISPHERE", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 4: DrawText("CYLINDER", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 5: DrawText("TORUS", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 6: DrawText("KNOT", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                case 7: DrawText("POLY", ATTR_ORBIS_WIDTH-150, 10, 20, DARKBLUE); break;
                default: break;
            }
			DrawFPS(10, 10);
		EndDrawing();
	}
    UnloadTexture(texture); // Unload texture
	
	for (int i = 0; i < NUM_MODELS; i++) UnloadModel(models[i]);

	CloseWindow();
	finishApp();
	return 0;
}