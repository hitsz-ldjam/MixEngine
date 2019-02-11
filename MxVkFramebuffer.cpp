#include "MxVkFramebuffer.h"

namespace Mix
{
	void MxVkFramebuffer::clear()
	{
		if (mAttachments)
			delete mAttachments;
	}
	MxVkFramebuffer::MxVkFramebuffer() :mRenderPass(VK_NULL_HANDLE)
	{
	}

	bool MxVkFramebuffer::setup(const MxVkManager * manager)
	{
		if (mIsReady)
			destroy();

		mAttachments = new std::vector<VkImageView>;
		if (!mAttachments)
			return false;
		mManager = manager;
		mIsReady = true;
		return true;
	}

	void MxVkFramebuffer::addAttachments(std::vector<VkImageView>& attachments)
	{
		mAttachments->insert(mAttachments->end(), attachments.begin(), attachments.end());
	}

	bool MxVkFramebuffer::createFramebuffer()
	{
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = mRenderPass;
		createInfo.pAttachments = mAttachments->data();
		createInfo.attachmentCount = mAttachments->size();
		createInfo.width = mExtent.width;
		createInfo.height = mExtent.height;
		createInfo.layers = mLayers;

		MX_VK_CHECK_RESULT(vkCreateFramebuffer(mManager->getDevice(), &createInfo, nullptr, &mFramebuffer));
		return true;
	}

	void MxVkFramebuffer::destroy()
	{
		if (!mIsReady)
			return;

		if (mAttachments)
			delete mAttachments;
		vkDestroyFramebuffer(mManager->getDevice(), mFramebuffer, nullptr);
		mAttachments = nullptr;
		mManager = nullptr;
		mRenderPass = VK_NULL_HANDLE;
		mFramebuffer = VK_NULL_HANDLE;
		mExtent = { 0,0 };
		mLayers = 0;
	}

	MxVkFramebuffer::~MxVkFramebuffer()
	{
		destroy();
	}
}
