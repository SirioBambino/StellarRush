#pragma once

#include "Common.h"
#include "GameWindow.h"
#include "MatrixStack.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class CStar;
class CDiamond;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CFrameBufferObject;
class CQuad;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();
	void RenderScene(int pass);

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh* m_pStartLine;
	COpenAssetImportMesh* m_pSpaceship;
	COpenAssetImportMesh* m_pBlock;
	COpenAssetImportMesh* m_pAsteroid;
	CQuad *m_pQuad;
	CStar* m_pStar;
	CDiamond* m_pDiamond;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pTrack;
	CFrameBufferObject *m_pFBO;

	int m_windowWidth;
	int m_windowHeight;

	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;

	int m_cameraMode; // 0: free camera, 1: top camera, 2: third person camera

	bool m_gameStart;
	float m_gameStartTimer;

	bool m_isPaused;
	bool m_showInstructions;
	bool m_showFog;

	int m_health;
	int m_score;

	float m_currentDistance;
	float m_playerForwardSpeed;
	float m_playerStrafeSpeed;
	float m_spaceshipStrafeValue;
	float m_spaceshipStrafeTarget;

	float m_asteroidsRotation;
	float m_pickupsRotation;

	glm::vec3 m_spaceshipPosition;
	glm::mat4 m_spaceshipOrientation;

	vector<glm::vec3> m_asteroidPositions;
	vector<glm::vec3> m_asteroidRotationAxes;
	vector<glm::vec3> m_asteroidScales;

	vector<glm::vec3> m_diamondPositions;
	vector<glm::vec3> m_starPositions;
	vector<glm::vec3> m_blockPositions;

	bool m_isImmune;
	float m_immunityTime;
	float m_immunityTimer;

	vector<glm::vec3> GetRandomPositionsInSpace(int amount);
	vector<glm::vec3> GetRandomTrackPositions(int amount);
	void AssignPickupPositions();
	void RestartGame();

	unsigned int seed;

	float m_time;
	float m_distance;
	float m_speed;

	float m_volume;

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
};
