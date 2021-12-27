/*******************************************************************************************
*
*	raylib [models] example - Cubicmap loading and drawing
*
*	This example has been created using raylib 1.8 (www.raylib.com)
*	raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*	Copyright (c) 2015 Ramon Santamaria (@raysan5)
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
extern size_t _orbisNfs_lastopenFile_size;
#define ATTR_ORBIS_WIDTH 1920 
#define ATTR_ORBIS_HEIGHT 1080
bool flag=true;
OrbisPadConfig *confPad;
bool l1flag=false;
bool r1flag=false;

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
				orbisNfsInit("nfs://192.168.1.12/usr/local/orbisdev/git/ps4sh/bin/hostapp");
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
	orbisNfsFinish();
	finishOrbisLinkApp();
	kill(getpid(), SIGTERM);
	
}
unsigned char * nfsGetFileContent(char *path,int *size)
{
	unsigned char *fileContent;
	fileContent=orbisNfsGetFileContent(path);
	*size=_orbisNfs_lastopenFile_size;
	return fileContent;
}
int main(int argc, char *argv[])
{
	flag=initApp();
	if(!flag)
	{
		finishApp();
	}
	
	InitWindow(ATTR_ORBIS_WIDTH,ATTR_ORBIS_HEIGHT,"raylib [models] example - cubesmap loading and drawing");
	
	// Define the camera to look into our 3d world
	Camera3D camera = { { 16.0f, 14.0f, 16.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
	
	int dataSize; 
	unsigned char *fileData=nfsGetFileContent("resources/cubicmap.png",&dataSize);
	Image image=LoadImageFromMemory(".png",fileData,dataSize);
	Texture2D cubicmap = LoadTextureFromImage(image);       // Convert image to texture to display (VRAM)
	Mesh mesh = GenMeshCubicmap(image, (Vector3){ 1.0f, 1.0f, 1.0f });
	Model model = LoadModelFromMesh(mesh);
	
	fileData=nfsGetFileContent("resources/cubicmap_atlas.png",&dataSize);
	Image image2=LoadImageFromMemory(".png",fileData,dataSize);
	// NOTE: By default each cube is mapped to one part of texture atlas

	Texture2D texture = LoadTextureFromImage(image2);    // Load map texture
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;             // Set map diffuse texture
	Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };          // Set model position
	UnloadImage(image);     // Unload cubesmap image from RAM, already uploaded to VRAM
	SetCameraMode(camera, CAMERA_ORBITAL);  // Set an orbital camera mode

	SetTargetFPS(60);

	while(flag)
	{
		updateController();
		// Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera);              // Update camera
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
		BeginDrawing();

			ClearBackground(RAYWHITE);

			BeginMode3D(camera);

				DrawModel(model, mapPosition, 1.0f, WHITE);

			EndMode3D();

			DrawTextureEx(cubicmap, (Vector2){ ATTR_ORBIS_WIDTH - cubicmap.width*4 - 20, 20 }, 0.0f, 4.0f, WHITE);
			DrawRectangleLines(ATTR_ORBIS_WIDTH - cubicmap.width*4 - 20, 20, cubicmap.width*4, cubicmap.height*4, GREEN);

			DrawText("cubicmap image used to", ATTR_ORBIS_WIDTH-142, 90, 10, GRAY);
			DrawText("generate map 3d model", ATTR_ORBIS_WIDTH-142, 104, 10, GRAY);

            DrawFPS(10, 10);

        EndDrawing();
		
	}
	// De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(cubicmap);    // Unload cubicmap texture
    UnloadTexture(texture);     // Unload map texture
    UnloadModel(model);         // Unload map model

    CloseWindow();              // Close window and OpenGL context
	finishApp();
	return 0;
}