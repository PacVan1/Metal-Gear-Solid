#include "common.h"
#include "Game.h"

Game::Game() : 
	mSM(this), 
	mGUI(this)
{
	Skybox::Init();
	CM::Init(mFreeCam);
	RC::Init();

	// initialize soldiers
	Soldiers::Singleton();

	GS.SetGame(*this);
	GS.SetFreeze(false);

	// place lights in the correct position
	for (u8 i = 0; i < 10; i++)
	{
		LIGHTS.SetSpotPosition(vec3(-100.0f, 400.0f, 0.0f), i);
		LIGHTS.SetSpotDirection(vec3(0.0f, -1.0f, 0.0f), i);
		LIGHTS.SetSpotDiffuse(vec3(600.0f, 1000.0f, 1000.0f), i);
		LIGHTS.SetSpotSpecular(vec3(1900.0f, 2500.0f, 2500.0f), i);
	}

	LIGHTS.SetSpotPosition(vec3(-0074.950f, 416.75f,-105.200), 0);
	LIGHTS.SetSpotPosition(vec3(-0257.100f, 416.75f, 259.000), 1);
	LIGHTS.SetSpotPosition(vec3(-0257.100f, 416.75f,-105.200), 2);
	LIGHTS.SetSpotPosition(vec3(-0257.100f, 416.75f,-368.400), 3);
	LIGHTS.SetSpotPosition(vec3(-0592.400f, 416.75f, 259.000), 4);
	LIGHTS.SetSpotPosition(vec3(-0592.400f, 416.75f,-105.200), 5);
	LIGHTS.SetSpotPosition(vec3(-0592.400f, 416.75f,-368.400), 6);
	LIGHTS.SetSpotPosition(vec3(-1014.350f, 416.75f, 259.000), 7);
	LIGHTS.SetSpotPosition(vec3(-1014.350f, 416.75f,-105.200), 8);
	LIGHTS.SetSpotPosition(vec3(-1014.350f, 416.75f,-368.400), 9);

	mInventory	= CreateSGNode<Inventory>();
	mPlayer		= CreateSGNode<Player>(mInventory);
	SM().AddActor(CreateSGNode<Terrain>("MGS_Brian/MGS_Brian.obj"));
	SM().AddActor(mPlayer);
	mFog		= CreateSGNode<FogNode>();

	// starting position
	mPlayer->SetLocalPosition(vec3(75.0f, 200.0f, -320.0f));

	mSkybox = &RM().GetSkybox("night_sky1/");

	//CM::SetActive(mGameOverCam);
	//mGameOverCam.mLookAt = vec3(0.0f);

	//mSM.SwitchState(mOpeningState);

	SM().AddChild(mFog);
}

Game::~Game()
{
	CM::Cleanup();
	Skybox::Cleanup();
}

void Game::Tick(float const dt)
{
	//mSM.Tick(dt);

	LIGHTS.RandomlyFlickerAllSpotlights(dt);
	LIGHTS.RedAlarmAllCeilingSpotlights(dt);

	mSkybox->Render();
	
	//mSM.Render();
	RayCaster::Render(); 
	
	SM().Tick(dt);
	CameraManager::Tick(dt);
	SM().Render();

	mInventory->Tick(dt);
	mInventory->Render();

	GUI2::Singleton().Render();
}

void Game::RenderGUI()
{
	mSM.RenderGUI();
}

void Game::Restart()
{
	mState = GAME_STATES_PLAYING;
}

void Game::GameOver()
{
	mSM.SwitchState(mOverState);
}

static ImGuiWindowFlags constexpr GAME_OPENING_WINDOW_FLAGS =
ImGuiWindowFlags_NoTitleBar |
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoBackground |
ImGuiWindowFlags_NoScrollbar |
ImGuiWindowFlags_NoScrollWithMouse |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_AlwaysAutoResize |
ImGuiWindowFlags_NoSavedSettings |
ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoBringToFrontOnFocus;

static ImGuiWindowFlags constexpr HEALTHBAR_WINDOW_FLAGS =
ImGuiWindowFlags_NoTitleBar |
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoScrollbar |
ImGuiWindowFlags_NoScrollWithMouse |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_AlwaysAutoResize |
ImGuiWindowFlags_NoSavedSettings |
ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoBringToFrontOnFocus;

static ImGuiWindowFlags constexpr GAME_INVENTORY_WINDOW_FLAGS =
ImGuiWindowFlags_NoTitleBar |
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoScrollbar |
ImGuiWindowFlags_NoScrollWithMouse |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_NoSavedSettings |
ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoBringToFrontOnFocus;

void GameOpeningState::Enter(Game&)
{
	GetWindow().EnableCursor();
}

void GameOpeningState::Exit(Game&)
{
	GetWindow().DisableCursor();
}

void GameOpeningState::RenderGUI(Game& g)
{
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2.0f - 100.0f, WINDOW_HEIGHT - 300.0f));
	ImGui::Begin("Main", nullptr, GAME_OPENING_WINDOW_FLAGS);
	if (ImGui::Button("Play", ImVec2(200, 60))) g.mSM.SwitchState(g.mPlayingState);
	if (ImGui::Button("Quit", ImVec2(200, 60))) Engine::Quit();
	ImGui::End();
}

static void Healthbar(IDamageable const& dmgable)
{
	ImGui::SetNextWindowPos(ImVec2(10, WINDOW_HEIGHT - 60));
	ImGui::Begin("Healthbar", nullptr, HEALTHBAR_WINDOW_FLAGS);
	float health = dmgable.GetHealth() / dmgable.GetMaxHealth();
	ImGui::Text("Health");
	ImGui::ProgressBar(health, ImVec2(200, 20));
	ImGui::End();
}

void GamePlayingState::Enter(Game& g)
{
	CameraManager::Transition(g.mPlayer->mOrbitCam);
}

void GamePlayingState::Tick(Game& g, float const dt)
{
	if (Inputs::ToggleMenu()) g.mSM.SwitchState(g.mMenuState);

	SceneManager::Tick(dt);
}

void GamePlayingState::RenderGUI(Game& g)
{
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH - 236.0f, WINDOW_HEIGHT - 84.0f));
	ImGui::Begin("Inventory", nullptr, GAME_OPENING_WINDOW_FLAGS);
	if (ImGui::BeginTable("Inventory", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_NoClip)) {
		ImGui::TableNextRow();

		for (int col = 0; col < 3; ++col) {
			ImGui::TableSetColumnIndex(col);
		}
		ImGui::EndTable();
	}
	ImGui::End();
	Healthbar(*g.mPlayer.get());
}

void GameMenuState::Enter(Game&)
{
	GetWindow().EnableCursor();
}

void GameMenuState::Exit(Game&)
{
	GetWindow().DisableCursor();
}

void GameMenuState::Tick(Game& g, float const)
{
	if (Inputs::ToggleMenu()) g.mSM.SwitchState(g.mPlayingState);
}

void GameMenuState::RenderGUI(Game&)
{
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2.0f - 250.0f, WINDOW_HEIGHT / 2.0f - 250.0f));
	ImGui::SetNextWindowSize(ImVec2(500, 500));
	ImGui::Begin("Menu", nullptr, GAME_INVENTORY_WINDOW_FLAGS);
	if (ImGui::BeginTabBar("Root"))
	{
		if (ImGui::BeginTabItem("Inventory"))	ImGui::EndTabItem();
		if (ImGui::BeginTabItem("Map"))			ImGui::EndTabItem();
		if (ImGui::BeginTabItem("Credits"))		ImGui::EndTabItem();

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void GameOverState::Enter(Game& g)
{
	g.mGameOverCam.mLookAt = g.mPlayer->GetWorldPosition();
	CM::Transition(g.mGameOverCam);
}
