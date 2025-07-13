#include "common.h"
#include "gui.h"

#include "Game.h"

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

//static void Image(OGLTexture2D const& texture)
//{
//	ImGui::Image((ImTextureID)(intptr_t)texture.ID, ImVec2(static_cast<float>(texture.width), static_cast<float>(texture.height)));
//}

static void Healthbar(IDamageable const& dmgable)
{
	ImGui::SetNextWindowPos(ImVec2(10, WINDOW_HEIGHT - 60));
	ImGui::Begin("Healthbar", nullptr, HEALTHBAR_WINDOW_FLAGS);
	float health = dmgable.GetHealth() / dmgable.GetMaxHealth();
	ImGui::Text("Health");
	ImGui::ProgressBar(health, ImVec2(200, 20));
	ImGui::End();
}

GUI::GUI() :
	mG(nullptr)
{}

GUI::GUI(Game* game) :
	mG(game)
{
	Init();
}

void GUI::Tick()
{
	if (!mEnabled) return; 

	switch (mG->mState)
	{
	case GAME_STATES_OPENING:	MainState();		break;
	case GAME_STATES_PLAYING:	HUDState();			break;
	case GAME_STATES_MENU:		MenuState();		break;
	case GAME_STATES_GAME_OVER: GameOverState();	break;
	}
}

void GUI::Init()
{
	//mData.invIcons.push_back(OGLTexture2D("imgui_image_test.png"));
}

void GUI::MainState()
{
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2.0f - 100.0f, WINDOW_HEIGHT - 300.0f));
	ImGui::Begin("Main", nullptr, GAME_OPENING_WINDOW_FLAGS);
	if (ImGui::Button("Play", ImVec2(200, 60))) mG->mState = GAME_STATES_PLAYING;
	if (ImGui::Button("Quit", ImVec2(200, 60))) Engine::Quit();
	ImGui::End();
}

void GUI::HUDState() const
{
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH - 236.0f, WINDOW_HEIGHT - 84.0f));
	ImGui::Begin("Inventory", nullptr, GAME_OPENING_WINDOW_FLAGS);
	if (ImGui::BeginTable("Inventory", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_NoClip)) {
		ImGui::TableNextRow();

		for (int col = 0; col < 3; ++col) {
			ImGui::TableSetColumnIndex(col);
			//Image(mData.invIcons[0]);
		}
		ImGui::EndTable();
	}
	ImGui::End();
	Healthbar(*mG->mPlayer);
}

void GUI::MenuState()
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

void GUI::GameOverState()
{
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2.0f - 100.0f, WINDOW_HEIGHT - 300.0f));
	ImGui::Begin("Main", nullptr, GAME_OPENING_WINDOW_FLAGS);
	if (ImGui::Button("Restart", ImVec2(200, 60))) mG->Restart();
	if (ImGui::Button("Quit", ImVec2(200, 60))) Engine::Quit();
	ImGui::End();
}

//void Game::DebugMenu()
//{
//	ImGui::Begin("Debug Info");
//	ImGui::Text("Game State: %d", state);
//	ImGui::Text("Is Any Item Hovered: %d", ImGui::IsAnyItemHovered());
//	ImGui::End();
//}
