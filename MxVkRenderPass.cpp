#include "MxVkRenderPass.h"

namespace Mix
{
	void MxVkRenderPass::clear()
	{
		if (!mIsReady)
			return;
		if (mAttachments)
			delete mAttachments;
		if (mAttachRefs)
			delete mAttachRefs;
		if (mDependencies)
			delete mDependencies;
		if (mSubpasses)
			delete mSubpasses;
	}
	MxVkRenderPass::MxVkRenderPass() :mRenderPass(VK_NULL_HANDLE)
	{
	}

	bool MxVkRenderPass::setup(const MxVkManager * manager)
	{
		if (mIsReady)
			destroy();

		mManager = manager;
		mAttachments = new std::vector<VkAttachmentDescription>;
		mAttachRefs = new  std::vector<VkAttachmentReference>;
		mDependencies = new std::vector<VkSubpassDependency>;
		mSubpasses = new std::vector<SubpassContent>;
		if (!mAttachments || !mAttachRefs || !mDependencies || !mSubpasses)
		{
			mManager = nullptr;
			mIsReady = false;
			return false;
		}
		mIsReady = true;
		return true;
	}

	size_t MxVkRenderPass::addColorAttach(VkFormat format, VkSampleCountFlagBits sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout initLayout, VkImageLayout finalLayout)
	{
		VkAttachmentDescription descri = {};
		descri.format = format;
		descri.samples = sampleCount;
		descri.loadOp = loadOp;
		descri.storeOp = storeOp;
		descri.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		descri.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		descri.initialLayout = initLayout;
		descri.finalLayout = finalLayout;
		mAttachments->push_back(std::move(descri));
		return mAttachments->size() - 1;
	}

	size_t MxVkRenderPass::addDepthStencilAttach(VkFormat format, VkSampleCountFlagBits sampleCount, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initLayout, VkImageLayout finalLayout)
	{
		VkAttachmentDescription descri = {};
		descri.format = format;
		descri.samples = sampleCount;
		descri.loadOp = loadOp;
		descri.storeOp = storeOp;
		descri.stencilLoadOp = stencilLoadOp;
		descri.stencilStoreOp = stencilStoreOp;
		descri.initialLayout = initLayout;
		descri.finalLayout = finalLayout;
		mAttachments->push_back(std::move(descri));
		return mAttachments->size() - 1;
	}

	size_t MxVkRenderPass::addColorAttachRef(size_t index, VkImageLayout layout)
	{
		VkAttachmentReference ref;
		ref.attachment = index;
		ref.layout = layout;
		mAttachRefs->push_back(std::move(ref));
		return mAttachRefs->size() - 1;
	}

	size_t MxVkRenderPass::addDepthStencilAttachRef(size_t index, VkImageLayout layout)
	{
		VkAttachmentReference ref;
		ref.attachment = index;
		ref.layout = layout;
		mAttachRefs->push_back(std::move(ref));
		return mAttachRefs->size() - 1;
	}

	size_t MxVkRenderPass::addSubpass(VkPipelineBindPoint bindPoint)
	{
		SubpassContent subpass;
		subpass.bindPoint = bindPoint;
		mSubpasses->push_back(std::move(subpass));
		return mSubpasses->size() - 1;
	}

	bool MxVkRenderPass::addSubpassColorRef(size_t subpassIndex, const std::vector<size_t>& refIndices)
	{
		for (auto& index : refIndices)
		{
			if (!(index < mAttachRefs->size()))
				return false;
		}
		auto& subpass = (*mSubpasses)[subpassIndex];
		subpass.colorRef.reserve(subpass.colorRef.size() + refIndices.size());
		for (const auto& index : refIndices)
		{
			subpass.colorRef.push_back((*mAttachRefs)[index]);
		}
		return true;
	}

	bool MxVkRenderPass::addSubpassColorRef(size_t subpassIndex, const size_t refIndex)
	{
		if (!(refIndex < mAttachRefs->size()))
			return false;
		(*mSubpasses)[subpassIndex].colorRef.push_back((*mAttachRefs)[refIndex]);
		return true;
	}

	bool MxVkRenderPass::addSubpassDepthStencilRef(size_t subpassIndex, size_t refIndex)
	{
		if (!(refIndex < mAttachRefs->size()))
			return false;
		(*mSubpasses)[subpassIndex].depthStencilRef = (*mAttachRefs)[refIndex];
		(*mSubpasses)[subpassIndex].hasDepthStencilRef = true;
		return true;
	}

	bool MxVkRenderPass::addSubpassResolveRef(size_t subpassIndex, size_t refIndex)
	{
		if (!(refIndex < mAttachRefs->size()))
			return false;
		(*mSubpasses)[subpassIndex].resolveRef = (*mAttachRefs)[refIndex];
		(*mSubpasses)[subpassIndex].hasResolveRef = true;
		return true;
	}

	size_t MxVkRenderPass::addDependency(size_t srcSubpass, size_t dstSubpass, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess)
	{
		if (srcSubpass != VK_SUBPASS_EXTERNAL)
		{
			if (srcSubpass > dstSubpass)
				return ~0U;
			if (!(srcSubpass < mSubpasses->size()))
				return ~0U;
		}
		if (dstSubpass != VK_SUBPASS_EXTERNAL && !(dstSubpass > mSubpasses->size()))
			return ~0U;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = srcSubpass;
		dependency.dstSubpass = dstSubpass;
		dependency.srcStageMask = srcStage;
		dependency.dstStageMask = dstStage;
		dependency.srcAccessMask = srcAccess;
		dependency.dstAccessMask = dstAccess;
		mDependencies->push_back(std::move(dependency));
		return mDependencies->size() - 1;
	}

	bool MxVkRenderPass::createRenderPass()
	{
		std::vector<VkSubpassDescription> subpasses(mSubpasses->size());
		for (uint32_t i = 0; i < subpasses.size(); ++i)
		{
			subpasses[i].pipelineBindPoint = (*mSubpasses)[i].bindPoint;

			if ((*mSubpasses)[i].colorRef.size() > 0)
			{
				subpasses[i].pColorAttachments = (*mSubpasses)[i].colorRef.data();
				subpasses[i].colorAttachmentCount = (*mSubpasses)[i].colorRef.size();
			}

			if ((*mSubpasses)[i].inputRef.size() > 0)
			{
				subpasses[i].pInputAttachments = (*mSubpasses)[i].inputRef.data();
				subpasses[i].inputAttachmentCount = (*mSubpasses)[i].inputRef.size();
			}

			if ((*mSubpasses)[i].preserveRef.size() > 0)
			{
				subpasses[i].pResolveAttachments = (*mSubpasses)[i].preserveRef.data();
				subpasses[i].preserveAttachmentCount = (*mSubpasses)[i].preserveRef.size();
			}

			if ((*mSubpasses)[i].hasDepthStencilRef)
			{
				subpasses[i].pDepthStencilAttachment = &(*mSubpasses)[i].depthStencilRef;
			}

			if ((*mSubpasses)[i].hasResolveRef)
			{
				subpasses[i].pResolveAttachments = &(*mSubpasses)[i].resolveRef;
			}
		}

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.pAttachments = mAttachments->data();
		createInfo.attachmentCount = mAttachments->size();
		createInfo.pSubpasses = subpasses.data();
		createInfo.subpassCount = subpasses.size();
		createInfo.pDependencies = mDependencies->data();
		createInfo.dependencyCount = mDependencies->size();

		MX_VK_CHECK_RESULT(vkCreateRenderPass(mManager->getDevice(), &createInfo, nullptr, &mRenderPass));
		clear();
		return true;
	}

	void MxVkRenderPass::beginRenderPass(const VkCommandBuffer commandBuffer, const VkFramebuffer frameBuffer, std::vector<VkClearValue>& clearValues, const VkExtent2D & extent, const VkOffset2D & offset, const VkSubpassContents subpassContent)
	{
		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = mRenderPass;
		beginInfo.framebuffer = frameBuffer;
		beginInfo.pClearValues = clearValues.data();
		beginInfo.clearValueCount = clearValues.size();
		beginInfo.renderArea.offset = offset;
		beginInfo.renderArea.extent = extent;

		vkCmdBeginRenderPass(commandBuffer, &beginInfo, subpassContent);
	}

	void MxVkRenderPass::endRenderPass(const VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffer);
	}

	void MxVkRenderPass::destroy()
	{
		if (!mIsReady)
			return;

		vkDestroyRenderPass(mManager->getDevice(), mRenderPass, nullptr);
		mRenderPass = VK_NULL_HANDLE;
		mManager = nullptr;
		mIsReady = false;
	}


	MxVkRenderPass::~MxVkRenderPass()
	{
		destroy();
	}

}