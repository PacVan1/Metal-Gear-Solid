#pragma once

// 2D paths
class PathFollower
{
public:
	vector<vec2>	mCheckpoints;
	u8				mCurrentIdx;

public:
			PathFollower(char const* name);
	vec2	GetCurrent() const;
	void	TryNext(vec2 const& followerPosition);
	void	Reset();
};