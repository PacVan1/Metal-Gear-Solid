#pragma once

#include "model.h"

class GUIElementNode3D : public ModelNode<StaticModel>
{
public:
	GUIElementNode3D() = default;
	GUIElementNode3D(char const* name);
	virtual void Render() const override;
};