#pragma once

#include <cmath>

// external
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <LinearMath/btVector3.h>
#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btQuaternion.h>

#include <assimp/quaternion.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

using namespace glm;

// mathematical constants
//unsigned constexpr	MAX_UNSIGNED		= ~0u;					// 0x00000000 >flipped> 0xffffffff
//signed constexpr	MIN_SIGNED			= ~(MAX_UNSIGNED >> 1);	// 0xffffffff >shifted> 0x7fffffff >flipped> 0x80000000
float constexpr		PI					= 3.1415926535897932f;
//float constexpr		TWO_PI				= 2.0f * PI;
//float constexpr		PI_OVER_MIN_SIGNED	= PI / MIN_SIGNED;

// conversion functions
btVector3		ToBT(vec3 const& from);
btQuaternion	ToBT(quat const& from);
vec3			ToGLM(aiVector3D const& v);
void			ToGLM(aiVector3D const& from, vec3& to);
mat4			ToGLM(aiMatrix4x4 const& from);
void			ToGLM(aiMatrix4x4 const& from, mat4& to);
quat			ToGLM(aiQuaternion const& from);
vec3			ToGLM(btVector3 const& from);
quat			ToGLM(btQuaternion const& from);

std::ostream& operator<<(std::ostream& os, vec3 const& v);