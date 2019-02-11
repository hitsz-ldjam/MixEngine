#pragma once
#ifndef _MX_VK_MODEL_H_
#define _MX_VK_MODEL_H_

#include"MxVkBuffer.h"
#include"MxVkManager.h"

#include<glm/glm.hpp>
#define FBXSDK_NAMESPACE_USING 0
#include<fbxsdk.h>
#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<unordered_map>

namespace Mix
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

		//返回Vertex对于的顶点绑定(VertexInputBinding)描述
		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0; //顶点数据的绑定点
			bindingDescription.stride = sizeof(Vertex); //两个Vertex之间的间隔
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		//返回顶点数据中每个 属性的描述 
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescription = {};
			attributeDescription[0].binding = 0; //顶点数据的绑定点
			attributeDescription[0].location = 0; //在vertex shader中的location
			attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT; //属性的数据格式
			attributeDescription[0].offset = offsetof(Vertex, pos); //属性相对于一个Vertex起始位置的便宜量

			attributeDescription[1].binding = 0;
			attributeDescription[1].location = 1;
			attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescription[1].offset = offsetof(Vertex, normal);

			attributeDescription[2].binding = 0;
			attributeDescription[2].location = 2;
			attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescription[2].offset = offsetof(Vertex, texCoord);

			return attributeDescription;
		}
	};

	class MxVkModelImporter;

	class MxVkMesh
	{
		friend class MxVkModelImporter;
	public:
		using vertexType = Vertex;

	private:
		std::vector<vertexType> mVertices;
		MxVkBuffer* mVertexBuffer;
		uint32_t mTriangleCount;

	public:
		MxVkMesh() = default;
		~MxVkMesh();

		uint32_t getTriangleCount() const;
		uint32_t getVertexCount() const;
		const vertexType* getBuffer() const; //返回保存的顶点数据数组
		vertexType* getBuffer();
		const MxVkBuffer* getVertexBuffer() const;
		MxVkBuffer* getVertexBuffer();

		void createVertexBuffer(const MxVkManager* manager, MxVkCommandPool* commandPool);
		void draw(const VkCommandBuffer commandBuffer);
	};


	class MxVkModel
	{
		friend class MxVkModelImporter;
	private:
		std::vector<MxVkMesh> mMeshes;

	public:
		MxVkModel() = default;
		~MxVkModel() = default;

		uint32_t getMeshCount() const;
		const MxVkMesh* getMeshes() const;
		MxVkMesh* getMeshes();

		void createVertexBuffer(const MxVkManager* manager, MxVkCommandPool* commandPool);
		void draw(const VkCommandBuffer commandBuffer);
	};

	class MxVkModelImporter
	{
	private:
		fbxsdk::FbxManager* mManager;
		bool mInitialized;

		void processNode(fbxsdk::FbxNode* node, MxVkModel& model);
		void processMesh(fbxsdk::FbxMesh* mesh, MxVkModel& model);
		void readPosition(fbxsdk::FbxVector4* ctrPoints, int ctrPointIndex, Vertex& v);
		void readUV(fbxsdk::FbxMesh* mesh, fbxsdk::FbxGeometryElementUV* uv, int ctrPointIndex, int polygonIndex, int vertexIndex, Vertex& v);
		void readNormal(fbxsdk::FbxMesh* mesh, fbxsdk::FbxGeometryElementNormal* normal, int vertexCount, Vertex& v);

	public:
		MxVkModelImporter();
		~MxVkModelImporter();

		bool initialize();
		MxVkModel* loadModel(const std::string& path);
		void destroy();
	};
}

#endif