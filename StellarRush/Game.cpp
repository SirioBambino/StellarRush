/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.
 
 Template version 4.0a 30/01/2016
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/

#include "game.h"

// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Quad.h"
#include "Star.h"
#include "Diamond.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "FrameBufferObject.h"


// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pFtFont = NULL;
	m_pStartLine = NULL;
	m_pSpaceship = NULL;
	m_pBlock = NULL;
	m_pQuad = NULL;
	m_pDiamond = NULL;
	m_pStar = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pTrack = NULL;
	m_pFBO = NULL;
	m_pAsteroid = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;

	m_cameraMode = 2;

	m_gameStart = false;
	m_gameStartTimer = -1.0f;

	m_isPaused = false;
	m_showInstructions = false;
	m_showFog = true;

	m_health = 100;
	m_score = 0;

	m_currentDistance = 0.0f;
	m_playerForwardSpeed = 0.25f;
	m_playerStrafeSpeed = 0.05f;
	m_spaceshipStrafeValue = 0.0f;
	m_spaceshipStrafeTarget = 0.0f;

	m_pickupsRotation = 0.0f;
	m_asteroidsRotation = 0.0f;

	m_isImmune = false;
	m_immunityTime = 5.0f;
	m_immunityTimer = 0.0f;

	m_time = 0.0f;
	m_distance = 0.0f;
	m_speed = 0.0f;

	m_volume = 0.1f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pFtFont;
	delete m_pStartLine;
	delete m_pSpaceship;
	delete m_pBlock;
	delete m_pQuad;
	delete m_pDiamond;
	delete m_pStar;
	delete m_pAudio;
	delete m_pTrack;
	delete m_pFBO;
	delete m_pAsteroid;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Initialise seed using current time
	srand((unsigned)time(NULL));

	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pFtFont = new CFreeTypeFont;
	m_pStartLine = new COpenAssetImportMesh;
	m_pSpaceship = new COpenAssetImportMesh;
	m_pBlock = new COpenAssetImportMesh;
	m_pQuad = new CQuad;
	m_pDiamond = new CDiamond;
	m_pStar = new CStar;
	m_pAudio = new CAudio;
	m_pTrack = new CCatmullRom;
	m_pFBO = new CFrameBufferObject;
	m_pAsteroid = new COpenAssetImportMesh;

	// Set the window's width and height
	RECT dimensions = m_gameWindow.GetDimensions();

	m_windowWidth = dimensions.right - dimensions.left;
	m_windowHeight = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(m_windowWidth, m_windowHeight); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) m_windowWidth / (float) m_windowHeight, 0.5f, 15000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Load the font
	m_pFtFont->LoadFont("resources\\fonts\\Orbitron-Regular.ttf", 60);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Create the skybox
	m_pSkybox->Create(2500.0f); // Downloaded from https://opengameart.org/content/ulukais-space-skyboxes on 26 March 2023

	// Load some meshes in OBJ format
	m_pStartLine->Load("resources\\models\\StartLine\\StartLine.obj");	// Downloaded from https://quaternius.com/packs/ultimatespacekit.html on 03 March 2023
	m_pSpaceship->Load("resources\\models\\Spaceship\\Spaceship.obj");	// Downloaded from https://quaternius.com/packs/ultimatespacekit.html on 03 March 2023
	m_pBlock->Load("resources\\models\\Block\\Block.obj");				// Downloaded from https://quaternius.com/packs/ultimatespacekit.html on 03 March 2023
	m_pAsteroid->Load("resources\\models\\Asteroid\\Asteroid.obj");		// Downloaded from https://opengameart.org/content/low-poly-rocks on 04 April 2023

	// Create a diamond
	m_pDiamond->Create("resources\\textures\\diamond.png");

	// Create a star
	m_pStar->Create("resources\\textures\\star.png");

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("Resources\\Audio\\beep_1.ogg");		// Royalty free sound from https://opengameart.org/content/50-cc0-sci-fi-sfx
	m_pAudio->LoadEventSound("Resources\\Audio\\beep_2.ogg");		// Royalty free sound from https://opengameart.org/content/50-cc0-sci-fi-sfx
	m_pAudio->LoadEventSound("Resources\\Audio\\explosion.ogg");	// Royalty free sound from https://opengameart.org/content/50-cc0-sci-fi-sfx
	m_pAudio->LoadEventSound("Resources\\Audio\\death.ogg");		// Royalty free sound from https://opengameart.org/content/50-cc0-sci-fi-sfx
	m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	
	m_pAudio->PlayMusicStream();
	m_pAudio->SetEventVolume(m_volume);
	m_pAudio->SetMusicVolume(m_volume);

	// Create the track
	m_pTrack->CreateCentreline();
	m_pTrack->CreateOffsetCurves();
	m_pTrack->CreateTrack();

	// Create quad
	m_pQuad->Create("resources\\textures\\hud.png");

	// Create FBO with the window size
	m_pFBO->Create(m_windowWidth, m_windowHeight);

	// Get random positions for the asteroids and the pickups and store them in vectors
	AssignPickupPositions();
	m_asteroidPositions = GetRandomPositionsInSpace(100);

	// For each asteroid position create a random rotation axis and random scale and store them in vectors
	for (auto p : m_asteroidPositions)
	{
		m_asteroidRotationAxes.push_back(glm::vec3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX));
		m_asteroidScales.push_back(glm::vec3(rand() % 10 + 10, rand() % 10 + 10, rand() % 10 + 10) * (float)(rand() % 5 + 5));
	}
}

void Game::Render()
{
	// Use the main shader program 
	CShaderProgram* pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();

	// Bind FBO
	m_pFBO->Bind();

	// First pass
	RenderScene(0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int pass = 0;

	// Blur the screen when instructions are shown
	if (m_showInstructions)
	{
		pMainProgram->SetUniform("blur", true);
		pass = 1;
	}
	else
	{
		pMainProgram->SetUniform("blur", false);
		pass = 0;
	}

	// Second pass
	RenderScene(pass);

	// Render HUD if player is alive
	if (m_health > 0)
	{
		pMainProgram->SetUniform("renderScreenQuad", true);
		m_pQuad->Render();
		pMainProgram->SetUniform("renderScreenQuad", false);
	}

	DisplayFrameRate();

	// Use the font shader program 
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(.9f, .9f, .9f, 1.0f));

	// Render text
	string text;
	const char* ctext;
	int offset;

	if (!m_showInstructions)
	{
		// Render HUD text if player is alive
		if (m_health > 0)
		{
			m_pFtFont->Render(m_windowWidth / 5.6f, m_windowHeight / 20, 35, "Health %d", m_health);
			m_pFtFont->Render(m_windowWidth / 2.56f, m_windowHeight / 20, 35, "Score");
			m_pFtFont->Render(m_windowWidth / 1.9f, m_windowHeight / 20, 35, "%d", m_score);
			m_pFtFont->Render(m_windowWidth / 1.5f, m_windowHeight / 21, 35, "%.1f", m_speed);
			m_pFtFont->Render(m_windowWidth / 1.3f, m_windowHeight / 20, 25, "kmh");

			text = "Press 'I' for instructions";
			ctext = text.c_str();
			offset = m_pFtFont->GetTextWidth(text, 25) / 2;
			m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 25, 25, ctext);			
		}
		// Else Render death screen
		else
		{
			text = "You Died!";
			ctext = text.c_str();
			offset = m_pFtFont->GetTextWidth(text, 80) / 2;
			m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight / 2, 80, ctext);

			text = "Your score was " + std::to_string(m_score);
			ctext = text.c_str();
			offset = m_pFtFont->GetTextWidth(text, 50) / 2;
			m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight / 2 - 150, 50, ctext);

			text = "Press 'R' to restart";
			ctext = text.c_str();
			offset = m_pFtFont->GetTextWidth(text, 50) / 2;
			m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight / 2 - 250, 50, ctext);
		}

		// Render timer before game starts
		if (!m_gameStart)
		{
			if (m_gameStartTimer <= 1.0f)
				text = "1";
			else if (m_gameStartTimer <= 2.0f)
				text = "2";
			else
				text = "3";

			ctext = text.c_str();
			offset = m_pFtFont->GetTextWidth(text, 80) / 2;
			m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight / 2, 80, ctext);
		}
	}
	// Render instructions
	else
	{
		text = "Controls";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 60) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 100, 60, ctext);

		text = "Press 'Q' to change camera view";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 35) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 200, 35, ctext);

		text = "Press 'E' to pause the game";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 35) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 250, 35, ctext);

		text = "Press 'R' to restart the game";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 35) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 300, 35, ctext);

		text = "Use 'A' and 'D' to move left and right";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 35) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 350, 35, ctext);

		text = "Press 'F' to toggle the fog";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 35) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 400, 35, ctext);

		text = "Press '1' and '2' to decrease and increase the volume";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 35) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 450, 35, ctext);


		text = "How to play";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 60) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 550, 60, ctext);

		text = "Collect the diamonds and stars to gain points, while avoiding the cubes.";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 40) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 650, 40, ctext);

		text = "Collecting a star will make you immune for a short period of time.";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 40) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 700, 40, ctext);

		text = "Completing a lap will give you points but will also make you faster";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 40) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 750, 40, ctext);

		text = "and will spawn new diamonds, stars and cubes.";
		ctext = text.c_str();
		offset = m_pFtFont->GetTextWidth(text, 40) / 2;
		m_pFtFont->Render(m_windowWidth / 2 - offset, m_windowHeight - 800, 40, ctext);
	}

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());
}

// Render method runs repeatedly in a loop
void Game::RenderScene(int pass)
{
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("sampler1", 1);
	
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	
	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	
	// Set directional light and material in main shader program
	glm::vec4 directionalLightPosition = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("directionalLight.position", modelViewMatrixStack.Top() * directionalLightPosition); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("directionalLight.La", glm::vec3(0.3f));	// Ambient colour of light
	pMainProgram->SetUniform("directionalLight.Ld", glm::vec3(0.8f));	// Diffuse colour of light
	pMainProgram->SetUniform("directionalLight.Ls", glm::vec3(0.3f));	// Specular colour of light

	pMainProgram->SetUniform("material.Ma", glm::vec3(0.5f));	// Ambient colour of material
	pMainProgram->SetUniform("material.Md", glm::vec3(1.0f));	// Diffuse colour of material
	pMainProgram->SetUniform("material.Ms", glm::vec3(1.0f));	// Specular colour of material
	pMainProgram->SetUniform("material.shininess", 128.0f);		// Shininess of material

	// Set fog
	pMainProgram->SetUniform("showFog", m_showFog);

	if (pass == 0)
	{
		pMainProgram->SetUniform("pass", 0);

		// Render the skybox
		pMainProgram->SetUniform("renderSkybox", true);

		modelViewMatrixStack.Push();
			// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
			glm::vec3 vEye = m_pCamera->GetPosition();
			modelViewMatrixStack.Translate(vEye);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pSkybox->Render(cubeMapTextureUnit);
		modelViewMatrixStack.Pop();

		pMainProgram->SetUniform("renderSkybox", false);

		// Render the start line 
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(m_pTrack->m_centrelinePoints[1]);
			modelViewMatrixStack.Scale(12.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pStartLine->Render();
		modelViewMatrixStack.Pop();

		// Use asteroid positions to render asteroids
		// Give asteroids random rotation and scale
		int index = 0;

		pMainProgram->SetUniform("isAsteroid", true);

		for (glm::vec3 position : m_asteroidPositions)
		{
			modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(position);
				modelViewMatrixStack.Rotate(m_asteroidRotationAxes[index], m_asteroidsRotation);
				modelViewMatrixStack.Scale(m_asteroidScales[index]);
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_pAsteroid->Render();
			modelViewMatrixStack.Pop();

			index++;
		}

		pMainProgram->SetUniform("isAsteroid", false);

		// Render the spaceship if the player is alive
		if (m_health > 0)
		{
			pMainProgram->SetUniform("t", m_currentDistance);
			pMainProgram->SetUniform("resolution", glm::vec2(30, 30));
			pMainProgram->SetUniform("isShip", true);
			if (m_isImmune) pMainProgram->SetUniform("isImmune", true);

			modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(m_spaceshipPosition);
				modelViewMatrixStack *= m_spaceshipOrientation;
				modelViewMatrixStack.Scale(1.5f);
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_pSpaceship->Render();
			modelViewMatrixStack.Pop();

			pMainProgram->SetUniform("isImmune", false);
			pMainProgram->SetUniform("isShip", false);
		}

		int lightCount = 0;

		// Use diamond positions to render diamonds
		for (glm::vec3 position : m_diamondPositions)
		{
			modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(position);
				modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_pickupsRotation);
				modelViewMatrixStack.Scale(4.0f);
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_pDiamond->Render();
			modelViewMatrixStack.Pop();

			// Set up spotlights
			string light = "spotlights[" + std::to_string(lightCount);
			glm::vec4 lightPosition = glm::vec4(position.x, position.y + 30, position.z, 1);
			pMainProgram->SetUniform(light + "].position", modelViewMatrixStack.Top() * lightPosition);
			pMainProgram->SetUniform(light + "].La", glm::vec3(0.0f));
			pMainProgram->SetUniform(light + "].Ld", glm::vec3(0.0f, 1.0f, 0.0f));
			pMainProgram->SetUniform(light + "].Ls", glm::vec3(0.0f, 1.0f, 0.0f));
			pMainProgram->SetUniform(light + "].direction", glm::vec3(0, -1, 0));
			pMainProgram->SetUniform(light + "].exponent", 20.0f);
			pMainProgram->SetUniform(light + "].cutoff", 30.0f);

			lightCount++;
		}

		// Use star positions to render star
		for (glm::vec3 position : m_starPositions)
		{
			modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(position);
				modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_pickupsRotation);
				modelViewMatrixStack.Scale(1.5f);
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_pStar->Render();
			modelViewMatrixStack.Pop();

			// Set up spotlights
			string light = "spotlights[" + std::to_string(lightCount);
			glm::vec4 lightPosition = glm::vec4(position.x, position.y + 30, position.z, 1);
			pMainProgram->SetUniform(light + "].position", modelViewMatrixStack.Top() * lightPosition);
			pMainProgram->SetUniform(light + "].La", glm::vec3(0.0f));
			pMainProgram->SetUniform(light + "].Ld", glm::vec3(1.0f, 0.85f, 0.0f));
			pMainProgram->SetUniform(light + "].Ls", glm::vec3(1.0f, 0.85f, 0.0f));
			pMainProgram->SetUniform(light + "].direction", glm::vec3(0, -1, 0));
			pMainProgram->SetUniform(light + "].exponent", 40.0f);
			pMainProgram->SetUniform(light + "].cutoff", 50.0f);

			lightCount++;
		}

		// Use block positions to render block
		for (glm::vec3 position : m_blockPositions)
		{
			modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(position);
				modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_pickupsRotation);
				modelViewMatrixStack.Scale(1.5f);
				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				m_pBlock->Render();
			modelViewMatrixStack.Pop();

			// Set up spotlights
			string light = "spotlights[" + std::to_string(lightCount);
			glm::vec4 lightPosition = glm::vec4(position.x, position.y + 30, position.z, 1);
			pMainProgram->SetUniform(light + "].position", modelViewMatrixStack.Top()* lightPosition);
			pMainProgram->SetUniform(light + "].La", glm::vec3(0.0f));
			pMainProgram->SetUniform(light + "].Ld", glm::vec3(1.0f, 0.0f, 0.0f));
			pMainProgram->SetUniform(light + "].Ls", glm::vec3(1.0f, 0.0f, 0.0f));
			pMainProgram->SetUniform(light + "].direction", glm::vec3(0, -1, 0));
			pMainProgram->SetUniform(light + "].exponent", 40.0f);
			pMainProgram->SetUniform(light + "].cutoff", 50.0f);

			lightCount++;
		}

		pMainProgram->SetUniform("spotlightsAmount", lightCount);

		// Render the track
		pMainProgram->SetUniform("isTrack", true);

		modelViewMatrixStack.Push();
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pTrack->RenderTrack();
		modelViewMatrixStack.Pop();

		pMainProgram->SetUniform("isTrack", false);
	}

	if (pass == 1)
	{
		// Render fullscreen quad with blur effect
		pMainProgram->SetUniform("renderScreenQuad", true);
		pMainProgram->SetUniform("pass", 1);
		pMainProgram->SetUniform("width", m_windowWidth);
		pMainProgram->SetUniform("height", m_windowHeight);
		m_pFBO->BindTexture(0);
		m_pQuad->Render(false);
		pMainProgram->SetUniform("renderScreenQuad", false);
	}
}

// Update method runs repeatedly with the Render method
void Game::Update()
{
	if (m_health > 0 && !m_showInstructions)
	{
		// Store current ship position to calculate speed
		glm::vec3 startPosition = m_spaceshipPosition;

		// If player is immune start timer
		if (m_isImmune)
		{
			m_immunityTimer -= (float)m_dt/1000.0f;

			// When timer reaches 0 set isImmune to false
			if (m_immunityTimer < 0.0f)
				m_isImmune = false;
		}

		// Update the rotation of pickups and asteroids
		m_pickupsRotation += m_dt * 0.003f;
		m_asteroidsRotation += m_dt * 0.0003f;

		// Store the current lap number
		int currentLap = m_pTrack->CurrentLap(m_currentDistance);

		// Stop ship from moving if game is paused
		if (!m_isPaused)
		{
			// Update the distance the player has moved if the game start timer has reached 0
			if (m_gameStartTimer < 0.0f)
				m_currentDistance += m_dt * m_playerForwardSpeed;
		}

		// Compare the current lap number to the one stored previously to check if it increased
		// If lap number increased generate new pickups, increase player speed and add 100 to score
		if (m_pTrack->CurrentLap(m_currentDistance) > currentLap)
		{
			AssignPickupPositions();
			m_distance = 0.0f;
			m_time = 0.0f;
			m_playerForwardSpeed += 0.05f;
			m_playerStrafeSpeed += 0.015f;
			m_score += 100;
		}

		// Calculate TNB frame
		glm::vec3 y = glm::vec3(0, 1, 0);
		glm::vec3 p, pNext;

		m_pTrack->Sample(m_currentDistance, p);
		m_pTrack->Sample(m_currentDistance + 1.0f, pNext);

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, y));
		glm::vec3 B = glm::normalize(glm::cross(N, T));
		
		switch (m_cameraMode)
		{
			// Free look view
			case 0:
				m_pCamera->Update(m_dt);
				break;
			// Top view
			case 1:
				//m_pCamera->Set(glm::vec3(0, 2500, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
				m_pCamera->Set(p + B * 140.0f + T * 55.0f, p + T * 55.0f, T);
				break;
			// Third person view
			case 2:
				m_pCamera->Set(p + B * 25.0f - T * 25.0f, p + 20.0f * T, glm::vec3(0, 1, 0));
				break;
		}

		if (!m_isPaused)
		{
			// Update the strafe value to match the strafe target
			if (m_spaceshipStrafeValue > m_spaceshipStrafeTarget)
				m_spaceshipStrafeValue -= m_dt * m_playerStrafeSpeed;

			if (m_spaceshipStrafeValue < m_spaceshipStrafeTarget)
				m_spaceshipStrafeValue += m_dt * m_playerStrafeSpeed;

			// Update spaceship's position and rotation
			m_spaceshipPosition = p + m_spaceshipStrafeValue * N;
			m_spaceshipOrientation = glm::mat4(glm::mat3(T, B, N));
		}

		// Calculate collisions between spaceships and diamond pickups
		// If spaceship collides, remove pickup, play event sound and add 10 to score
		auto it = m_diamondPositions.begin();
		while (it != m_diamondPositions.end())
		{
			if (glm::length(m_spaceshipPosition - *it) < 10.0f)
			{
				it = m_diamondPositions.erase(it);
				m_pAudio->PlayEventSound(0);
				m_score += 10;
			}
			else
				it++;
		}

		// Calculate collisions between spaceships and star pickups
		// If spaceship collides, remove pickup, play event sound, add 15 to score, set 
		// isImmune to true and set the timer to the immunity time
		it = m_starPositions.begin();
		while (it != m_starPositions.end())
		{
			if (glm::length(m_spaceshipPosition - *it) < 10.0f)
			{
				it = m_starPositions.erase(it);
				m_pAudio->PlayEventSound(1);
				m_score += 15;
				m_isImmune = true;
				m_immunityTimer = m_immunityTime;
			}
			else
				it++;
		}

		// Calculate collisions between spaceships and block pickups
		// If spaceship collides, remove pickup, remove 20 from health and play event
		// sound. On death play different sound
		it = m_blockPositions.begin();
		while (it != m_blockPositions.end())
		{
			if (glm::length(m_spaceshipPosition - *it) < 10.0f)
			{
				it = m_blockPositions.erase(it);
				if (!m_isImmune)
					m_health -= 20;
				if (m_health > 0)
					m_pAudio->PlayEventSound(2);
				else
					m_pAudio->PlayEventSound(3);
			}
			else
				it++;
		}

		// If the game hasn't started
		if (!m_gameStart)
		{
			// Set the start timer to 3 seconds
			if (m_gameStartTimer < 0.0f)
				m_gameStartTimer = 3.0f;

			// Start the timer
			m_gameStartTimer -= (float)m_dt / 1000.0f;

			// When timer reaches 0 set gameStart to true
			if (m_gameStartTimer < 0.0f)
				m_gameStart = true;
		}
		
		// Increase time if the game has started and isn't paused
		if (m_gameStart && !m_isPaused)
			m_time += (float)m_dt / 1000.0f;;

		// Calculate speed
		glm::vec3 endPosition = m_spaceshipPosition;
		float distance = glm::length(endPosition - startPosition);
		if (m_gameStart) m_distance += distance;
		m_speed = (m_gameStart) ? (std::round(m_distance / m_time) * 3600) / 5000 : 0.0f;
	}

	//Update the audio component
	m_pAudio->Update();
}

void Game::DisplayFrameRate()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int m_windowHeight = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(m_windowWidth - 150, m_windowHeight - 25, 20, "FPS: %d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
}

WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			// Change camera mode when 'Q' is pressed
			case 'Q':
				m_cameraMode = (m_cameraMode + 1) % 3;
				break;
			// Toggle pause when 'E' is pressed
			case 'E':
				m_isPaused = !m_isPaused;
				break;
				// Restart the game when 'R' is pressed
			case 'R':
				RestartGame();
				break;
			// Toggle fog when 'F' is pressed
			case 'F':
				m_showFog = !m_showFog;
				break;
			// Toggle instructions when 'I' is pressed
			case 'I':
				if (m_health > 0)
					m_showInstructions = !m_showInstructions;
				break;
			// Move spaceship to the left when 'A' is pressed
			case 'A':
				m_spaceshipStrafeTarget = -25.0f;
				break;
			// Move spaceship to the right when 'D' is pressed
			case 'D':
				m_spaceshipStrafeTarget = 25.0f;
				break;
			case '1':
				m_volume = std::max((m_volume - 0.1f), 0.0f);
				m_pAudio->SetEventVolume(m_volume);
				m_pAudio->SetMusicVolume(m_volume);
				break;
			case '2':
				m_volume = std::min((m_volume + 0.1f), 1.0f);
				m_pAudio->SetEventVolume(m_volume);
				m_pAudio->SetMusicVolume(m_volume);
				break;
			case 'X':
				m_isImmune = true;
				m_immunityTimer = m_immunityTime;
				break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	// When the 'A' and 'D' keys stops being pressed keep current strafe value
	if (!(GetKeyState('A') & 0x80) && !(GetKeyState('D') & 0x80))
		m_spaceshipStrafeTarget = m_spaceshipStrafeValue;

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}

// Check if a given position is within a given range of any positions in given vector
bool IsInRange(glm::vec3 position, vector<glm::vec3> points, float range)
{
	// Loops through all the points in given vector
	for (auto point : points)
	{
		// If the given position's distance to the points is less than the given range return true
		if (glm::length(point - position) < range)
			return true;
	}

	// If the position is not in the given range of any of the points return false
	return false;
}

// Return a given amount of positions that dont intersect with the track or each other
vector<glm::vec3> Game::GetRandomPositionsInSpace(int amount)
{
	vector<glm::vec3> randomPositions;

	// Loop for the amount of positions to create
	for (int i = 0; i < amount; i++)
	{
		// Create a random position until it's not close to the track and to the previously created points
		glm::vec3 randomPosition;
		do {
			 randomPosition = glm::vec3(rand() % 12000 - 6000, rand() % 12000 - 6000, rand() % 12000 - 6000);
		} while (IsInRange(randomPosition, m_pTrack->m_centrelinePoints, 200.0f) || IsInRange(randomPosition, randomPositions, 300.0f));

		// Add position to vector
		randomPositions.push_back(randomPosition);
	}

	return randomPositions;
}

// Return a given amount of positions that are on the track
vector<glm::vec3> Game::GetRandomTrackPositions(int amount)
{
	vector<glm::vec3> possiblePositions, randomPositions;

	// Get the track's centreline points
	vector<glm::vec3> trackPoints(m_pTrack->m_centrelinePoints);

	// Remove 2/3 of the original points so final points aren't too crowded 
	auto it = trackPoints.begin();
	while (it != trackPoints.end() && ++it != trackPoints.end())
	{
		it = trackPoints.erase(it);
		it = trackPoints.erase(it);
	}

	// Use TNB frame to calculate points to the left and right of the original points
	// Then add all the points to another vector
	for (auto i = 0; i < trackPoints.size(); i++)
	{
		glm::vec3 y = glm::vec3(0, 1, 0);

		glm::vec3 p = trackPoints[i] + 5.0f * y;
		glm::vec3 pNext = trackPoints[(i + 1) % trackPoints.size()];
		
		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, y));

		glm::vec3 l = p - 25.0f * N;
		glm::vec3 r = p + 25.0f * N;

		possiblePositions.push_back(p);
		possiblePositions.push_back(l);
		possiblePositions.push_back(r);
	}

	// Calculate points that are uniformly spread within the track
	// Then add some randomness and add them to the vector
	int spread = possiblePositions.size() / amount;
	int index = 9; // Index starts at 9 to make sure points at the very start of the track aren't picked
	for (int i = 0; i < amount; i++)
	{
		index = (index + spread + (rand() % 10 - 5)) % possiblePositions.size();
		randomPositions.push_back(possiblePositions[index]);
	}

	// Shuffle the vector so the points aren't in order
	std::random_shuffle(randomPositions.begin(), randomPositions.end());

	return randomPositions;
}

// Use GetRandomTrackPositions() to assign the positions to the different pickups
void Game::AssignPickupPositions()
{
	// Clear the vectors
	m_diamondPositions.clear();
	m_starPositions.clear();
	m_blockPositions.clear();

	vector<glm::vec3> randomTrackPositions = GetRandomTrackPositions(40);

	auto it = randomTrackPositions.begin();

	// Use 1/3 of random track positions to assign diamond positions
	while (it != randomTrackPositions.begin() + randomTrackPositions.size() / 3)
	{
		m_diamondPositions.push_back(*it);
		it++;
	}

	// Use 1/5 of random track positions to assign star positions
	while (it != randomTrackPositions.begin() + (randomTrackPositions.size() / 5) * 2)
	{
		m_starPositions.push_back(*it);
		it++;
	}

	// Use rest of random track positions to assign block positions
	while (it != randomTrackPositions.end())
	{
		m_blockPositions.push_back(*it);
		it++;
	}
}

// Reset variables
void Game::RestartGame()
{
	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;

	m_cameraMode = 2;

	m_gameStart = false;
	m_gameStartTimer = -1.0f;

	m_isPaused = false;
	m_showInstructions = false;

	m_health = 100;
	m_score = 0;

	m_currentDistance = 0.0f;
	m_playerForwardSpeed = 0.25f;
	m_playerStrafeSpeed = 0.05f;
	m_spaceshipStrafeValue = 0.0f;
	m_spaceshipStrafeTarget = 0.0f;

	m_pickupsRotation = 0.0f;
	m_asteroidsRotation = 0.0f;

	m_isImmune = false;
	m_immunityTime = 5.0f;
	m_immunityTimer = 0.0f;

	m_time = 0.0f;
	m_distance = 0.0f;
	m_speed = 0.0f;

	AssignPickupPositions();

	m_pAudio->StopMusicStream();
	m_pAudio->PlayMusicStream();

	m_pAudio->SetEventVolume(m_volume);
	m_pAudio->SetMusicVolume(m_volume);
}