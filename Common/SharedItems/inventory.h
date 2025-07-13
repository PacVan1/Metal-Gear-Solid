#pragma once

class Item : public SGNode
{
protected:
	shared_ptr<StaticModelNode> mDisplay;

public:
	virtual void Init() override;
	virtual void Equip()	{}
	virtual void Use()		{}
	virtual void Unequip()	{}

protected:
	Item(shared_ptr<StaticModelNode> model);
};

class ItemGUI : public SGNode
{
protected:
	shared_ptr<GUIElementNode3D>	mDisplay;

public:
	ItemGUI();
	ItemGUI(char const* name);
	virtual void Init() override;
	virtual void Tick(float const dt) override;
	virtual void Render() const override;
};

enum itemTypes : u8
{
	ITEM_TYPES_HUNTER,
	ITEM_TYPES_COUNT,
};

class Inventory : public SGNode
{
public:
	vector<shared_ptr<Item>>	mItems;
	vector<shared_ptr<ItemGUI>> mItemGUIs;
	shared_ptr<Item>			mEquiped;
	shared_ptr<ItemGUI>			mEquipedGUI;

public:
	Inventory();
	virtual void Init() override;
	virtual void Tick(float const dt) override;
	virtual void Render() const override;
	void Equip(u8 const type);
	void Use() const;
	void Unequip();
	void HideEquiped() const;
	void ShowEquiped() const;
};

vec3 NDCToWorld(vec3 const NDC);
