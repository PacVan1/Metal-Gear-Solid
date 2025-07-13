#include "common.h"
#include "maths.h"

// conversion functions
vec3 ToGLM(btVector3 const& v)
{
	return vec3(v.x(), v.y(), v.z());
}

quat ToGLM(const btQuaternion& q)
{
	return quat(q.w(), q.x(), q.y(), q.z());
}

btVector3 ToBT(vec3 const& v)
{
	return btVector3(v.x, v.y, v.z);
}

btQuaternion ToBT(quat const& q)
{
	return btQuaternion(q.x, q.y, q.z, q.w);
}

std::ostream& operator<<(std::ostream& os, vec3 const& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}