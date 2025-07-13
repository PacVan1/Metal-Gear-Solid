#include "common.h"
#include "editor.h"

#include "Game.h"

//char const* STR_RENDER_MODES[]	= { "Shaded", "Normal" };
auto constexpr STR_RENDER_MODES = "Shaded\0Normal\0";
char const* STR_GAME_STATES[]	= { "Opening", "Playing", "Menu", "Paused" };

Editor::Editor() : 
	mG(nullptr), 
	mShow(true)
{}

Editor::Editor(Game* game) :
	mG(game), 
	mShow(true)
{}

void Editor::Toggle()
{
	if (mShow)
	{
		mShow = false;
		GetWindow().DisableCursor();
	}
	else
	{
		mShow = true;
		GetWindow().EnableCursor();
	}
}

void Editor::Tick() const
{
	if (!mShow) return; 

	ImGui::Begin("Editor");
	ImGui::Text("FPS: %f", GetFPS());
	ImGui::Separator(); 
	if (ImGui::BeginTabBar("Root"))
	{
		if (ImGui::BeginTabItem("Global"))	{ Globals(); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem("Player"))	{ Edit(*mG->mPlayer.get()); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem("Fog"))		{ Edit(*mG->mFog); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem("Lights"))	{ Edit(LIGHTS); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem("Scene"))	{ Edit(SM().GetActive()); ImGui::EndTabItem(); }
		if (ImGui::BeginTabItem("Cam"))		{ Edit(mG->mPlayer->mOrbitCam);	ImGui::EndTabItem(); }
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void Editor::Edit(Camera& c) const
{
	if (ImGui::DragFloat3("Position", glm::value_ptr(c.mPosition), 0.5f))	c.Refresh();
	if (ImGui::DragFloat("Yaw", &c.mYaw, 0.002f))							c.Refresh();
	if (ImGui::DragFloat("Pitch", &c.mPitch, 0.002f))						c.Refresh();
	if (ImGui::DragFloat("Speed", &c.mSpeed, 0.001f))						c.Refresh();
	if (ImGui::DragFloat("Sensitivity", &c.mSens, 0.001f))					c.Refresh();
	if (ImGui::DragFloat("FOV", &c.mFOV, 0.002f))							c.Refresh();
}

void Editor::Edit(OrbitalCamera& c) const
{
	ImGui::DragFloat("Distance", &c.mDist, 0.05f);
	ImGui::Checkbox("Lerp", &c.mLerp);
	ImGui::DragFloat("Lerp speed", &c.mLerpSpeed, 0.002f);
}

void Editor::Edit(Transform& t) const
{
	if (ImGui::Button("Reset"))												  t.Reset(); 
	if (ImGui::DragFloat3("Position",	glm::value_ptr(t.mPosition), 0.002f)) t.RefreshAll();
	if (ImGui::DragFloat3("Rotation",	glm::value_ptr(t.mRotation), 0.002f)) t.RefreshAll();
	if (ImGui::DragFloat3("Scale",		glm::value_ptr(t.mScale), 0.002f))	  t.RefreshAll();
}

void Editor::Edit(FogInstance& f) const
{
	ImGui::DragFloat3("Position", glm::value_ptr(f.mPosition), 0.05f);
	ImGui::ColorEdit3("Color", glm::value_ptr(f.mColor));
	ImGui::DragFloat2("Size", glm::value_ptr(f.mSize), 0.05f);
	ImGui::DragFloat("Rate", &f.mRate, 0.05f);
}

void Editor::Edit(FogNode& f) const
{
	ImGui::DragFloat3("Position", glm::value_ptr(f.mPosition), 0.05f);
	ImGui::ColorEdit3("Color", glm::value_ptr(f.mColor));
	ImGui::DragFloat2("Size", glm::value_ptr(f.mSize), 0.05f);
	ImGui::DragFloat("Rate", &f.mRate, 0.05f);
}

void Editor::Edit(Player& p) const
{
	ImGui::Text("State: %u", p.mBehavior);
	ImGui::DragFloat("Move force", &p.mMoveForce, 0.02f, 0.0f);
	ImGui::DragFloat("Max move speed", &p.mMaxMoveSpeed, 0.02f, 0.0f);
}

void Editor::Edit(Lights& ls) const
{
	if (ImGui::CollapsingHeader("Point lights"))
	{
		for (u8 i = 0; i < POINT_LIGHT_COUNT; i++)
		{
			if (ImGui::TreeNode(("PointLight##" + std::to_string(i)).c_str()))
			{
				EditPointLight(ls, i);
				ImGui::TreePop();
			}
		}
	}
	if (ImGui::CollapsingHeader("Spot lights"))
	{
		for (u8 i = 0; i < SPOTLIGHT_COUNT; i++)
		{
			if (ImGui::TreeNode(("Spotlight##" + std::to_string(i)).c_str()))
			{
				EditSpotlight(ls, i);
				ImGui::TreePop();
			}
		}
	}
}

void Editor::EditPointLight(Lights& ls, u8 const i) const
{
	if (ImGui::DragFloat3("Position", glm::value_ptr(ls.mPointBlock.positions[i]), 0.05f))	ls.UploadPointPosition(i);
	if (ImGui::DragFloat3("Diffuse", glm::value_ptr(ls.mPointBlock.diffuses[i]), 0.05f))	ls.UploadPointDiffuse(i);
	if (ImGui::DragFloat3("Specular", glm::value_ptr(ls.mPointBlock.speculars[i]), 0.05f))	ls.UploadPointSpecular(i);
	bool enabled = (bool)ls.mPointBlock.enabled[i].x;
	if (ImGui::Checkbox("Enabled", &enabled))												ls.SetPointEnabled(enabled, i);
}

void Editor::EditSpotlight(Lights& ls, u8 const i) const
{
	if (ImGui::DragFloat3("Position", glm::value_ptr(ls.mSpotBlock.positions[i]), 0.05f))	ls.UploadSpotPosition(i);
	if (ImGui::DragFloat3("Direction", glm::value_ptr(ls.mSpotBlock.directions[i]), 0.05f))	ls.UploadSpotDirection(i);
	if (ImGui::DragFloat3("Diffuse", glm::value_ptr(ls.mSpotBlock.diffuses[i]), 0.05f))		ls.UploadSpotDiffuse(i);
	if (ImGui::DragFloat3("Specular", glm::value_ptr(ls.mSpotBlock.speculars[i]), 0.05f))	ls.UploadSpotSpecular(i);
	bool enabled = (bool)ls.mSpotBlock.enabled[i].x;
	if (ImGui::Checkbox("Enabled", &enabled))												ls.SetSpotEnabled(enabled, i);
}

void Editor::Edit(shared_ptr<SGNode> const& node) const
{
	if (!node) return;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (node->mChildren.empty()) 
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (ImGui::TreeNodeEx(reinterpret_cast<void*>(node.get()), flags, "%s", node->mName.data()))
	{
		auto ID = std::to_string(reinterpret_cast<uintptr_t>(node.get()));

		vec3 pos = node->GetLocalPosition();
		if (ImGui::DragFloat3(("Position##" + ID).c_str(), glm::value_ptr(pos), 0.1f))
			node->SetLocalPosition(pos);

		quat rot = node->GetLocalRotation();
		vec3 euler = glm::degrees(glm::eulerAngles(rot));
		if (ImGui::DragFloat3(("Rotation##" + ID).c_str(), glm::value_ptr(euler), 1.0f))
		{
			quat newRot = quat(glm::radians(euler));
			node->SetLocalRotation(newRot);
		}
		
		vec3 scale = node->GetLocalScale();
		if (ImGui::DragFloat3(("Scale##" + ID).c_str(), glm::value_ptr(scale), 0.1f))
			node->SetLocalScale(scale);
		
		if (ImGui::Button(("Reset Transform##" + ID).c_str())) node->ResetLocalTransform();
		ImGui::Checkbox(("Visible##" + ID).c_str(), &node->mIsVisible);
		ImGui::Checkbox(("Active##" + ID).c_str(), &node->mIsActive);

		if (!(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			for (auto& child : node->mChildren)
				Edit(child);
			ImGui::TreePop();
		}
	}
}

void Editor::Globals() const
{
	//ImGui::Text("Game state: %s", STR_GAME_STATES[mG->mState]);
	ImGui::Checkbox("Debug lines", &GS.mWireFrame);
	static bool freezed = GS.IsFreezed();
	if (ImGui::Checkbox("Freeze", &freezed)) GS.SetFreeze(freezed);
	static bool playerPossessed = GS.IsPlayerPossessed();
	if (ImGui::Checkbox("Player Possessed", &playerPossessed)) GS.SetPlayerPossessed(playerPossessed);
	ImGui::Checkbox("Fog", &GS.mFog);
	//ImGui::Combo("Render mode", reinterpret_cast<int*>(&ModelDisplay::sMode), STR_RENDER_MODES);
}
