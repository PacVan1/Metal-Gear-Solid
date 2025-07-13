#include "common.h"
#include "sgnode.h"

SGNode::SGNode() :
	mLocalPosition(0.0f),
	mLocalRotation(quat(1.0f, 0.0f, 0.0f, 0.0f)),
	mLocalScale(1.0f),
	mLocalTransform(1.0f),
	mName("[NO NAME]"),
	mIsDirty(true), 
	mIsActive(true),
	mIsVisible(true)
{}

SGNode::SGNode(SGNode const& other) : 
	mLocalPosition(other.mLocalPosition), 
	mLocalRotation(other.mLocalRotation),
	mLocalScale(other.mLocalScale),
	mLocalTransform(other.mLocalTransform),
	mWorldTransform(other.mWorldTransform),
	mName(other.mName),
	mIsDirty(true),
	mIsActive(true), 
	mIsVisible(true)
{}

SGNode::~SGNode()
{
	PrintDestroyed();
	mChildren.clear();
}

void SGNode::SetLocalPosition(vec3 const& position)
{
	mLocalPosition	= position;
	mIsDirty		= true;
}

void SGNode::SetLocalRotation(quat const& rotation)
{
	mLocalRotation	= rotation;
	mIsDirty		= true;
}

void SGNode::SetLocalScale(vec3 const& scale)
{
	mLocalScale = scale;
	mIsDirty	= true;
}

void SGNode::AddToLocalPosition(vec3 const& offset)
{
	mLocalPosition += offset;
}

void SGNode::ResetLocalTransform()
{
	mLocalPosition	= vec3(0.0f);
	mLocalRotation	= quat(1.0f, 0.0f, 0.0f, 0.0f);
	mLocalScale		= vec3(1.0f);
	mIsDirty		= true;
}

void SGNode::AddChild(shared_ptr<SGNode> node)
{
	if (!node) return;
	node->mParent = shared_from_this(); // requires inheritance from std::enable_shared_from_this
	mChildren.push_back(node);
}

void SGNode::RemoveChild(SGNode* node)
{
	// does any child, point to the node
	auto it = std::remove_if(mChildren.begin(), mChildren.end(),
		[&](const std::shared_ptr<SGNode>& ptr) 
		{
			return ptr.get() == node;
		});

	// if a child points to a node, remove it
	if (it != mChildren.end())
	{
		(*it)->mParent.reset();
		mChildren.erase(it, mChildren.end());
	}
}

void SGNode::TranslateLocal(vec3 const& offset)
{
	mLocalPosition += offset;
	mIsDirty		= true;
}

void SGNode::RotateLocal(quat const& offset)
{
	mLocalRotation	= glm::normalize(offset * mLocalRotation);
	mIsDirty		= true;
}

void SGNode::LookAt(vec3 const& target, vec3 const& up)
{
	mat4 const lookMat = glm::lookAt(mLocalPosition, target, up);
	mLocalRotation = glm::quat_cast(glm::inverse(lookMat));
	mIsDirty = true;
}

void SGNode::SetName(string_view const& name)
{
	mName = name;
}

void SGNode::RefreshTransforms()
{
	if (mIsDirty) 
	{
		mLocalTransform =
			glm::translate(glm::mat4(1.0f), mLocalPosition) *
			glm::toMat4(mLocalRotation) *
			glm::scale(glm::mat4(1.0f), mLocalScale);

		mIsDirty = false;
	}

	if (auto parent = mParent.lock()) 
		mWorldTransform = parent->mWorldTransform * mLocalTransform;
	else
		mWorldTransform = mLocalTransform;

	for (auto& child : mChildren)
		child->RefreshTransforms();
}

void SGNode::PrintUseCount() const
{
	DebugLog(INFO, (std::string(mName) + " has a use count of: " + std::to_string(shared_from_this().use_count())).c_str());
}

void SGNode::PrintDestroyed() const
{
	DebugLog(INFO, ("Deloading sgnode: '" + std::string(mName) + "' got destroyed").c_str());
}

quat SGNode::GetWorldRotation() const
{
	vec3 scale, translation, skew;
	vec4 perspective;
	quat rotation;
	glm::decompose(mWorldTransform, scale, rotation, translation, skew, perspective);
	return rotation;
}

vec3 SGNode::GetWorldScale() const
{
	vec3 scale, translation, skew;
	vec4 perspective;
	quat rotation;
	glm::decompose(mWorldTransform, scale, rotation, translation, skew, perspective);
	return scale;
}

void SGNode::Tick(float const dt)
{
	if (mIsActive)
		ChildrenTick(dt);
}

void SGNode::Render() const
{
	if (mIsVisible)
		ChildrenRender();
}

void SGNode::ChildrenTick(float const dt)
{
	for (const auto& child : mChildren)
		child->Tick(dt);
}

void SGNode::ChildrenRender() const
{
	for (const auto& child : mChildren)
		child->Render();
}
