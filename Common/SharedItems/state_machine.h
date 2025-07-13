#pragma once

template <typename Owner>
class IState
{
public:
	virtual ~IState() = default;
	virtual void Enter(Owner&)				{}
	virtual void Exit(Owner&)				{}
	virtual void Tick(Owner&, float const)	{}
	virtual void Render(Owner&)				{}
	virtual void RenderGUI(Owner&)			{}
};

template <typename Owner>
class StateMachine
{
private:
	Owner*			mOwner;
	IState<Owner>*	mCurrentState;
	IState<Owner>*	mPrevState;

public:
	StateMachine(Owner* o) : mOwner(o) {}
	void SwitchState(IState<Owner>&);
	void Revert();
	void Tick(float const);
	void Render();
	void RenderGUI();
};

template<typename Owner>
inline void StateMachine<Owner>::SwitchState(IState<Owner>& next)
{
	if (mCurrentState) mCurrentState->Exit(*mOwner);
	mPrevState		= mCurrentState;
	mCurrentState	= &next;
	if (mCurrentState) mCurrentState->Enter(*mOwner);
}

template<typename Owner>
inline void StateMachine<Owner>::Revert()
{
	SwitchState(*mOwner, mPrevState);
}

template<typename Owner>
inline void StateMachine<Owner>::Tick(float const dt)
{
	if (mCurrentState) mCurrentState->Tick(*mOwner, dt);
}

template<typename Owner>
inline void StateMachine<Owner>::Render()
{
	if (mCurrentState) mCurrentState->Render(*mOwner);
}

template<typename Owner>
inline void StateMachine<Owner>::RenderGUI()
{
	if (mCurrentState) mCurrentState->RenderGUI(*mOwner);
}
