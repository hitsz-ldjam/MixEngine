#include "MxResourceParser.h"

namespace Mix {
    namespace Resource {
        std::shared_ptr<Object> GltfParser::Load(const std::filesystem::path& _path, const ResourceType _type) {
            Utils::GltfLoader::ModelData modelData;
            bool success = false;
            if (_type == ResourceType::GLTF_BIN)
                success = mLoader.LoadFromGlb(modelData, _path, _path.filename().string());
            else if (_type == ResourceType::GLTF_ASCII)
                success = mLoader.LoadFromJson(modelData, _path, _path.filename().string());

            if (!success)
                return nullptr;

            auto model = ParseModelData(_path, modelData);
            mRefMgr->AddElement(model->GetGuid(), model);
            return Parse(model->GetGuid());
        }

        std::shared_ptr<Object> GltfParser::Load(const std::filesystem::path& _path, const std::string& _ext) {
            Utils::GltfLoader::ModelData modelData;
            bool success = false;
            if (_ext == "glb")
                success = mLoader.LoadFromGlb(modelData, _path, _path.filename().string());
            else if (_ext == "gltf")
                success = mLoader.LoadFromJson(modelData, _path, _path.filename().string());

            if (!success)
                return nullptr;

            auto model = ParseModelData(_path, modelData);
            mRefMgr->AddElement(model->GetGuid(), model);
            return Parse(model->GetGuid());
        }

        std::shared_ptr<Object> GltfParser::Parse(const Guid& _key) {
            auto ref = mRefMgr->GetReference(_key);
            auto ptr = ref.Get<ResModel>();

            auto gameObject = std::shared_ptr<GameObject>();
            gameObject->SetModelRef(ref);
            for (auto& mesh : ptr->meshes) {
                auto child = new GameObject();
                auto filter = child->AddComponent<MeshFilter>();
                filter->SetMesh(std::make_shared<Mesh>(mesh));
            }

            return gameObject;
#pragma region TODO_IMPLEMENT
            //GameObject* Vulkan::CreateModelObj(const Utils::ModelData& _modelData) {
        //    // test
        //    vk::DescriptorImageInfo imageInfo;
        //    imageInfo.imageView = texImageView;
        //    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        //    imageInfo.sampler = sampler;

        //    auto cmd = mCommandMgr->allocCommandBuffer();

        //    mMeshMgr->beginLoad(cmd);
        //    auto idPair = std::move(mMeshMgr->loadModel(_modelData));
        //    mCommandMgr->freeCommandBuffers(cmd);
        //    mMeshMgr->endLoad();

        //    MeshFilter* filter;
        //    Transform* transform;
        //    MeshRenderer* render;

        //    GameObject* obj = new GameObject();
        //    for (size_t i = 0; i < _modelData.meshes.size(); ++i) {
        //        GameObject* child = new GameObject();

        //        transform = child->GetComponent<Transform>();
        //        transform->position() = _modelData.meshes[i].transform.translation;
        //        transform->rotation() = _modelData.meshes[i].transform.rotation;
        //        transform->scale() = _modelData.meshes[i].transform.scale;

        //        filter = child->AddComponent<MeshFilter>();
        //        filter->setMeshRef(idPair.first, idPair.second[i]);

        //        std::vector<std::shared_ptr<Buffer>> uniformBuffers;
        //        std::vector<vk::DescriptorSet> descriptorSets;
        //        uniformBuffers.reserve(mSwapchain->imageCount());

        //        for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
        //            uniformBuffers.emplace_back(Buffer::CreateBuffer(mCore,
        //                                        mAllocator,
        //                                        vk::BufferUsageFlagBits::eUniformBuffer,
        //                                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        //                                        sizeof(MeshUniform)));
        //        }

        //        descriptorSets = mDescriptorPool->allocDescriptorSet(mDescriptorSetLayout["Object"]->get(), mSwapchain->imageCount());
        //        for (size_t i = 0; i < descriptorSets.size(); ++i) {
        //            std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};
        //            descriptorWrites[0].dstSet = descriptorSets[i]; //descriptor which will be write in
        //            descriptorWrites[0].dstBinding = 0; //destination binding
        //            descriptorWrites[0].dstArrayElement = 0;
        //            descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer; //the type of the descriptor that will be wirte in
        //            descriptorWrites[0].descriptorCount = 1; //descriptor count
        //            descriptorWrites[0].pBufferInfo = &uniformBuffers[i]->descriptor; //descriptorBufferInfo
        //            descriptorWrites[0].pImageInfo = nullptr;
        //            descriptorWrites[0].pTexelBufferView = nullptr;

        //            descriptorWrites[1].dstSet = descriptorSets[i];
        //            descriptorWrites[1].dstBinding = 1;
        //            descriptorWrites[1].dstArrayElement = 0;
        //            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        //            descriptorWrites[1].descriptorCount = 1;
        //            descriptorWrites[1].pBufferInfo = nullptr;
        //            descriptorWrites[1].pImageInfo = &imageInfo;
        //            descriptorWrites[1].pTexelBufferView = nullptr;

        //            mCore->device().updateDescriptorSets(descriptorWrites, nullptr);
        //        }

        //        render = child->AddComponent<MeshRenderer>(uniformBuffers, descriptorSets);

        //        obj->addChild(child);
        //    }

        //    return obj;
        //}
#pragma endregion
        }

        std::shared_ptr<ResModel> GltfParser::ParseModelData(const std::filesystem::path&        _path,
                                                             const Utils::GltfLoader::ModelData& _modelData) {
            const auto vulkan = Object::FindObjectOfType<Graphics::Vulkan>();
            const auto vkAllocator = vulkan->GetAllocator();
            auto cmd = vulkan->GetCommandMgr()->allocCommandBuffer();

            // upload vertex data
            vk::DeviceSize byteSize = _modelData.vertices.size() * sizeof(Graphics::Vertex);
            const auto vertexStaging = Graphics::Buffer::CreateBuffer(vulkan->GetCore(),
                                                                      vkAllocator,
                                                                      vk::BufferUsageFlagBits::eTransferSrc,
                                                                      vk::MemoryPropertyFlagBits::eHostVisible |
                                                                      vk::MemoryPropertyFlagBits::eHostCoherent,
                                                                      byteSize,
                                                                      vk::SharingMode::eExclusive,
                                                                      _modelData.vertices.data());

            const auto vertexBuffer = Graphics::Buffer::CreateBuffer(vulkan->GetCore(),
                                                                     vkAllocator,
                                                                     vk::BufferUsageFlagBits::eVertexBuffer |
                                                                     vk::BufferUsageFlagBits::eTransferDst,
                                                                     vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                                     byteSize);

            const vk::BufferCopy vertexBufferCopy(0, 0, byteSize);

            // upload index data
            byteSize = _modelData.indices.size() * sizeof(uint32_t);
            const auto indexStaging = Graphics::Buffer::CreateBuffer(vulkan->GetCore(),
                                                                     vkAllocator,
                                                                     vk::BufferUsageFlagBits::eTransferSrc,
                                                                     vk::MemoryPropertyFlagBits::eHostVisible |
                                                                     vk::MemoryPropertyFlagBits::eHostCoherent,
                                                                     byteSize,
                                                                     vk::SharingMode::eExclusive,
                                                                     _modelData.indices.data());

            const auto indexBuffer = Graphics::Buffer::CreateBuffer(vulkan->GetCore(),
                                                                    vkAllocator,
                                                                    vk::BufferUsageFlagBits::eTransferDst |
                                                                    vk::BufferUsageFlagBits::eIndexBuffer,
                                                                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                                                                    byteSize);

            const vk::BufferCopy indexBufferCopy(0, 0, byteSize);

            cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
            cmd.copyBuffer(vertexStaging->buffer, vertexBuffer->buffer, vertexBufferCopy);
            cmd.copyBuffer(indexStaging->buffer, indexBuffer->buffer, indexBufferCopy);
            cmd.end();

            vk::SubmitInfo submitInfo;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &cmd;

            vulkan->GetCore()->GetQueues().transfer.value().submit(submitInfo, mFence.get());
            vulkan->GetLogicalDevice().waitForFences(mFence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
            vulkan->GetLogicalDevice().resetFences(mFence.get());

            auto model = std::make_shared<ResModel>(Utils::GuidGenerator::GetGuid(_path),
                                                    _path.filename().string(),
                                                    _path);

            model->meshes.reserve(_modelData.meshes.size());
            model->vertexBuffer = vertexBuffer;
            model->indexBuffer = indexBuffer;
            model->vertexCount = _modelData.vertices.size();
            model->indexCount = _modelData.indices.size();

            for (auto& meshData : _modelData.meshes) {
                auto mesh = std::make_shared<ResMesh>(Utils::GuidGenerator::GetGuid(_path.generic_string() + "/" + meshData.name),
                                                      meshData.name,
                                                      _path);

                mesh->vertexBuffer = vertexBuffer;
                mesh->indexBuffer = indexBuffer;

                mesh->submeshes.reserve(meshData.primitives.size());
                for (auto& submesh : meshData.primitives)
                    mesh->submeshes.emplace_back(submesh.firstVertex,
                                                 submesh.vertexCount,
                                                 submesh.firstIndex,
                                                 submesh.indexCount);

                model->meshes.push_back(mRefMgr->AddElement(mesh->GetGuid(),
                                        std::static_pointer_cast<ResourceBase>(mesh)));
            }

            return model;
        }
    }
}
