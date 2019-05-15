//#include "MxVkMesh.h"
//
//namespace Mix {
//    namespace Graphics {
//        namespace fbx {
//            void MeshMgr::init(std::shared_ptr<Core>& core, std::shared_ptr<DeviceAllocator>& allocator) {
//                GraphicsComponent::init(core);
//                mpAllocator = allocator;
//
//                vk::BufferCreateInfo createInfo;
//                createInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
//                createInfo.size = mBufferSize;
//                createInfo.sharingMode = vk::SharingMode::eExclusive;
//
//                mFence = mCore->getSyncObjMgr().createFence();
//                mCore->getDevice().resetFences(mFence.get());
//            }
//
//            void MeshMgr::beginLoad(const vk::CommandBuffer & cmd) {
//                if (!mBegin) {
//                    mBegin = true;
//                    mCmd = cmd;
//                }
//            }
//
//            void MeshMgr::loadModel(const std::string & name, fbxsdk::FbxScene * scene) {
//                if (mModelInfos.count(name) != 0)
//                    throw std::runtime_error("Error : Model [ " + name + " ] already exists");
//
//                fbxsdk::FbxNode* root = scene->GetRootNode();
//                ModelData modelData;
//                for (int i = 0; i < root->GetChildCount(); ++i) {
//                    processNode(root->GetChild(i), modelData);
//                }
//
//                vk::DeviceSize totalSize = 0;
//                for (auto& data : modelData)
//                    totalSize += data.vertices.size();
//
//                vk::DeviceSize byteSize = totalSize * sizeof(float);
//
//                vk::BufferCreateInfo stagingBufferInfo(vk::BufferCreateFlags(),
//                                                       byteSize,
//                                                       vk::BufferUsageFlagBits::eTransferSrc);
//
//                vk::Buffer stagingBuffer = mCore->getDevice().createBuffer(stagingBufferInfo);
//                MemoryBlock stagingMemBlock = mpAllocator->Allocate(stagingBuffer,
//                                                                    vk::MemoryPropertyFlagBits::eHostVisible |
//                                                                    vk::MemoryPropertyFlagBits::eHostCoherent);
//
//                vk::DeviceSize offset = 0;
//                for (auto& data : modelData) {
//                    memcpy(static_cast<char*>(stagingMemBlock.ptr) + offset, data.vertices.data(), data.vertices.size() * sizeof(float));
//                    offset += data.vertices.size() * sizeof(float);
//                }
//
//                vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(),
//                                                byteSize,
//                                                vk::BufferUsageFlagBits::eTransferDst |
//                                                vk::BufferUsageFlagBits::eVertexBuffer);
//
//                vk::Buffer buffer = mCore->getDevice().createBuffer(bufferInfo);
//                MemoryBlock memBlock = mpAllocator->Allocate(buffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
//
//                vk::BufferCopy bufferCopy;
//                bufferCopy.size = byteSize;
//                bufferCopy.srcOffset = 0;
//                bufferCopy.dstOffset = 0;
//
//                // todo try to submit commandbuffer in endLoad() after all loadModel() calls
//                mCmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
//                mCmd.copyBuffer(stagingBuffer, buffer, bufferCopy);
//                mCmd.end();
//
//                vk::SubmitInfo submitInfo;
//                submitInfo.commandBufferCount = 1;
//                submitInfo.pCommandBuffers = &mCmd;
//
//                mCore->getQueues().transfer.value().submit(submitInfo, mFence.get());
//                mCore->getDevice().waitForFences(mFence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
//                mCore->getDevice().resetFences(mFence.get());
//
//                //destroy staging buffer, free staging memory
//                mCore->getDevice().destroyBuffer(stagingBuffer);
//                mpAllocator->deallocate(stagingMemBlock);
//
//                uint32_t vertexOffset = 0;
//                mMeshInfos.reserve(mMeshInfos.size() + modelData.size());
//
//                ModelInfo modelInfo;
//                modelInfo.name = name;
//                modelInfo.buffer = buffer;
//                modelInfo.meshs.reserve(modelData.size());
//
//                for (auto& data : modelData) {
//                    MeshInfo meshInfo;
//                    meshInfo.name = name + "." + data.name;
//                    meshInfo.buffer = buffer;
//                    meshInfo.firstVertex = vertexOffset;
//                    meshInfo.vertexCount = data.vertices.size() / (3 + 3 + 2);
//                    meshInfo.triangleCount = meshInfo.vertexCount / 3;
//
//                    mMeshInfos.insert(std::make_pair(meshInfo.name, Info(meshInfo, memBlock)));
//                    modelInfo.meshs.push_back(meshInfo);
//                    vertexOffset += meshInfo.vertexCount;
//                }
//
//                mModelInfos[name] = std::move(modelInfo);
//
//
//                /*MeshInfo meshInfo;
//                meshInfo.buffer = buffer;
//                meshInfo.triangleCount = totalSize / (3 + 3 + 2);
//                meshInfo.firstVertex.reserve(modelData.size());
//                meshInfo.vertexCount.reserve(modelData.size());
//
//                uint32_t vertexOffset = 0;
//                for (size_t i = 0; i < modelData.size(); ++i) {
//                    meshInfo.firstVertex.push_back(vertexOffset);
//                    meshInfo.vertexCount.push_back(modelData[i].size() / (3 + 3 + 2));
//
//                    vertexOffset += meshInfo.vertexCount.back();
//                }
//
//                mMeshInfos.insert(std::make_pair(name, Info(meshInfo, memBlock)));*/
//            }
//
//            void MeshMgr::endLoad() {
//                mCmd = nullptr;
//            }
//
//            void MeshMgr::destroy() {
//                if (!mCore)
//                    return;
//
//                for (auto& pair : mMeshInfos) {
//                    mCore->getDevice().destroyBuffer(pair.second.meshInfo.buffer);
//                    mpAllocator->deallocate(pair.second.memoryBlock);
//                }
//
//                mMeshInfos.clear();
//                mModelInfos.clear();
//                mpAllocator = nullptr;
//                mCore = nullptr;
//            }
//
//            void MeshMgr::processNode(fbxsdk::FbxNode * node, ModelData& modelData) {
//                if (node->GetNodeAttribute()) {
//                    switch (node->GetNodeAttribute()->GetAttributeType()) {
//                    case fbxsdk::FbxNodeAttribute::eMesh:
//                        processMesh(node->GetMesh(), modelData);
//                        break;
//                    }
//                }
//
//                for (int i = 0; i < node->GetChildCount(); ++i) {
//                    processNode(node->GetChild(i), modelData);
//                }
//            }
//
//            void MeshMgr::processMesh(fbxsdk::FbxMesh * mesh, ModelData& ModelData) {
//                MeshData data;
//                data.name = mesh->GetNode()->GetName();
//
//                int triangleCount = mesh->GetPolygonCount();
//                data.vertices.reserve(triangleCount * 3 * (3 + 3 + 2));
//
//                fbxsdk::FbxVector4* pCtrlPoints = mesh->GetControlPoints();
//                fbxsdk::FbxGeometryElementUV* pUv = mesh->GetElementUV();
//                fbxsdk::FbxGeometryElementNormal* pNormal = mesh->GetElementNormal();
//
//                int vertexCount = 0;
//                int ctrlPointIndex = 0;
//                for (int i = 0; i < triangleCount; ++i) {
//                    for (int j = 0; j < 3; ++j) {
//                        ctrlPointIndex = mesh->GetPolygonVertex(i, j);
//
//                        readPosition(pCtrlPoints, ctrlPointIndex, data);
//                        readNormal(mesh, ctrlPointIndex, vertexCount, data);
//                        readUV(mesh, pUv, ctrlPointIndex, i, j, data);
//                        ++vertexCount;
//                    }
//                }
//                ModelData.push_back(std::move(data));
//            }
//
//            void MeshMgr::readUV(fbxsdk::FbxMesh* mesh,
//                                 fbxsdk::FbxGeometryElementUV* uv,
//                                 int ctrPointIndex,
//                                 int polygonIndex,
//                                 int vertexIndex,
//                                 MeshData& data) {
//                glm::vec2 result;
//
//                switch (uv->GetMappingMode()) {
//                case fbxsdk::FbxGeometryElement::eByControlPoint:
//                    switch (uv->GetReferenceMode()) {
//                    case fbxsdk::FbxGeometryElement::eDirect:
//                        result.x = static_cast<float>(uv->GetDirectArray().GetAt(ctrPointIndex)[0]);
//                        result.y = static_cast<float>(uv->GetDirectArray().GetAt(ctrPointIndex)[1]);
//                        break;
//                    case fbxsdk::FbxGeometryElement::eIndexToDirect:
//                        int index = uv->GetIndexArray().GetAt(ctrPointIndex);
//                        result.x = static_cast<float>(uv->GetDirectArray().GetAt(index)[0]);
//                        result.y = static_cast<float>(uv->GetDirectArray().GetAt(index)[1]);
//                        break;
//                    }
//                    break;
//
//                case fbxsdk::FbxGeometryElement::eByPolygonVertex:
//                    int uvIndex = mesh->GetTextureUVIndex(polygonIndex, vertexIndex);
//
//                    switch (uv->GetReferenceMode()) {
//                    case fbxsdk::FbxGeometryElement::eDirect:
//                    case fbxsdk::FbxGeometryElement::eIndexToDirect:
//                        result.x = static_cast<float>(uv->GetDirectArray().GetAt(uvIndex)[0]);
//                        result.y = static_cast<float>(uv->GetDirectArray().GetAt(uvIndex)[1]);
//                        break;
//                    }
//                    break;
//                }//getuv
//
//                data.vertices.push_back(result.x);
//                data.vertices.push_back(result.y);
//            }
//
//            void MeshMgr::readNormal(fbxsdk::FbxMesh * mesh,
//                                     int ctrlPointIndex,
//                                     int vertexCount,
//                                     MeshData& data) {
//                     // check if mesh has normal
//                if (mesh->GetElementNormalCount() < 1)
//                    throw std::runtime_error("Error : Mesh has no Normal element");
//
//                glm::vec3 result;
//                auto pNormal = mesh->GetElementNormal(0);
//
//                switch (pNormal->GetMappingMode()) {
//                case fbxsdk::FbxGeometryElement::eByControlPoint:
//                    switch (pNormal->GetReferenceMode()) {
//                    case FbxGeometryElement::eDirect:
//                        result.x = static_cast<float>(pNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[0]);
//                        result.y = static_cast<float>(pNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[1]);
//                        result.z = static_cast<float>(pNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[2]);
//                        break;
//
//                    case FbxGeometryElement::eIndexToDirect:
//                        int index = pNormal->GetIndexArray().GetAt(ctrlPointIndex);
//                        result.x = static_cast<float>(pNormal->GetDirectArray().GetAt(index)[0]);
//                        result.y = static_cast<float>(pNormal->GetDirectArray().GetAt(index)[1]);
//                        result.z = static_cast<float>(pNormal->GetDirectArray().GetAt(index)[2]);
//                        break;
//                    }
//
//                case fbxsdk::FbxGeometryElement::eByPolygonVertex:
//                    switch (pNormal->GetReferenceMode()) {
//                    case fbxsdk::FbxGeometryElement::eDirect:
//                        result.x = static_cast<float>(pNormal->GetDirectArray().GetAt(vertexCount)[0]);
//                        result.y = static_cast<float>(pNormal->GetDirectArray().GetAt(vertexCount)[1]);
//                        result.z = static_cast<float>(pNormal->GetDirectArray().GetAt(vertexCount)[2]);
//                        break;
//
//                    case fbxsdk::FbxGeometryElement::eIndexToDirect:
//                        int index = pNormal->GetIndexArray().GetAt(vertexCount);
//                        result.x = static_cast<float>(pNormal->GetDirectArray().GetAt(index)[0]);
//                        result.y = static_cast<float>(pNormal->GetDirectArray().GetAt(index)[1]);
//                        result.z = static_cast<float>(pNormal->GetDirectArray().GetAt(index)[2]);
//                        break;
//                    }
//                    break;
//                }
//
//                data.vertices.push_back(result.x);
//                data.vertices.push_back(result.y);
//                data.vertices.push_back(result.z);
//            }
//        }
//    }
//}
//
//#ifdef GLTF_ENABLED
//
//Mix::Vulkan::gltf::BoundingBox Mix::Vulkan::gltf::BoundingBox::getAABB(const glm::mat4 & m) {
//    glm::vec3 min = glm::vec3(m[3]);
//    glm::vec3 max = min;
//    glm::vec3 v0, v1;
//
//    glm::vec3 right = glm::vec3(m[0]);
//    v0 = right * this->min.x;
//    v1 = right * this->max.x;
//    min += glm::min(v0, v1);
//    max += glm::max(v0, v1);
//
//    glm::vec3 up = glm::vec3(m[1]);
//    v0 = up * this->min.y;
//    v1 = up * this->max.y;
//    min += glm::min(v0, v1);
//    max += glm::max(v0, v1);
//
//    glm::vec3 back = glm::vec3(m[2]);
//    v0 = back * this->min.z;
//    v1 = back * this->max.z;
//    min += glm::min(v0, v1);
//    max += glm::max(v0, v1);
//
//    return BoundingBox(min, max);
//}
//
//void Mix::Vulkan::gltf::Texture::updateDescriptor() {
//    descriptor.sampler = sampler;
//    descriptor.imageView = view;
//    descriptor.imageLayout = imageLayout;
//}
//
//void Mix::Vulkan::gltf::Texture::destroy() {
//    core->device().destroyImageView(view);
//    core->device().destroyImage(image);
//    core->device().freeMemory(deviceMemory);
//    core->device().destroySampler(sampler);
//}
//
//void Mix::Vulkan::gltf::Texture::fromglTfImage(tinygltf::Image & gltfimage, TextureSampler textureSampler) {
//    unsigned char* buffer = nullptr;
//    vk::DeviceSize bufferSize = 0;
//    bool deleteBuffer = false;
//    if (gltfimage.component == 3) {
//         //Most devices don't support RGB only on Vulkan so convert if necessary
//         //TODO: Check actual format support and transform only if required
//        bufferSize = gltfimage.width * gltfimage.height * 4;
//        buffer = new unsigned char[bufferSize];
//        unsigned char* rgba = buffer;
//        unsigned char* rgb = &gltfimage.image[0];
//        for (int32_t i = 0; i < gltfimage.width * gltfimage.height; ++i) {
//            for (int32_t j = 0; j < 3; ++j) {
//                rgba[j] = rgb[j];
//            }
//            rgba += 4;
//            rgb += 3;
//        }
//        deleteBuffer = true;
//    } else {
//        buffer = &gltfimage.image[0];
//        bufferSize = gltfimage.image.size();
//    }
//
//    vk::Format format = vk::Format::eR8G8B8A8Unorm;
//
//    vk::FormatProperties formatProperties;
//
//    width = gltfimage.width;
//    height = gltfimage.height;
//    mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height))) + 1.0);
//
//    formatProperties = core->physicalDevice().getFormatProperties(format);
//    assert(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eBlitSrc);
//    assert(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eBlitDst);
//
//    vk::MemoryAllocateInfo memAllocInfo;
//    vk::MemoryRequirements memReqs;
//
//    vk::Buffer stagingBuffer;
//    vk::DeviceMemory stagingMemory;
//
//    vk::BufferCreateInfo bufferCreateInfo;
//    bufferCreateInfo.size = bufferSize;
//    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
//    bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
//    stagingBuffer = core->device().createBuffer(bufferCreateInfo);
//    memReqs = core->device().getBufferMemoryRequirements(stagingBuffer);
//
//    memAllocInfo.allocationSize = memReqs.size;
//    memAllocInfo.memoryTypeIndex = core->getMemoryTypeIndex(memReqs.memoryTypeBits,
//                                                            vk::MemoryPropertyFlagBits::eHostVisible |
//                                                            vk::MemoryPropertyFlagBits::eHostCoherent);
//    stagingMemory = core->device().allocateMemory(memAllocInfo);
//    core->device().bindBufferMemory(stagingBuffer, stagingMemory, 0);
//
//    uint8_t *data;
//    data = static_cast<uint8_t*>(core->device().mapMemory(stagingMemory, 0, memReqs.size));
//    memcpy(data, buffer, bufferSize);
//    core->device().unmapMemory(stagingMemory);
//
//    vk::ImageCreateInfo imageCreateInfo;
//    imageCreateInfo.imageType = vk::ImageType::e2D;
//    imageCreateInfo.format = format;
//    imageCreateInfo.mipLevels = mipLevels;
//    imageCreateInfo.arrayLayers = 1;
//    imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
//    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
//    imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled;
//    imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
//    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
//    imageCreateInfo.extent = { width, height, 1 };
//    imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst
//        | vk::ImageUsageFlagBits::eTransferSrc
//        | vk::ImageUsageFlagBits::eSampled;
//    core->device().createImage(imageCreateInfo);
//
//    memReqs = core->device().getImageMemoryRequirements(image);
//    memAllocInfo.allocationSize = memReqs.size;
//    memAllocInfo.memoryTypeIndex = core->getMemoryTypeIndex(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
//    deviceMemory = core->device().allocateMemory(memAllocInfo);
//    core->device().bindImageMemory(image, deviceMemory, 0);
//
//    vk::CommandBuffer copyCmd = cmdMgr->beginTempCommandBuffer();
//
//    vk::ImageSubresourceRange subresourceRange = {};
//    subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
//    subresourceRange.levelCount = 1;
//    subresourceRange.layerCount = 1;
//
//    Tools::transferImageLayout(copyCmd,
//                               image,
//                               vk::ImageLayout::eUndefined,
//                               vk::ImageLayout::eTransferDstOptimal,
//                               subresourceRange);
//
//    vk::BufferImageCopy bufferCopyRegion;
//    bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
//    bufferCopyRegion.imageSubresource.mipLevel = 0;
//    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
//    bufferCopyRegion.imageSubresource.layerCount = 1;
//    bufferCopyRegion.imageExtent = vk::Extent3D(width, height, 1);
//
//    copyCmd.copyBufferToImage(stagingBuffer, image, vk::ImageLayout::eTransferDstOptimal, bufferCopyRegion);
//
//    Tools::transferImageLayout(copyCmd,
//                               image,
//                               vk::ImageLayout::eTransferDstOptimal,
//                               vk::ImageLayout::eTransferDstOptimal,
//                               subresourceRange);
//
//    vk::SubmitInfo submitInfo;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &copyCmd;
//
//    Fence fence = core->getSyncObjMgr().createFence();
//    core->device().resetFences(fence.get());
//
//    core->getQueues().transfer.value().submit(submitInfo, fence.get());
//    core->device().resetFences(fence.get());
//    core->device().waitForFences(fence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
//
//    core->device().freeMemory(stagingMemory);
//    core->device().destroyBuffer(stagingBuffer);
//
//    // we don't use mipmap now
//    //Generate the mip chain(glTF uses jpg and png, so we need to create this manually)
//        /*VkCommandBuffer blitCmd = device->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
//    for (uint32_t i = 1; i < mipLevels; i++) {
//        VkImageBlit imageBlit{};
//
//        imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        imageBlit.srcSubresource.layerCount = 1;
//        imageBlit.srcSubresource.mipLevel = i - 1;
//        imageBlit.srcOffsets[1].x = int32_t(width >> (i - 1));
//        imageBlit.srcOffsets[1].y = int32_t(height >> (i - 1));
//        imageBlit.srcOffsets[1].z = 1;
//
//        imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        imageBlit.dstSubresource.layerCount = 1;
//        imageBlit.dstSubresource.mipLevel = i;
//        imageBlit.dstOffsets[1].x = int32_t(width >> i);
//        imageBlit.dstOffsets[1].y = int32_t(height >> i);
//        imageBlit.dstOffsets[1].z = 1;
//
//        VkImageSubresourceRange mipSubRange = {};
//        mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        mipSubRange.baseMipLevel = i;
//        mipSubRange.levelCount = 1;
//        mipSubRange.layerCount = 1;
//
//        {
//            VkImageMemoryBarrier imageMemoryBarrier{};
//            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//            imageMemoryBarrier.srcAccessMask = 0;
//            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//            imageMemoryBarrier.image = image;
//            imageMemoryBarrier.subresourceRange = mipSubRange;
//            vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
//        }
//
//        vkCmdBlitImage(blitCmd, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);
//
//        {
//            VkImageMemoryBarrier imageMemoryBarrier{};
//            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//            imageMemoryBarrier.image = image;
//            imageMemoryBarrier.subresourceRange = mipSubRange;
//            vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
//        }
//    }
//
//    subresourceRange.levelCount = mipLevels;
//    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//    {
//        VkImageMemoryBarrier imageMemoryBarrier{};
//        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//        imageMemoryBarrier.image = image;
//        imageMemoryBarrier.subresourceRange = subresourceRange;
//        vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
//    }
//
//    device->flushCommandBuffer(blitCmd, copyQueue, true);*/
//
//    vk::SamplerCreateInfo samplerInfo{};
//    samplerInfo.magFilter = textureSampler.magFilter;
//    samplerInfo.minFilter = textureSampler.minFilter;
//    samplerInfo.addressModeU = textureSampler.addressMode.U;
//    samplerInfo.addressModeV = textureSampler.addressMode.V;
//    samplerInfo.addressModeW = textureSampler.addressMode.W;
//    samplerInfo.compareOp = vk::CompareOp::eNever;
//    samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
//    sampler = core->device().createSampler(samplerInfo);
//
//    view = Tools::createImageView2D(core->device(),
//                                    image,
//                                    format,
//                                    vk::ImageAspectFlagBits::eColor);
//
//    descriptor.sampler = sampler;
//    descriptor.imageView = view;
//    descriptor.imageLayout = imageLayout;
//
//    if (deleteBuffer)
//        delete[] buffer;
//}
//
//Mix::Vulkan::gltf::Mesh::Mesh(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator, const glm::mat4 matrix) {
//    this->core = core;
//    this->allocator = allocator;
//
//    uniformBlock.matrix = matrix;
//
//    vk::BufferCreateInfo createInfo;
//    createInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
//    createInfo.size = sizeof(uniformBlock);
//    createInfo.sharingMode = vk::SharingMode::eExclusive;
//
//    buffer = core->device().createBuffer(createInfo);
//    memBlock = allocator->Allocate(buffer,
//                                   vk::MemoryPropertyFlagBits::eHostVisible |
//                                   vk::MemoryPropertyFlagBits::eHostCoherent);
//
//    descriptor = vk::DescriptorBufferInfo(buffer, 0, sizeof(uniformBlock));
//}
//
//void Mix::Vulkan::gltf::Node::update() {
//    if (mesh) {
//        glm::mat4 m = getMatrix();
//        if (skin) {
//            mesh->uniformBlock.matrix = m;
//            // update join matrices
//            glm::mat4 inverseTransform = glm::inverse(m);
//
//            size_t numJoints = std::min((uint32_t)skin->joints.size(), MAX_NUM_JOINTS);
//            for (size_t i = 0; i < numJoints; i++) {
//                Node *jointNode = skin->joints[i];
//                glm::mat4 jointMat = jointNode->getMatrix() * skin->inverseBindMats[i];
//                jointMat = inverseTransform * jointMat;
//                mesh->uniformBlock.jointMatrix[i] = jointMat;
//            }
//
//            mesh->uniformBlock.jointCount = (float)numJoints;
//            memcpy(mesh->memBlock.ptr, &mesh->uniformBlock, sizeof(mesh->uniformBlock));
//        } else {
//            memcpy(mesh->memBlock.ptr, &m, sizeof(glm::mat4));
//        }
//    }
//
//    for (auto& child : children) {
//        child->update();
//    }
//}
//
//void Mix::Vulkan::gltf::Model::destroy() {
//    if (vertices.buffer != nullptr) {
//        core->device().destroy(vertices.buffer);
//        allocator->deallocate(vertices.memory);
//    }
//
//    if (indices.buffer != nullptr) {
//        core->device().destroyBuffer(indices.buffer);
//        allocator->deallocate(indices.memory);
//    }
//
//    for (Texture& texture : textures) {
//        // todo
//    }
//
//    textures.clear();
//    textureSamplers.clear();
//
//    for (auto node : nodes) {
//        delete node;
//    }
//
//    materials.clear();
//    animations.clear();
//    nodes.clear();
//    linearNodes.clear();
//    extensions.clear();
//    skins.clear();
//}
//
//Mix::Vulkan::gltf::Node * Mix::Vulkan::gltf::Model::findNode(Node * parent, uint32_t index) {
//    Node* nodeFound = nullptr;
//    if (parent->index == index) {
//        return parent;
//    }
//    for (auto& child : parent->children) {
//        nodeFound = findNode(child, index);
//        if (nodeFound) {
//            break;
//        }
//    }
//    return nodeFound;
//}
//
//Mix::Vulkan::gltf::Node * Mix::Vulkan::gltf::Model::nodeFromIndex(uint32_t index) {
//    Node* nodeFound = nullptr;
//    for (auto &node : nodes) {
//        nodeFound = findNode(node, index);
//        if (nodeFound) {
//            break;
//        }
//    }
//    return nodeFound;
//}
//
//void Mix::Vulkan::gltf::Model::loadNode(Node * parent, const tinygltf::Node & node, uint32_t nodeIndex, const tinygltf::Model & model, std::vector<uint32_t>& mIndexBuffer, std::vector<Vertex>& mVertexBuffer, float globalscale) {
//    Node *newNode = new Node();
//    newNode->index = nodeIndex;
//    newNode->parent = parent;
//    newNode->name = node.name;
//    newNode->skinIndex = node.skin;
//    newNode->matrix = glm::mat4(1.0f);
//
//    // Generate local node matrix
//    glm::vec3 translation = glm::vec3(0.0f);
//    if (node.translation.size() == 3) {
//        translation = glm::make_vec3(node.translation.data());
//        newNode->translation = translation;
//    }
//    glm::mat4 rotation = glm::mat4(1.0f);
//    if (node.rotation.size() == 4) {
//        glm::quat q = glm::make_quat(node.rotation.data());
//        newNode->rotation = glm::mat4(q);
//    }
//    glm::vec3 scale = glm::vec3(1.0f);
//    if (node.scale.size() == 3) {
//        scale = glm::make_vec3(node.scale.data());
//        newNode->scale = scale;
//    }
//    if (node.matrix.size() == 16) {
//        newNode->matrix = glm::make_mat4x4(node.matrix.data());
//    };
//
//    // Node with children
//    if (node.children.size() > 0) {
//        for (size_t i = 0; i < node.children.size(); i++) {
//            loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, mIndexBuffer, mVertexBuffer, globalscale);
//        }
//    }
//
//    // Node contains mesh data
//    if (node.mesh > -1) {
//        const tinygltf::Mesh mesh = model.mMeshes[node.mesh];
//        Mesh *newMesh = new Mesh(core, allocator, newNode->matrix);
//
//        for (size_t j = 0; j < mesh.primitives.size(); j++) {
//            const tinygltf::Primitive &primitive = mesh.primitives[j];
//
//            uint32_t indexStart = static_cast<uint32_t>(mIndexBuffer.size());
//            uint32_t vertexStart = static_cast<uint32_t>(mVertexBuffer.size());
//            uint32_t indexCount = 0;
//            uint32_t vertexCount = 0;
//            glm::vec3 posMin{};
//            glm::vec3 posMax{};
//
//            bool hasSkin = false;
//            bool hasIndices = primitive.indices > -1;
//            // Vertices
//            {
//                const float *bufferPos = nullptr;
//                const float *bufferNormals = nullptr;
//                const float *bufferTexCoordSet0 = nullptr;
//                const float *bufferTexCoordSet1 = nullptr;
//                const uint16_t *bufferJoints = nullptr;
//                const float *bufferWeights = nullptr;
//
//                // Position attribute is required
//                assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
//
//                const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
//                const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
//
//                bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
//                posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
//                posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
//                vertexCount = static_cast<uint32_t>(posAccessor.count);
//
//                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
//                    const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
//                    const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
//                    bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
//                }
//
//                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
//                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
//                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
//                    bufferTexCoordSet0 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
//                }
//                if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
//                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
//                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
//                    bufferTexCoordSet1 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
//                }
//
//                // Skinning
//                // Joints
//                if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
//                    const tinygltf::Accessor &jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
//                    const tinygltf::BufferView &jointView = model.bufferViews[jointAccessor.bufferView];
//                    bufferJoints = reinterpret_cast<const uint16_t *>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
//                }
//
//                if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
//                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
//                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
//                    bufferWeights = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
//                }
//
//                hasSkin = (bufferJoints && bufferWeights);
//
//                for (size_t v = 0; v < posAccessor.count; v++) {
//                    Vertex vert{};
//                    vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
//                    vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
//                    vert.uv0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) : glm::vec3(0.0f);
//                    vert.uv1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * 2]) : glm::vec3(0.0f);
//
//                    vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
//                    vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
//                    mVertexBuffer.push_back(vert);
//                }
//            }
//            // Indices
//            if (hasIndices) {
//                const tinygltf::Accessor &accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
//                const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
//                const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
//
//                indexCount = static_cast<uint32_t>(accessor.count);
//                const void *dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
//
//                switch (accessor.componentType) {
//                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
//                {
//                    const uint32_t *buf = static_cast<const uint32_t*>(dataPtr);
//                    for (size_t index = 0; index < accessor.count; index++) {
//                        mIndexBuffer.push_back(buf[index] + vertexStart);
//                    }
//                    break;
//                }
//                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
//                {
//                    const uint16_t *buf = static_cast<const uint16_t*>(dataPtr);
//                    for (size_t index = 0; index < accessor.count; index++) {
//                        mIndexBuffer.push_back(buf[index] + vertexStart);
//                    }
//                    break;
//                }
//                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
//                {
//                    const uint8_t *buf = static_cast<const uint8_t*>(dataPtr);
//                    for (size_t index = 0; index < accessor.count; index++) {
//                        mIndexBuffer.push_back(buf[index] + vertexStart);
//                    }
//                    break;
//                }
//                default:
//                    std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
//                    return;
//                }
//            }
//            Primitive *newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
//            newPrimitive->setBoundingBox(posMin, posMax);
//            newMesh->primitives.push_back(newPrimitive);
//        }
//        // Mesh BB from BBs of primitives
//        for (auto p : newMesh->primitives) {
//            if (p->bb.valid && !newMesh->bb.valid) {
//                newMesh->bb = p->bb;
//                newMesh->bb.valid = true;
//            }
//            newMesh->bb.min = glm::min(newMesh->bb.min, p->bb.min);
//            newMesh->bb.max = glm::max(newMesh->bb.max, p->bb.max);
//        }
//        newNode->mesh = newMesh;
//    }
//
//    if (parent) {
//        parent->children.push_back(newNode);
//    } else {
//        nodes.push_back(newNode);
//    }
//    linearNodes.push_back(newNode);
//}
//
//void Mix::Vulkan::gltf::Model::loadSkins(tinygltf::Model & gltfModel) {
//    for (tinygltf::Skin &source : gltfModel.skins) {
//        Skin *newSkin = new Skin{};
//        newSkin->name = source.name;
//
//        // Find skeleton root node
//        if (source.skeleton > -1) {
//            newSkin->skeletonRoot = nodeFromIndex(source.skeleton);
//        }
//
//        // Find joint nodes
//        for (int jointIndex : source.joints) {
//            Node* node = nodeFromIndex(jointIndex);
//            if (node) {
//                newSkin->joints.push_back(nodeFromIndex(jointIndex));
//            }
//        }
//
//        // Get inverse bind matrices from buffer
//        if (source.inverseBindMatrices > -1) {
//            const tinygltf::Accessor &accessor = gltfModel.accessors[source.inverseBindMatrices];
//            const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
//            const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
//            newSkin->inverseBindMats.resize(accessor.count);
//            memcpy(newSkin->inverseBindMats.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
//        }
//
//        skins.push_back(newSkin);
//    }
//}
//
//void Mix::Vulkan::gltf::Model::loadTextures(tinygltf::Model & gltfModel) {
//    for (tinygltf::Texture &tex : gltfModel.textures) {
//        tinygltf::Image image = gltfModel.images[tex.source];
//        TextureSampler textureSampler;
//        if (tex.sampler == -1) {
//            // No sampler specified, use a default one
//            textureSampler.magFilter = vk::Filter::eLinear;
//            textureSampler.minFilter = vk::Filter::eLinear;
//            textureSampler.addressMode.U = vk::SamplerAddressMode::eRepeat;
//            textureSampler.addressMode.V = vk::SamplerAddressMode::eRepeat;
//            textureSampler.addressMode.W = vk::SamplerAddressMode::eRepeat;
//        } else {
//            textureSampler = textureSamplers[tex.sampler];
//        }
//
//        Texture texture;
//        texture.fromglTfImage(image, textureSampler);
//        textures.push_back(texture);
//    }
//}
//
//vk::SamplerAddressMode Mix::Vulkan::gltf::Model::getVkWrapMode(int32_t wrapMode) {
//    switch (wrapMode) {
//    case 10497:
//        return vk::SamplerAddressMode::eRepeat;
//    case 33071:
//        return vk::SamplerAddressMode::eClampToEdge;
//    case 33648:
//        return vk::SamplerAddressMode::eMirroredRepeat;
//    }
//}
//
//vk::Filter Mix::Vulkan::gltf::Model::getVkFilterMode(int32_t filterMode) {
//    switch (filterMode) {
//    case 9728:
//        return vk::Filter::eNearest;
//    case 9729:
//        return vk::Filter::eLinear;
//    case 9984:
//        return vk::Filter::eNearest;
//    case 9985:
//        return vk::Filter::eNearest;
//    case 9986:
//        return vk::Filter::eLinear;
//    case 9987:
//        return vk::Filter::eLinear;
//    }
//}
//
//void Mix::Vulkan::gltf::Model::loadTextureSamplers(tinygltf::Model & gltfModel) {
//    for (tinygltf::Sampler smpl : gltfModel.samplers) {
//        TextureSampler sampler;
//        sampler.minFilter = getVkFilterMode(smpl.minFilter);
//        sampler.magFilter = getVkFilterMode(smpl.magFilter);
//        sampler.addressMode.U = getVkWrapMode(smpl.wrapS);
//        sampler.addressMode.V = getVkWrapMode(smpl.wrapT);
//        sampler.addressMode.W = sampler.addressMode.V;
//        textureSamplers.push_back(sampler);
//    }
//}
//
//void Mix::Vulkan::gltf::Model::loadMaterials(tinygltf::Model & gltfModel) {
//    for (tinygltf::Material &mat : gltfModel.materials) {
//        Material material{};
//        if (mat.values.find("baseColorTexture") != mat.values.end()) {
//            material.baseColorTexture = &textures[mat.values["baseColorTexture"].TextureIndex()];
//            material.texCoordSets.baseColor = mat.values["baseColorTexture"].TextureTexCoord();
//        }
//        if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
//            material.metallicRoughnessTexture = &textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
//            material.texCoordSets.metallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
//        }
//        if (mat.values.find("roughnessFactor") != mat.values.end()) {
//            material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
//        }
//        if (mat.values.find("metallicFactor") != mat.values.end()) {
//            material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
//        }
//        if (mat.values.find("baseColorFactor") != mat.values.end()) {
//            material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
//        }
//        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
//            material.normalTexture = &textures[mat.additionalValues["normalTexture"].TextureIndex()];
//            material.texCoordSets.normal = mat.additionalValues["normalTexture"].TextureTexCoord();
//        }
//        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
//            material.emissiveTexture = &textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
//            material.texCoordSets.emissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
//        }
//        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
//            material.occlusionTexture = &textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
//            material.texCoordSets.occlusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
//        }
//        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
//            tinygltf::Parameter param = mat.additionalValues["alphaMode"];
//            if (param.string_value == "BLEND") {
//                material.alphaMode = Material::ALPHAMODE_BLEND;
//            }
//            if (param.string_value == "MASK") {
//                material.alphaCutoff = 0.5f;
//                material.alphaMode = Material::ALPHAMODE_MASK;
//            }
//        }
//        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
//            material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
//        }
//        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
//            material.emissiveFactor = glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
//        }
//
//        // Extensions
//        // @TODO: Find out if there is a nicer way of reading these properties with recent tinygltf headers
//        if (mat.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
//            auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
//            if (ext->second.Has("specularGlossinessTexture")) {
//                auto index = ext->second.Get("specularGlossinessTexture").Get("index");
//                material.extension.specularGlossinessTexture = &textures[index.Get<int>()];
//                auto texCoordSet = ext->second.Get("specularGlossinessTexture").Get("texCoord");
//                material.texCoordSets.specularGlossiness = texCoordSet.Get<int>();
//                material.pbrWorkflows.specularGlossiness = true;
//            }
//            if (ext->second.Has("diffuseTexture")) {
//                auto index = ext->second.Get("diffuseTexture").Get("index");
//                material.extension.diffuseTexture = &textures[index.Get<int>()];
//            }
//            if (ext->second.Has("diffuseFactor")) {
//                auto factor = ext->second.Get("diffuseFactor");
//                for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
//                    auto val = factor.Get(i);
//                    material.extension.diffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
//                }
//            }
//            if (ext->second.Has("specularFactor")) {
//                auto factor = ext->second.Get("specularFactor");
//                for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
//                    auto val = factor.Get(i);
//                    material.extension.specularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
//                }
//            }
//        }
//
//        materials.push_back(material);
//    }
//    // Push a default material at the end of the list for mMeshes with no material assigned
//    materials.push_back(Material());
//}
//
//void Mix::Vulkan::gltf::Model::loadAnimations(tinygltf::Model & gltfModel) {
//    for (tinygltf::Animation &anim : gltfModel.animations) {
//        Animation animation;
//        animation.name = anim.name;
//
//        if (anim.name.empty()) {
//            animation.name = std::to_string(animations.size());
//        }
//
//        // Samplers
//        for (auto &samp : anim.samplers) {
//            AnimationSampler sampler{};
//
//            if (samp.interpolation == "LINEAR") {
//                sampler.interpolation = AnimationSampler::InterpolationType::LINEAR;
//            }
//            if (samp.interpolation == "STEP") {
//                sampler.interpolation = AnimationSampler::InterpolationType::STEP;
//            }
//            if (samp.interpolation == "CUBICSPLINE") {
//                sampler.interpolation = AnimationSampler::InterpolationType::CUBICSPLINE;
//            }
//
//            // Read sampler input time values
//            {
//                const tinygltf::Accessor &accessor = gltfModel.accessors[samp.input];
//                const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
//                const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
//
//                assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
//
//                const void *dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
//                const float *buf = static_cast<const float*>(dataPtr);
//                for (size_t index = 0; index < accessor.count; index++) {
//                    sampler.inputs.push_back(buf[index]);
//                }
//
//                for (auto input : sampler.inputs) {
//                    if (input < animation.start) {
//                        animation.start = input;
//                    };
//                    if (input > animation.end) {
//                        animation.end = input;
//                    }
//                }
//            }
//
//            // Read sampler output T/R/S values 
//            {
//                const tinygltf::Accessor &accessor = gltfModel.accessors[samp.output];
//                const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
//                const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
//
//                assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
//
//                const void *dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
//
//                switch (accessor.type) {
//                case TINYGLTF_TYPE_VEC3:
//                {
//                    const glm::vec3 *buf = static_cast<const glm::vec3*>(dataPtr);
//                    for (size_t index = 0; index < accessor.count; index++) {
//                        sampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));
//                    }
//                    break;
//                }
//                case TINYGLTF_TYPE_VEC4:
//                {
//                    const glm::vec4 *buf = static_cast<const glm::vec4*>(dataPtr);
//                    for (size_t index = 0; index < accessor.count; index++) {
//                        sampler.outputsVec4.push_back(buf[index]);
//                    }
//                    break;
//                }
//                default:
//                {
//                    std::cout << "unknown type" << std::endl;
//                    break;
//                }
//                }
//            }
//
//            animation.samplers.push_back(sampler);
//        }
//
//        // Channels
//        for (auto &source : anim.channels) {
//            AnimationChannel channel{};
//
//            if (source.target_path == "rotation") {
//                channel.path = AnimationChannel::PathType::ROTATION;
//            }
//            if (source.target_path == "translation") {
//                channel.path = AnimationChannel::PathType::TRANSLATION;
//            }
//            if (source.target_path == "scale") {
//                channel.path = AnimationChannel::PathType::SCALE;
//            }
//            if (source.target_path == "weights") {
//                std::cout << "weights not yet supported, skipping channel" << std::endl;
//                continue;
//            }
//            channel.samplerIndex = source.sampler;
//            channel.node = nodeFromIndex(source.target_node);
//            if (!channel.node) {
//                continue;
//            }
//
//            animation.channels.push_back(channel);
//        }
//
//        animations.push_back(animation);
//    }
//}
//
//void Mix::Vulkan::gltf::Model::loadFromFile(std::string filename, float scale) {
//    tinygltf::Model gltfModel;
//    tinygltf::TinyGLTF gltfContext;
//    std::string error;
//    std::string warning;
//
//    bool binary = false;
//    size_t extpos = filename.rfind('.', filename.length());
//    if (extpos != std::string::npos) {
//        binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
//    }
//
//    bool fileLoaded = binary ? gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str()) : gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());
//
//    std::vector<uint32_t> mIndexBuffer;
//    std::vector<Vertex> mVertexBuffer;
//
//    if (fileLoaded) {
//        loadTextureSamplers(gltfModel);
//        loadTextures(gltfModel);
//        loadMaterials(gltfModel);
//
//        // TODO: scene handling with no default scene
//        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
//        for (size_t i = 0; i < scene.nodes.size(); i++) {
//            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
//            loadNode(nullptr, node, scene.nodes[i], gltfModel, mIndexBuffer, mVertexBuffer, scale);
//        }
//        if (gltfModel.animations.size() > 0) {
//            loadAnimations(gltfModel);
//        }
//        loadSkins(gltfModel);
//
//        for (auto node : linearNodes) {
//            // Assign skins
//            if (node->skinIndex > -1) {
//                node->skin = skins[node->skinIndex];
//            }
//            // Initial pose
//            if (node->mesh) {
//                node->update();
//            }
//        }
//    } else {
//        // TODO: throw
//        std::cerr << "Could not load gltf file: " << error << std::endl;
//        return;
//    }
//
//    extensions = gltfModel.extensionsUsed;
//
//    size_t vertexBufferSize = mVertexBuffer.size() * sizeof(Vertex);
//    size_t indexBufferSize = mIndexBuffer.size() * sizeof(uint32_t);
//    indices.count = static_cast<uint32_t>(mIndexBuffer.size());
//
//    assert(vertexBufferSize > 0);
//
//    struct StagingBuffer {
//        vk::Buffer buffer;
//        MemoryBlock memory;
//    } vertexStaging, indexStaging;
//
//    // Create staging buffers
//    // Vertex data
//    vk::BufferCreateInfo stagingBufferInfo(vk::BufferCreateFlags(),
//                                           vertexBufferSize,
//                                           vk::BufferUsageFlagBits::eTransferSrc);
//    vertexStaging.buffer = core->device().createBuffer(stagingBufferInfo);
//    vertexStaging.memory = allocator->Allocate(vertexStaging.buffer,
//                                               vk::MemoryPropertyFlagBits::eHostVisible |
//                                               vk::MemoryPropertyFlagBits::eHostCoherent);
//    memcpy(vertexStaging.memory.ptr, mVertexBuffer.data(), vertexBufferSize);
//
//    // Index data
//    if (indexBufferSize > 0) {
//        vk::BufferCreateInfo stagingBufferInfo(vk::BufferCreateFlags(),
//                                               indexBufferSize,
//                                               vk::BufferUsageFlagBits::eTransferSrc);
//        indexStaging.buffer = core->device().createBuffer(stagingBufferInfo);
//        indexStaging.memory = allocator->Allocate(indexStaging.buffer,
//                                                  vk::MemoryPropertyFlagBits::eHostVisible |
//                                                  vk::MemoryPropertyFlagBits::eHostCoherent);
//        memcpy(indexStaging.memory.ptr, mIndexBuffer.data(), indexBufferSize);
//    }
//
//    // Create device local buffers
//    // Vertex buffer
//    vk::BufferCreateInfo vertexBufferInfo(vk::BufferCreateFlags(),
//                                          vertexBufferSize,
//                                          vk::BufferUsageFlagBits::eTransferDst |
//                                          vk::BufferUsageFlagBits::eVertexBuffer);
//
//    vertices.buffer = core->device().createBuffer(vertexBufferInfo);
//    vertices.memory = allocator->Allocate(vertices.buffer,
//                                          vk::MemoryPropertyFlagBits::eDeviceLocal);
//
//    // Index buffer
//    if (indexBufferSize > 0) {
//        vk::BufferCreateInfo indexBufferInfo(vk::BufferCreateFlags(),
//                                             indexBufferSize,
//                                             vk::BufferUsageFlagBits::eTransferDst |
//                                             vk::BufferUsageFlagBits::eIndexBuffer);
//
//        indices.buffer = core->device().createBuffer(indexBufferInfo);
//        indices.memory = allocator->Allocate(indices.buffer,
//                                             vk::MemoryPropertyFlagBits::eDeviceLocal);
//    }
//
//    // Copy from staging buffers
//    vk::CommandBuffer copyCmd = cmdMgr->beginTempCommandBuffer();
//
//    vk::BufferCopy copyRegion;
//
//    copyRegion.size = vertexBufferSize;
//    copyCmd.copyBuffer(vertexStaging.buffer, vertices.buffer, copyRegion);
//
//    if (indexBufferSize > 0) {
//        copyRegion.size = indexBufferSize;
//        copyCmd.copyBuffer(indexStaging.buffer, indices.buffer, copyRegion);
//    }
//
//    vk::SubmitInfo submitInfo;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &copyCmd;
//
//    Fence fence = core->getSyncObjMgr().createFence();
//    core->device().resetFences(fence.get());
//
//    core->getQueues().transfer.value().submit(submitInfo, fence.get());
//    core->device().resetFences(fence.get());
//    core->device().waitForFences(fence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
//
//    core->device().destroyBuffer(vertexStaging.buffer);
//    allocator->deallocate(vertexStaging.memory);
//    if (indexBufferSize > 0) {
//        core->device().destroyBuffer(indexStaging.buffer);
//        allocator->deallocate(indexStaging.memory);
//    }
//
//    getSceneDimensions();
//}
//
//void Mix::Vulkan::gltf::Model::calculateBoundingBox(Node * node, Node * parent) {
//    BoundingBox parentBvh = parent ? parent->bvh : BoundingBox(dimensions.min, dimensions.max);
//
//    if (node->mesh) {
//        if (node->mesh->bb.valid) {
//            node->aabb = node->mesh->bb.getAABB(node->getMatrix());
//            if (node->children.size() == 0) {
//                node->bvh.min = node->aabb.min;
//                node->bvh.max = node->aabb.max;
//                node->bvh.valid = true;
//            }
//        }
//    }
//
//    parentBvh.min = glm::min(parentBvh.min, node->bvh.min);
//    parentBvh.max = glm::min(parentBvh.max, node->bvh.max);
//
//    for (auto &child : node->children) {
//        calculateBoundingBox(child, node);
//    }
//}
//
//void Mix::Vulkan::gltf::Model::getSceneDimensions() {
//    // Calculate binary volume hierarchy for all nodes in the scene
//    for (auto node : linearNodes) {
//        calculateBoundingBox(node, nullptr);
//    }
//
//    dimensions.min = glm::vec3(FLT_MAX);
//    dimensions.max = glm::vec3(-FLT_MAX);
//
//    for (auto node : linearNodes) {
//        if (node->bvh.valid) {
//            dimensions.min = glm::min(dimensions.min, node->bvh.min);
//            dimensions.max = glm::max(dimensions.max, node->bvh.max);
//        }
//    }
//
//    // Calculate scene aabb
//    aabb = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.max[0] - dimensions.min[0], dimensions.max[1] - dimensions.min[1], dimensions.max[2] - dimensions.min[2]));
//    aabb[3][0] = dimensions.min[0];
//    aabb[3][1] = dimensions.min[1];
//    aabb[3][2] = dimensions.min[2];
//}
//
//void Mix::Vulkan::gltf::Model::drawNode(Node * node, vk::CommandBuffer cmdBuffer) {
//    if (node->mesh) {
//        for (Primitive *primitive : node->mesh->primitives) {
//            cmdBuffer.drawIndexed(primitive->indexCount,
//                                  1,
//                                  primitive->firstIndex,
//                                  0,
//                                  0);
//        }
//    }
//    for (auto& child : node->children) {
//        drawNode(child, cmdBuffer);
//    }
//}
//
//void Mix::Vulkan::gltf::Model::draw(vk::CommandBuffer cmdBuffer) {
//    cmdBuffer.bindVertexBuffers(0, vertices.buffer, { 0 });
//    cmdBuffer.bindIndexBuffer(indices.buffer, 0, vk::IndexType::eUint32);
//    for (auto& node : nodes) {
//        drawNode(node, cmdBuffer);
//    }
//}
//
//void Mix::Vulkan::gltf::Model::updateAnimation(uint32_t index, float time) {
//    if (index > static_cast<uint32_t>(animations.size()) - 1) {
//        std::cout << "No animation with index " << index << std::endl;
//        return;
//    }
//    Animation &animation = animations[index];
//
//    bool updated = false;
//    for (auto& channel : animation.channels) {
//        AnimationSampler &sampler = animation.samplers[channel.samplerIndex];
//        if (sampler.inputs.size() > sampler.outputsVec4.size()) {
//            continue;
//        }
//
//        for (size_t i = 0; i < sampler.inputs.size() - 1; i++) {
//            if ((time >= sampler.inputs[i]) && (time <= sampler.inputs[i + 1])) {
//                float u = std::max(0.0f, time - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);
//                if (u <= 1.0f) {
//                    switch (channel.path) {
//                    case AnimationChannel::PathType::TRANSLATION:
//                    {
//                        glm::vec4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
//                        channel.node->translation = glm::vec3(trans);
//                        break;
//                    }
//                    case AnimationChannel::PathType::SCALE:
//                    {
//                        glm::vec4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
//                        channel.node->scale = glm::vec3(trans);
//                        break;
//                    }
//                    case AnimationChannel::PathType::ROTATION:
//                    {
//                        glm::quat q1;
//                        q1.x = sampler.outputsVec4[i].x;
//                        q1.y = sampler.outputsVec4[i].y;
//                        q1.z = sampler.outputsVec4[i].z;
//                        q1.w = sampler.outputsVec4[i].w;
//                        glm::quat q2;
//                        q2.x = sampler.outputsVec4[i + 1].x;
//                        q2.y = sampler.outputsVec4[i + 1].y;
//                        q2.z = sampler.outputsVec4[i + 1].z;
//                        q2.w = sampler.outputsVec4[i + 1].w;
//                        channel.node->rotation = glm::normalize(glm::slerp(q1, q2, u));
//                        break;
//                    }
//                    }
//                    updated = true;
//                }
//            }
//        }
//    }
//    if (updated) {
//        for (auto &node : nodes) {
//            node->update();
//        }
//    }
//}
//
//#endif // !GLTF_ENABLED
//
//namespace Mix {
//    namespace Graphics {
//        std::pair<ModelId, std::vector<MeshId>> gltf::MeshMgr::loadModel(const Utils::ModelData& modelData) {
//            // create Vertex buffer
//            vk::DeviceSize byteSize = modelData.vertices.size() * sizeof(Vertex);
//
//            vk::BufferCreateInfo vertexStagingInfo(vk::BufferCreateFlags(),
//                                                   byteSize,
//                                                   vk::BufferUsageFlagBits::eTransferSrc);
//
//            vk::Buffer vertexStaging = mCore->getDevice().createBuffer(vertexStagingInfo);
//            MemoryBlock vertexStagingMem = mpAllocator->Allocate(vertexStaging,
//                                                                 vk::MemoryPropertyFlagBits::eHostVisible |
//                                                                 vk::MemoryPropertyFlagBits::eHostCoherent);
//
//            memcpy(static_cast<char*>(vertexStagingMem.ptr), modelData.vertices.data(), static_cast<size_t>(byteSize));
//
//            vk::BufferCreateInfo vertexBufferInfo(vk::BufferCreateFlags(),
//                                                  byteSize,
//                                                  vk::BufferUsageFlagBits::eTransferDst |
//                                                  vk::BufferUsageFlagBits::eVertexBuffer);
//
//            vk::Buffer vertexBuffer = mCore->getDevice().createBuffer(vertexBufferInfo);
//            MemoryBlock vertexMem = mpAllocator->Allocate(vertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
//
//            vk::BufferCopy vertexBufferCopy(0, 0, byteSize);
//            // create indices buffer
//            byteSize = modelData.indices.size() * sizeof(uint32_t);
//
//            vk::BufferCreateInfo indexStagingInfo(vk::BufferCreateFlags(),
//                                                  byteSize,
//                                                  vk::BufferUsageFlagBits::eTransferSrc);
//
//            vk::Buffer indexStaging = mCore->getDevice().createBuffer(indexStagingInfo);
//            MemoryBlock indexStagingMem = mpAllocator->Allocate(indexStaging,
//                                                                vk::MemoryPropertyFlagBits::eHostVisible |
//                                                                vk::MemoryPropertyFlagBits::eHostCoherent);
//
//            memcpy(static_cast<char*>(indexStagingMem.ptr), modelData.indices.data(), static_cast<size_t>(byteSize));
//
//            vk::BufferCreateInfo indexBufferInfo(vk::BufferCreateFlags(),
//                                                 byteSize,
//                                                 vk::BufferUsageFlagBits::eTransferDst |
//                                                 vk::BufferUsageFlagBits::eIndexBuffer);
//
//            vk::Buffer indexBuffer = mCore->getDevice().createBuffer(indexBufferInfo);
//            MemoryBlock indexMem = mpAllocator->Allocate(indexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
//
//            vk::BufferCopy indexBufferCopy(0, 0, byteSize);
//
//            // todo try to submit commandbuffer in endLoad() after all loadModel() calls
//            mCmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
//            mCmd.copyBuffer(vertexStaging, vertexBuffer, vertexBufferCopy);
//            mCmd.copyBuffer(indexStaging, indexBuffer, indexBufferCopy);
//            mCmd.end();
//
//            vk::SubmitInfo submitInfo;
//            submitInfo.commandBufferCount = 1;
//            submitInfo.pCommandBuffers = &mCmd;
//
//            mCore->getQueues().transfer.value().submit(submitInfo, mFence.get());
//            mCore->getDevice().waitForFences(mFence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
//            mCore->getDevice().resetFences(mFence.get());
//
//            //destroy staging buffer, free staging memory
//            mCore->getDevice().destroyBuffer(vertexStaging);
//            mCore->getDevice().destroyBuffer(indexStaging);
//            mpAllocator->deallocate(vertexStagingMem);
//            mpAllocator->deallocate(indexStagingMem);
//
//            ModelInfo modelInfo;
//            modelInfo.name = modelData.name;
//            modelInfo.vertexBuffer = vertexBuffer;
//            modelInfo.vertexMem = vertexMem;
//            modelInfo.indexBuffer = indexBuffer;
//            modelInfo.indexMem = indexMem;
//            modelInfo.meshes.reserve(modelData.meshes.size());
//
//            size_t primitiveCount = 0;
//            for (auto& mesh : modelData.meshes) {
//                primitiveCount += mesh.primitives.size();
//            }
//
//            std::vector<MeshId> meshIds;
//            meshIds.reserve(primitiveCount);
//            for (auto& mesh : modelData.meshes) {
//                MeshInfo meshInfo;
//                meshInfo.name = mesh.name;
//
//                meshInfo.submeshes.reserve(mesh.primitives.size());
//                for (auto& primitive : mesh.primitives) {
//                    SubmeshInfo submeshInfo;
//                    submeshInfo.firstVertex = primitive.firstVertex;
//                    submeshInfo.vertexCount = primitive.vertexCount;
//                    submeshInfo.firstIndex = primitive.firstIndex;
//                    submeshInfo.indexCount = primitive.indexCount;
//
//                    meshInfo.submeshes.emplace_back(std::move(submeshInfo));
//                }
//                modelInfo.meshes[mNextMeshId.Now()] = std::move(meshInfo);
//                meshIds.push_back(mNextMeshId.Next());
//            }
//
//            mModels[mNextModelId.Now()] = std::move(modelInfo);
//
//            return std::make_pair(mNextModelId.Next(), std::move(meshIds));
//        }
//
//        const gltf::MeshMgr::ModelInfo & gltf::MeshMgr::getModel(const ModelId & modelId) {
//            assert(mModels.count(modelId) != 0);
//            return mModels[modelId];
//        }
//
//        gltf::MeshMgr::SingleMeshInfo gltf::MeshMgr::getMesh(const ModelId & modelId, const MeshId & meshId) {
//            assert(mModels.count(modelId) != 0);
//            assert(mModels[modelId].meshes.count(meshId) != 0);
//            return SingleMeshInfo(mModels[modelId].vertexBuffer,
//                                  mModels[modelId].indexBuffer,
//                                  mModels[modelId].meshes[meshId]);
//        }
//
//        void gltf::MeshMgr::init(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator) {
//            GraphicsComponent::init(core);
//            mpAllocator = allocator;
//
//            mFence = mCore->getSyncObjMgr().createFence();
//            mCore->getDevice().resetFences(mFence.get());
//        }
//
//        void gltf::MeshMgr::destroy() {
//            if (!mCore)
//                return;
//
//            for (auto& pair : mModels) {
//                mCore->getDevice().destroyBuffer(pair.second.vertexBuffer);
//                mCore->getDevice().destroyBuffer(pair.second.indexBuffer);
//                mpAllocator->deallocate(pair.second.vertexMem);
//                mpAllocator->deallocate(pair.second.indexMem);
//            }
//
//            mModels.clear();
//            mpAllocator = nullptr;
//            mCore = nullptr;
//        }
//    }
//}