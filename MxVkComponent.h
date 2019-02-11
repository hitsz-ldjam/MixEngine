#pragma once
#ifndef _MX_VK_COMPONENT_H_
#define _MX_VK_COMPONENT_H_

#include"MxVkManager.h"

namespace Mix
{
	class MxVkComponent
	{
	protected:
		bool mIsReady;
		const MxVkManager* mManager;
	public:
		MxVkComponent();
		virtual bool setup(const MxVkManager* manager);
		virtual void destroy();
		virtual ~MxVkComponent();
	};
}

#endif // !_MX_VK_OBJECT_H_