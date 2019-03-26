#include "MxVkMesh.h"

namespace Mix {
    namespace Graphics {
        void MeshMgr::init(std::shared_ptr<Core>& core, std::shared_ptr<DeviceAllocator>& allocator) {
            GraphicsComponent::init(core);
            mpAllocator = allocator;

            vk::BufferCreateInfo createInfo;
            createInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.size = mBufferSize;
            createInfo.sharingMode = vk::SharingMode::eExclusive;

            mFence = mCore->getSyncObjMgr().createFence();
            mCore->device().resetFences(mFence.get());
        }

        void MeshMgr::beginLoad(const vk::CommandBuffer & cmd) {
            if (!mBegin) {
                mBegin = true;
                mCmd = cmd;
            }
        }

        void MeshMgr::loadModel(const std::string & name, fbxsdk::FbxScene * scene) {
            fbxsdk::FbxNode* root = scene->GetRootNode();
            ModelData modelData;
            for (int i = 0; i < root->GetChildCount(); ++i) {
                processNode(root->GetChild(i), modelData);
            }

            vk::DeviceSize totalSize = 0;
            for (auto& data : modelData)
                totalSize += data.size();

            vk::DeviceSize byteSize = totalSize * sizeof(MeshData::value_type);

            vk::BufferCreateInfo stagingBufferInfo(vk::BufferCreateFlags(),
                                                   byteSize,
                                                   vk::BufferUsageFlagBits::eTransferSrc);

            vk::Buffer stagingBuffer = mCore->device().createBuffer(stagingBufferInfo);
            MemoryBlock stagingMemBlock = mpAllocator->allocate(stagingBuffer,
                                                                vk::MemoryPropertyFlagBits::eHostVisible |
                                                                vk::MemoryPropertyFlagBits::eHostCoherent);

            vk::DeviceSize offset = 0;
            for (auto& data : modelData) {
                memcpy(static_cast<char*>(stagingMemBlock.ptr) + offset, data.data(), data.size() * sizeof(MeshData::value_type));
                offset += data.size() * sizeof(MeshData::value_type);
            }

            vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(),
                                            byteSize,
                                            vk::BufferUsageFlagBits::eTransferDst |
                                            vk::BufferUsageFlagBits::eVertexBuffer);

            vk::Buffer buffer = mCore->device().createBuffer(bufferInfo);
            MemoryBlock memBlock = mpAllocator->allocate(buffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

            vk::BufferCopy bufferCopy;
            bufferCopy.size = byteSize;
            bufferCopy.srcOffset = 0;
            bufferCopy.dstOffset = 0;

            // todo try to submit commandbuffer in endLoad() after all loadModel() calls
            mCmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
            mCmd.copyBuffer(stagingBuffer, buffer, bufferCopy);
            mCmd.end();

            vk::SubmitInfo submitInfo;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &mCmd;

            mCore->getQueues().transfer.value().submit(submitInfo, mFence.get());
            mCore->device().waitForFences(mFence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
            mCore->device().resetFences(mFence.get());

            //destroy staging buffer, free staging memory
            mCore->device().destroyBuffer(stagingBuffer);
            mpAllocator->deallocate(stagingMemBlock);

            // todo here MeshInfo stands for whole model
            MeshInfo meshInfo;
            meshInfo.buffer = buffer;
            meshInfo.triangleCount = totalSize / (3 + 3 + 2);
            meshInfo.firstVertex.reserve(modelData.size());
            meshInfo.vertexCount.reserve(modelData.size());

            uint32_t vertexOffset = 0;
            for (size_t i = 0; i < modelData.size(); ++i) {
                meshInfo.firstVertex.push_back(vertexOffset);
                meshInfo.vertexCount.push_back(modelData[i].size() / (3 + 3 + 2));

                vertexOffset += meshInfo.vertexCount.back();
            }

            mMeshInfos.insert(std::make_pair(name, Info(meshInfo, memBlock)));
        }

        void MeshMgr::endLoad() {
            mCmd = nullptr;
        }

        void MeshMgr::destroy() {
            if (!mCore)
                return;

            for (auto& pair : mMeshInfos) {
                mCore->device().destroyBuffer(pair.second.meshInfo.buffer);
                mpAllocator->deallocate(pair.second.memoryBlock);
            }

            mMeshInfos.clear();
            mpAllocator = nullptr;
            mCore = nullptr;
        }

        void MeshMgr::processNode(fbxsdk::FbxNode * node, ModelData& modelData) {
            if (node->GetNodeAttribute()) {
                switch (node->GetNodeAttribute()->GetAttributeType()) {
                case fbxsdk::FbxNodeAttribute::eMesh:
                    processMesh(node->GetMesh(), modelData);
                    break;
                }
            }

            for (int i = 0; i < node->GetChildCount(); ++i) {
                processNode(node->GetChild(i), modelData);
            }
        }

        void MeshMgr::processMesh(fbxsdk::FbxMesh * mesh, ModelData& ModelData) {
            int triangleCount = mesh->GetPolygonCount();

            MeshData data;
            data.reserve(triangleCount * 3 * (3 + 3 + 2));

            fbxsdk::FbxVector4* pCtrlPoints = mesh->GetControlPoints();
            fbxsdk::FbxGeometryElementUV* pUv = mesh->GetElementUV();
            fbxsdk::FbxGeometryElementNormal* pNormal = mesh->GetElementNormal();

            int vertexCount = 0;
            int ctrlPointIndex = 0;
            for (int i = 0; i < triangleCount; ++i) {
                for (int j = 0; j < 3; ++j) {
                    ctrlPointIndex = mesh->GetPolygonVertex(i, j);

                    readPosition(pCtrlPoints, ctrlPointIndex, data);
                    readNormal(mesh, pNormal, vertexCount, data);
                    readUV(mesh, pUv, ctrlPointIndex, i, j, data);
                    ++vertexCount;
                }
            }
            ModelData.push_back(std::move(data));
        }

        void MeshMgr::readUV(fbxsdk::FbxMesh* mesh,
                             fbxsdk::FbxGeometryElementUV* uv,
                             int ctrPointIndex,
                             int polygonIndex,
                             int vertexIndex,
                             MeshData& data) {
            switch (uv->GetMappingMode()) {
            case fbxsdk::FbxGeometryElement::eByControlPoint:
                switch (uv->GetReferenceMode()) {
                case fbxsdk::FbxGeometryElement::eDirect:
                    data.push_back(uv->GetDirectArray().GetAt(ctrPointIndex)[0]);
                    data.push_back(uv->GetDirectArray().GetAt(ctrPointIndex)[1]);
                    break;
                case fbxsdk::FbxGeometryElement::eIndexToDirect:
                    int index = uv->GetIndexArray().GetAt(ctrPointIndex);
                    data.push_back(uv->GetDirectArray().GetAt(index)[0]);
                    data.push_back(uv->GetDirectArray().GetAt(index)[1]);
                    break;
                }
                break;

            case fbxsdk::FbxGeometryElement::eByPolygonVertex:
                int uvIndex = mesh->GetTextureUVIndex(polygonIndex, vertexIndex);

                switch (uv->GetReferenceMode()) {
                case fbxsdk::FbxGeometryElement::eDirect:
                case fbxsdk::FbxGeometryElement::eIndexToDirect:
                    data.push_back(uv->GetDirectArray().GetAt(uvIndex)[0]);
                    data.push_back(uv->GetDirectArray().GetAt(uvIndex)[1]);
                    break;
                }
                break;
            }//getuv
        }

        void MeshMgr::readNormal(fbxsdk::FbxMesh * mesh,
                                 fbxsdk::FbxGeometryElementNormal * normal,
                                 int vertexCount,
                                 MeshData& data) {
            switch (normal->GetMappingMode()) {
            case fbxsdk::FbxGeometryElement::eByControlPoint:
                break;
            case fbxsdk::FbxGeometryElement::eByPolygonVertex:
                switch (normal->GetReferenceMode()) {
                case fbxsdk::FbxGeometryElement::eDirect:
                    data.push_back(normal->GetDirectArray().GetAt(vertexCount)[0]);
                    data.push_back(normal->GetDirectArray().GetAt(vertexCount)[1]);
                    data.push_back(normal->GetDirectArray().GetAt(vertexCount)[2]);
                    break;
                case fbxsdk::FbxGeometryElement::eIndexToDirect:
                    int index = normal->GetIndexArray().GetAt(vertexCount);
                    data.push_back(normal->GetDirectArray().GetAt(index)[0]);
                    data.push_back(normal->GetDirectArray().GetAt(index)[1]);
                    data.push_back(normal->GetDirectArray().GetAt(index)[2]);
                    break;
                }
                break;
            }
        }
    }
}