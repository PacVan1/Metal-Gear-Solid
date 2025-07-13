#include "common.h"
#include "colliders.h"

#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"

#ifdef DEBUG_MODE
ColliderMesh::ColliderMesh(ColliderMesh const& other) : 
	mShape(other.mShape),
	mVAO(other.mVAO), 
	mVBO(other.mVBO), 
	mEBO(other.mEBO), 
	mVertexCount(other.mVertexCount),
	mIndexCount(other.mIndexCount)
{}
#else
ColliderMesh::ColliderMesh(ColliderMesh const& other) :
	mShape(other.mShape)
{}
#endif

ColliderMesh::ColliderMesh(StaticModel const& model)
{
	mTriangleMesh = new btTriangleMesh();

	for (StaticMesh const& mesh : model.meshes)
	{
		for (int i = 0; i < mesh.indices.size() / 3; ++i)
		{
			btVector3 v0(mesh.vertices[i * 3 + 0].position.x, mesh.vertices[i * 3 + 0].position.y, mesh.vertices[i * 3 + 0].position.z);
			btVector3 v1(mesh.vertices[i * 3 + 1].position.x, mesh.vertices[i * 3 + 1].position.y, mesh.vertices[i * 3 + 1].position.z);
			btVector3 v2(mesh.vertices[i * 3 + 2].position.x, mesh.vertices[i * 3 + 2].position.y, mesh.vertices[i * 3 + 2].position.z);
			mTriangleMesh->addTriangle(v0, v1, v2);
		}
	}

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mTriangleMesh, true);

	shape->setMargin(0.04f);

#ifdef DEBUG_MODE
	btStridingMeshInterface* interface = shape->getMeshInterface();
	if (!interface) {
		std::cerr << "Error: Mesh interface is null!" << std::endl;
		return;
	}

	// Retrieve vertex and index data from the mesh interface
	const unsigned char* vertexBase = nullptr;
	int numVertices;
	PHY_ScalarType vertexType;
	int vertexStride;

	const unsigned char* indexBase = nullptr;
	PHY_ScalarType indexType;
	int indexStride;
	int numFaces;

	// Call the function with all required parameters
	interface->getLockedReadOnlyVertexIndexBase(&vertexBase, numVertices, vertexType, vertexStride,
		&indexBase, indexStride, numFaces, indexType, 0);
	interface->unLockReadOnlyVertexBase(0);

	if (!vertexBase || !indexBase) {
		std::cerr << "Error: Invalid vertex or index data!" << std::endl;
		return;
	}


	// Allocate storage for vertices and indices
	std::vector<vec3> vertices(numVertices);

	// Extract vertex data safely
	for (int i = 0; i < numVertices; i++) {
		btVector3 vertex = *((btVector3*)(vertexBase + i * vertexStride));
		vertices[i] = vec3(vertex.x(), vertex.y(), vertex.z());
	}

	u32 triangleCount = numVertices / 3;
	u32 numIndices = triangleCount * 6;

	std::vector<u32> indices = std::vector<u32>(numIndices);
	u32 element = 0;
	u32 index = 0;

	for (u32 triangle = 0; triangle < triangleCount; triangle++)
	{
		indices[index++] = element++;
		indices[index++] = element;
		indices[index++] = element++;
		indices[index++] = element;
		indices[index++] = element++;
		indices[index++] = element - 3;
	}

	mVAO = OGL::VAO();
	mVBO = OGL::VBO();
	mEBO = OGL::EBO();
	mVertexCount = numVertices;
	mIndexCount = numIndices;
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
#endif

	mShape = shape;
}

ColliderMesh::ColliderMesh(btBvhTriangleMeshShape* shape) :
	mShape(shape)
{
	// to remove jitters
	shape->setMargin(0.04f);

#ifdef DEBUG_MODE
	btStridingMeshInterface* interface = shape->getMeshInterface();
	if (!interface) {
		std::cerr << "Error: Mesh interface is null!" << std::endl;
		return;
	}

	// Retrieve vertex and index data from the mesh interface
	const unsigned char* vertexBase = nullptr;
	int numVertices;
	PHY_ScalarType vertexType;
	int vertexStride;

	const unsigned char* indexBase = nullptr;
	PHY_ScalarType indexType;
	int indexStride;
	int numFaces;

	// Call the function with all required parameters
	interface->getLockedReadOnlyVertexIndexBase(&vertexBase, numVertices, vertexType, vertexStride,
		&indexBase, indexStride, numFaces, indexType, 0);
	interface->unLockReadOnlyVertexBase(0);

	if (!vertexBase || !indexBase) {
		std::cerr << "Error: Invalid vertex or index data!" << std::endl;
		return;
	}


	// Allocate storage for vertices and indices
	std::vector<vec3> vertices(numVertices);

	// Extract vertex data safely
	for (int i = 0; i < numVertices; i++) {
		btVector3 vertex = *((btVector3*)(vertexBase + i * vertexStride));
		vertices[i] = vec3(vertex.x(), vertex.y(), vertex.z());
	}

	u32 triangleCount = numVertices / 3;
	u32 numIndices = triangleCount * 6;

	std::vector<u32> indices = std::vector<u32>(numIndices);
	u32 element = 0;
	u32 index = 0;

	for (u32 triangle = 0; triangle < triangleCount; triangle++)
	{
		indices[index++] = element++;
		indices[index++] = element;
		indices[index++] = element++;
		indices[index++] = element;
		indices[index++] = element++;
		indices[index++] = element - 3;
	}

	mVAO = OGL::VAO();
	mVBO = OGL::VBO();
	mEBO = OGL::EBO();
	mVertexCount = numVertices;
	mIndexCount = numIndices;
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
#endif
}

ColliderMesh::ColliderMesh(btCapsuleShape* shape) :
	mShape(shape)
{
#ifdef DEBUG_MODE
	float radius = shape->getRadius();
	float halfHeight = shape->getHalfHeight();

	static const vec3 VERTICES[8] =
	{
		{ -1.f,-1.f,-1.f },
		{  1.f,-1.f,-1.f },
		{  1.f, 1.f,-1.f },
		{ -1.f, 1.f,-1.f },
		{ -1.f,-1.f, 1.f },
		{  1.f,-1.f, 1.f },
		{  1.f, 1.f, 1.f },
		{ -1.f, 1.f, 1.f }
	};
	std::vector<u32> INDICES =
	{
		1, 0, 0, 2, 2, 1,
		2, 0, 0, 3, 3, 2,
		4, 5, 5, 6, 6, 4,
		6, 7, 7, 4, 4, 6,
		7, 3, 3, 0, 0, 7,
		0, 4, 4, 7, 7, 0,
		6, 1, 1, 2, 2, 6,
		1, 6, 6, 5, 5, 1,
		0, 1, 1, 5, 5, 0,
		5, 4, 4, 0, 0, 5,
		3, 6, 6, 2, 2, 3,
		6, 3, 3, 7, 7, 6
	};

	std::vector<vec3> vertices(8);
	for (s32 i = 0; i < 8; ++i)
	{
		vertices[i].x = VERTICES[i].x * radius;
		vertices[i].y = VERTICES[i].y * (halfHeight + radius);
		vertices[i].z = VERTICES[i].z * radius;
	}

	mVAO = OGL::VAO();
	mVBO = OGL::VBO();
	mEBO = OGL::EBO();
	mIndexCount = 72;
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 72 * sizeof(u32), INDICES.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
#endif
}

ColliderMesh::~ColliderMesh()
{
#ifdef DEBUG_MODE
	glDeleteBuffers(1, &mEBO);
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
#endif

	delete mShape;
	if (mTriangleMesh) delete mTriangleMesh;
}

void ColliderMesh::WireFrame(Transform const& transform, vec3 const& color) const
{
#ifdef DEBUG_MODE
	static std::string DIR			= "../Common/Assets/Shaders/";
	static u32 const PROGRAM		= Shader::LoadAndBind("wireframe", { BINDING_CAM });
	static s32 const UNIFORM_MVP	= Shader::GetLoc("model");
	static s32 const UNIFORM_COLOR	= Shader::GetLoc("color");

	Shader::Bind(PROGRAM);
	Shader::SetMat4("model", transform.GetWorld());
	Shader::SetVec3("color", color);
	glLineWidth(3.0f);
	glBindVertexArray(mVAO);
	glDrawElements(GL_LINES, mIndexCount, GL_UNSIGNED_INT, 0);
#endif
}

void ColliderMesh::WireFrame(mat4 const& transform, vec3 const& color) const
{
#ifdef DEBUG_MODE
	static u32 const PROGRAM = Shader::LoadAndBind("wireframe", { BINDING_CAM });
	static s32 const U_MODEL = Shader::GetLoc("model");
	static s32 const U_COLOR = Shader::GetLoc("color");

	Shader::Bind(PROGRAM);
	Shader::SetMat4(U_MODEL, transform);
	Shader::SetVec3(U_COLOR, color);
	glLineWidth(3.0f);
	glBindVertexArray(mVAO);
	glDrawElements(GL_LINES, mIndexCount, GL_UNSIGNED_INT, 0);
#endif
}

void Collider::WireFrame() const
{
	if (mMesh) mMesh->WireFrame(mMotionState->GetTransform(), mDebugColor);
}

void Collider::MakeChild(Transform& transform)
{
	mMotionState->MakeChild(transform);
}

void Collider::RemoveChild(Transform& transform)
{
	mMotionState->RemoveChild(transform);
}

Collider::Collider(ColliderMesh const* mesh) :
	mMesh(mesh), 
	mMotionState(new MotionState()), 
	mDebugColor(vec3(1.0f, 0.0f, 0.0f))
{}

Collider::~Collider()
{
	//delete mColliderMesh;
	//delete mMotionState;
}

DynaCollider::DynaCollider() :
	Collider(nullptr), 
	mRB(nullptr)
{}

DynaCollider::DynaCollider(ColliderMesh const* mesh, float const mass) :
	Collider(mesh),
	mRB(new btRigidBody(btScalar(mass), mMotionState, mMesh->mShape, btVector3(0, 0, 0)))
{}

void DynaCollider::Disable()
{
	mRB->forceActivationState(ACTIVE_TAG);
	mRB->setActivationState(DISABLE_SIMULATION);
}

void DynaCollider::Enable()
{
	mRB->forceActivationState(ACTIVE_TAG);
	mRB->setActivationState(DISABLE_DEACTIVATION);
}

DynaCollider::~DynaCollider()
{
	delete mRB;
}

NewCollider::NewCollider(ColliderMesh const* mesh) : 
	mColliderMesh(mesh), 
	mMotionState(make_shared<MotionStateNode>())
{}

NewCollider::~NewCollider()
{
}

void NewCollider::WireFrame() const
{
	if (mColliderMesh) 
		mColliderMesh->WireFrame(mMotionState->GetWorldTransform(), vec3(1.0f, 0.0f, 0.0f));
}

RigidBody::RigidBody() : 
	NewCollider(nullptr),
	mRB(nullptr)
{}

RigidBody::RigidBody(ColliderMesh const* mesh, float const mass) : 
	NewCollider(mesh)
{
	btVector3 localInertia(0, 0, 0);
	if (mass > 0.0f)
		mColliderMesh->mShape->calculateLocalInertia(mass, localInertia);

	btMotionState* motionState	= mMotionState.get();
	btCollisionShape* shape		= mesh->mShape;

	mRB = new btRigidBody(mass, motionState, shape, localInertia);
}

RigidBody::~RigidBody()
{
	delete mRB;
}
