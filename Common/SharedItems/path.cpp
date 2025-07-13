#include "common.h"
#include "path.h"

PathFollower::PathFollower(char const*)
{
	Reset();
}

vec2 PathFollower::GetCurrent() const
{
	return mCheckpoints[mCurrentIdx];
}

void PathFollower::TryNext(vec2 const& followerPosition)
{
	float const dist = glm::length(mCheckpoints[mCurrentIdx] - followerPosition);
	if (dist <= 5.0f)
		mCurrentIdx = (mCurrentIdx + 1) % mCheckpoints.size();
}

void PathFollower::Reset()
{
	mCurrentIdx = 0;
}
