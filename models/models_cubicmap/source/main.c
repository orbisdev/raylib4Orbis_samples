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
#include <spine/spine.h>
#include <rayspine.h>
#include <spine/extension.h>
extern size_t _orbisNfs_lastopenFile_size;
#define ATTR_ORBIS_WIDTH 1920 
#define ATTR_ORBIS_HEIGHT 1080
bool flag=true;
OrbisPadConfig *confPad;
bool l1flag=false;
bool r1flag=false;


spAtlas* atlas = NULL;
spSkeletonJson* json;
spSkeletonData* skeletonData;
spSkeleton* skeleton;
spAnimationStateData* animationStateData;
spAnimationState* animationState;
Vector3 skeletonPosition = { 1920 / 2, 1080 / 2+125, 0};
int x=-62;
int y=132;
typedef enum 
{
	IDLE,
	WALK,
	RUN, 
	JUMP, 
	DEATH, 
	FALL
}State;
int playerStatus=IDLE;
int playerStatusPrev=IDLE;
int playerStatusChange=0;

typedef struct MapAnimTime
{
	spAnimation *animation;
	float time;
}MapAnimTime;

typedef struct StateView
{
	spAnimation *animation;
	bool loop;
	// Controls the start frame when changing from another animation to this animation.
	MapAnimTime startTimes[2];
	float defaultStartTime;
}StateView;
typedef struct Assets
{
	spAtlas *playerAtlas;
	spAtlas *enemyAtlas;
	//TextureRegion *bulletRegion, *hitRegion, *crosshair;
	//TextureRegion *titleRegion, *gameOverRegion, *youLoseRegion, *youWinRegion, *startRegion;

	spSkeletonData *playerSkeletonData;
	spSkeletonData *enemySkeletonData;
	spAnimationStateData *playerAnimationData;
	spAnimationStateData *enemyAnimationData;
	MapAnimTime startTimes[2];

	StateView playerStates[6];
	StateView enemyStates[5];
}Assets;
typedef struct PlayerView
{
	//View view;
	//Model model;
	spSkeleton *skeleton;
	spAnimationState *animationState;
	spEventData *footstepEvent;
	//Player player;
	spBone *rearUpperArmBone; 
	spBone *rearBracerBone;
	spBone *gunBone;
	spBone *headBone;
	spBone *torsoBone; 
	spBone *frontUpperArmBone;
	spAnimation *shootAnimation;
	spAnimation *hitAnimation;
	bool canShoot;
	float burstShots;
	float burstTimer;
	Vector2 temp1;
	Vector2 temp2;
}PlayerView;

Assets assets;
PlayerView playerView;
char * getAnimation(int status)
{

	switch(status)
	{
		case 0:
			return "idle";
		case 1:
			return "walk";
		case 2:
			return "run";
		case 3:
			return "jump";
		case 4:
			return "death";
		case 5: 
			return "fall";
		default:
			return NULL;
	}
}

void updatePlayerStatus(int status)
{
	if(playerStatus==status)
	{
		playerStatusChange=0;
	}
	else
	{
		TraceLog(LOG_INFO,"player status change from %d to %d",playerStatus,status);
		playerStatusChange=1;
		playerStatusPrev=playerStatus;
		playerStatus=status;
	}
}

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
void setupState(StateView *map, State state, spSkeletonData *skeletonData, const char* name, bool loop)
{
	map[state].animation = spSkeletonData_findAnimation(skeletonData,name);
	map[state].loop = loop;
	return;
}
void loadAssets(Assets *this)
{
	this->playerAtlas = spAtlas_createFromFile("/data/raylib/assets/spineboy/spineboy-pma.atlas",0);
	spSkeletonJson *json = spSkeletonJson_create(this->playerAtlas);

	//json->scale=(this->player->height /this->player->heightSource);
	this->playerSkeletonData = spSkeletonJson_readSkeletonDataFile(json,"/data/raylib/assets/spineboy/spineboy-ess.json");
	if (!this->playerSkeletonData)
	{
	    debugNetPrintf(DEBUGNET_ERROR,"%s %s\n",__FUNCTION__,json->error);
	    spSkeletonJson_dispose(json);
	}

	this->playerAnimationData = spAnimationStateData_create(this->playerSkeletonData);
	this->playerAnimationData->defaultMix=(0.2f);
	spAnimationStateData_setMixByName(this->playerAnimationData,"idle", "run", 0.3f);
	spAnimationStateData_setMixByName(this->playerAnimationData,"run", "idle", 0.1f);
	spAnimationStateData_setMixByName(this->playerAnimationData,"shoot", "shoot", 0);

	

	setupState(this->playerStates, DEATH, this->playerSkeletonData, "death", false);

	setupState(this->playerStates, IDLE, this->playerSkeletonData, "idle", true);
	setupState(this->playerStates, JUMP, this->playerSkeletonData, "jump", false);
	setupState(this->playerStates, RUN, this->playerSkeletonData, "run", true);
	
	if(this->playerStates[IDLE].animation != NULL)
	{
		this->playerStates[RUN].startTimes[0].animation=this->playerStates[IDLE].animation;
		this->playerStates[RUN].startTimes[0].time=8 * 60;
	}

	if(this->playerStates[JUMP].animation != NULL)
	{
		this->playerStates[RUN].startTimes[1].animation=this->playerStates[JUMP].animation;
		this->playerStates[RUN].startTimes[1].time=22 * 60;
	}
	setupState(this->playerStates, FALL, this->playerSkeletonData, "jump", false);
	this->playerStates[FALL].defaultStartTime=22*60;
}
void _spAtlasPage_createTexture(spAtlasPage *self, const char *path)
{
	Texture2D *t=SpineCreateTexture2d((char *) path);
	self->rendererObject=t;
	self->width=t->width;
	self->height=t->height;
}
char *_spUtil_readFile(const char *path, int *length) {
    return _spReadFile(path, length);
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    if (self->rendererObject == NULL) return;
    Texture2D *t2d = self->rendererObject;
    UnloadTexture(*t2d);
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
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	debugNetPrintf(DEBUGNET_INFO,"[SPINE] loading assets");
	
	loadAssets(&assets);

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