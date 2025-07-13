#include "common.h"
#include "inventory.h"

Item::Item(shared_ptr<StaticModelNode> model) :
	mDisplay(model)
{}

void Item::Init()
{
	AddChild(mDisplay);
}

Inventory::Inventory()
{
	mItems.push_back(CreateSGNode<RCFirearm>(CreateSGNode<StaticModelNode>("Vepr_Hunter3/vepr_hunter.dae"), 1300.0f, 600.0f, 600.0f));
	mItemGUIs.push_back(CreateSGNode<ItemGUI>("Vepr_Hunter3/vepr_hunter.dae"));

	mItemGUIs.back()->SetLocalPosition(NDCToWorld(vec3(0.6f, -0.7f, -12.0f)));

	Equip(0);
}

void Inventory::Init()
{
	for (auto& item : mItemGUIs)
		AddChild(item);
}

void Inventory::Tick(float const dt)
{
	if (GS.IsFreezed()) return;
	SGNode::Tick(dt);
	RefreshTransforms();
}

void Inventory::Render() const
{
	if (!mIsVisible) return;

	// to make sure ui elements are on top of 
	// the game world while occluding one another
	glClear(GL_DEPTH_BUFFER_BIT);
	if (mEquipedGUI) mEquipedGUI->Render();
}

void Inventory::Equip(u8 const type)
{
	Unequip();
	mEquiped = mItems[type] ? mItems[type] : nullptr;
	if (mEquiped)
	{
		mEquipedGUI = mItemGUIs[type];
		mEquiped->Equip();
		DebugLog(INFO, (std::string(mEquiped->GetName()) + " got equiped!").c_str());
	}
}

void Inventory::Use() const
{
	if (mEquiped)
	{
		mEquiped->Use();
		DebugLog(INFO, (std::string(mEquiped->GetName()) + " got used!").c_str());
	}
}

void Inventory::Unequip()
{
	if (mEquiped)
	{
		mEquipedGUI = nullptr;
		mEquiped->Unequip();
		DebugLog(INFO, (std::string(mEquiped->GetName()) + " got unequiped!").c_str());
	}
}

void Inventory::HideEquiped() const
{
	if (mEquiped) mEquiped->mIsVisible	= false;
	if (mEquiped) mEquiped->mIsActive	= false;
}

void Inventory::ShowEquiped() const
{
	if (mEquiped) mEquiped->mIsVisible	= true;
	if (mEquiped) mEquiped->mIsActive	= true;
}

ItemGUI::ItemGUI() :
	mDisplay(make_shared<GUIElementNode3D>())
{}

ItemGUI::ItemGUI(char const* name) :
	mDisplay(make_shared<GUIElementNode3D>(name))
{}

void ItemGUI::Init()
{
	AddChild(mDisplay);
}

void ItemGUI::Tick(float const dt)
{
	if (!mIsActive) return;
	RotateLocal(vec3(0.0f, dt, 0.0f)); 
	ChildrenTick(dt);
}

void ItemGUI::Render() const
{
	mDisplay->Render();
}

vec3 NDCToWorld(vec3 const NDC)
{
	float tanHalfFov = tan(45.0f * 0.5f);
	float xView = NDC.x * -NDC.z * tanHalfFov * (4.0f / 3.0f);
	float yView = NDC.y * -NDC.z * tanHalfFov;
	return glm::vec3(xView, yView, NDC.z);
}
