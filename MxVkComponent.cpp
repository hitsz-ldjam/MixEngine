#include "MxVkComponent.h"

namespace Mix
{
	MxVkComponent::MxVkComponent() :mIsReady(false), mManager(nullptr)
	{
	}

	bool MxVkComponent::setup(const MxVkManager * manager)
	{
		if (mIsReady)
			destroy();
		mManager = manager;
		mIsReady = true;
		return true;
	}

	void MxVkComponent::destroy()
	{
		if (!mIsReady)
			return;
		mManager = nullptr;
		mIsReady = false;
	}


	MxVkComponent::~MxVkComponent()
	{
		destroy();
	}
}
