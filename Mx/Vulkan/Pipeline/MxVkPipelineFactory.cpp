#include "MxVkPipelineFactory.h"
#include "MxVkPipeline.h"
#include "MxVkShader.h"
#include "../../../MixEngine.h"
#include "../../Resource/MxResourceLoader.h"
#include "../../Resource/Shader/MxShaderSource.h"
#include <nlohmann/json.hpp>

namespace Mix {
	namespace Graphics {
		const char* const PipelineFactory::sDefaultEntry = "main";

		const vk::PipelineColorBlendAttachmentState PipelineFactory::DefaultBlendAttachment = {
			false,
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA
		};

		std::vector<Shader> PipelineFactory::CreateShaderStageFromJson(const nlohmann::json& _json, const std::shared_ptr<Device>& _device) {
			if (!_json.contains("shaderStage"))
				throw Exception("Pipeline description file does not contain key: shaderStage");

			std::vector<Shader> results;
			for (auto& path : _json["shaderStage"]) {
				results.emplace_back(_device, *(MixEngine::Instance().getModule<Resource::ResourceLoader>()->load<Resource::ShaderSource>(path.get<std::string>())));
			}
			return results;
		}

		void PipelineFactory::CreateVertexInputFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			if (!_json.contains("vertexInput"))
				throw Exception("Pipeline description file does not contain key: vertexInput");

			std::vector<vk::VertexInputBindingDescription> bindings;
			std::vector<vk::VertexInputAttributeDescription> attributes;

			uint32_t bd = 0, lc = 0;
			for (auto& bindingJs : _json["vertexInput"]) {
				vk::VertexInputBindingDescription binding;
				binding.stride = bindingJs["stride"].get<uint32_t>();
				binding.inputRate =
					bindingJs["inputRate"].get<std::string>() == std::string("vertex") ?
					vk::VertexInputRate::eVertex : vk::VertexInputRate::eInstance;

				auto& attributeJs = bindingJs["attribute"];
				for (auto& attri : attributeJs) {
					vk::Format format = GetVkFormatFromStr(attri["format"].get<std::string>());
					uint32_t offset = attri["offset"].get<uint32_t>();
					attributes.emplace_back(lc++, bd, format, offset);
				}

				binding.binding = bd++;
				bindings.push_back(binding);
			}

			_factory.setVertexInput(bindings, attributes);
		}

		void PipelineFactory::CreateInputAssemblyFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			if (!_json.contains("inputAssembly"))
				throw Exception("Pipeline description file does not contain key: inputAssembly");
			auto& inputAssemblyJs = _json["inputAssembly"];

			vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
			auto str = inputAssemblyJs["topology"].get<std::string>();

			if (str == "PointList")				topology = vk::PrimitiveTopology::ePointList;
			else if (str == "LineList")			topology = vk::PrimitiveTopology::eLineList;
			else if (str == "LineStrip")		topology = vk::PrimitiveTopology::eLineStrip;
			else if (str == "TriangleList")		topology = vk::PrimitiveTopology::eTriangleList;
			else if (str == "TriangleStrip")	topology = vk::PrimitiveTopology::eTriangleStrip;
			else if (str == "TriangleFan")		topology = vk::PrimitiveTopology::eTriangleFan;

			bool restart = false;
			if (inputAssemblyJs.contains("primitiveRestart"))
				restart = inputAssemblyJs["primitiveRestart"].get<bool>();

			_factory.setInputAssembly(topology, restart);
		}

		void PipelineFactory::CreateRasterizationFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {

			vk::PolygonMode polygon = vk::PolygonMode::eFill;
			vk::CullModeFlags cullMode = vk::CullModeFlagBits::eNone;
			vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise;
			float lineWidth = 1.0f;
			bool depthClamp = false;
			bool discard = false;

			bool depthBiasEnable = false;
			float constantFactor = 0.0f;
			float clamp = 0.0f;
			float slopeFactor = 0.0f;

			if (_json.contains("rasterization")) {
				auto& rasterizationJs = _json["rasterization"];
				if (rasterizationJs.contains("polygon")) {
					auto str = rasterizationJs["polygon"].get<std::string>();

					if (str == "Fill");
					else if (str == "Line") polygon = vk::PolygonMode::eLine;
					else if (str == "Point")polygon = vk::PolygonMode::ePoint;
				}

				if (rasterizationJs.contains("cullMode")) {
					auto str = rasterizationJs["cullMode"].get<std::string>();

					if (str == "Back");
					if (str == "Front") cullMode = vk::CullModeFlagBits::eFront;
					if (str == "FrontAndBack")cullMode = vk::CullModeFlagBits::eFrontAndBack;
					if (str == "None")cullMode = vk::CullModeFlagBits::eNone;
				}

				if (rasterizationJs.contains("frontFace")) {
					auto str = rasterizationJs["frontFace"].get<std::string>();

					if (str == "ClockWise") frontFace = vk::FrontFace::eClockwise;
				}

				if (rasterizationJs.contains("lineWidth")) {
					lineWidth = rasterizationJs["lineWidth"].get<float>();
				}

				if (rasterizationJs.contains("depthClamp")) {
					depthClamp = rasterizationJs.get<bool>();
				}

				if (rasterizationJs.contains("discard")) {
					discard = rasterizationJs["discard"].get<bool>();
				}

				// depthBias

				if (rasterizationJs.contains("depthBias")) {
					auto& depthBiasJs = rasterizationJs["depthBias"];
					depthBiasEnable = depthBiasJs["enable"].get<bool>();
					constantFactor = depthBiasJs["constantFactor"].get<float>();
					clamp = depthBiasJs["clamp"].get<float>();
					slopeFactor = depthBiasJs["slopeFactor"].get<float>();
				}
			}

			_factory.setRasterization(polygon, cullMode, frontFace, lineWidth, depthClamp, discard);
			_factory.setDepthBias(depthBiasEnable, constantFactor, clamp, slopeFactor);
		}

		void PipelineFactory::CreateDepthStencilFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			bool depthTestEnable = false;
			bool writeEnable = false;
			auto compareOp = vk::CompareOp::eAlways;

			bool stencilTestEnable = false;
			vk::StencilOpState front, back;
			if (_json.contains("depthStencil")) {
				auto& depthStencilJs = _json["depthStencil"];

				// depthTest
				if (depthStencilJs.contains("depthTest")) {
					auto& depthTestJs = depthStencilJs["depthTest"];

					if (depthTestJs.contains("enable"))
						depthTestEnable = depthTestJs["enable"].get<bool>();

					if (depthTestJs.contains("writeEnable"))
						writeEnable = depthTestJs["writeEnable"].get<bool>();

					if (depthTestJs.contains("compareOp"))
						compareOp = GetVkCompareOpFromStr(depthTestJs["compareOp"].get<std::string>());
				}

				// stencil
				if (depthStencilJs.contains("stencilTest")) {
					auto& stencilTestJs = depthStencilJs["stencilTest"];


					if (stencilTestJs.contains("enable"))
						stencilTestEnable = stencilTestJs["enable"].get<bool>();

					if (stencilTestJs.contains("opState")) {
						auto& opStateJs = depthStencilJs["opState"];
						if (opStateJs.contains("front")) {
							auto& frontJs = opStateJs["front"];
							front.failOp = GetVkStencilOpFromStr(frontJs["failOp"].get<std::string>());
							front.passOp = GetVkStencilOpFromStr(frontJs["passOp"].get<std::string>());
							front.depthFailOp = GetVkStencilOpFromStr(frontJs["depthFailOp"].get<std::string>());
							front.compareOp = GetVkCompareOpFromStr(frontJs["compareOp"].get<std::string>());
							front.compareMask = frontJs["compareMask"].get<uint32_t>();
							front.writeMask = frontJs["writeMask"].get<uint32_t>();
							front.reference = frontJs.contains("reference") ? frontJs["reference"].get<uint32_t>() : 0;
						}

						if (opStateJs.contains("back")) {
							auto& backJs = opStateJs["back"];
							back.failOp = GetVkStencilOpFromStr(backJs["failOp"].get<std::string>());
							back.passOp = GetVkStencilOpFromStr(backJs["passOp"].get<std::string>());
							back.depthFailOp = GetVkStencilOpFromStr(backJs["depthFailOp"].get<std::string>());
							back.compareOp = GetVkCompareOpFromStr(backJs["compareOp"].get<std::string>());
							back.compareMask = backJs["compareMask"].get<uint32_t>();
							back.writeMask = backJs["writeMask"].get<uint32_t>();
							back.reference = backJs.contains("reference") ? backJs["reference"].get<uint32_t>() : 0;
						}
					}

				}
			}

			_factory.setDepthTest(depthTestEnable, writeEnable, compareOp);
			_factory.setStencilTest(stencilTestEnable, front, back);
		}

		void PipelineFactory::CreateBlendFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			if (_json.contains("blend")) {
				auto& blendJs = _json["blend"];

				bool logicalOpEnable = blendJs.contains("logicOpEnable") ? blendJs["logicOpEnable"].get<bool>() : false;
				vk::LogicOp logicOp = blendJs.contains("logicOp") ? GetVkLogicOpFromStr(blendJs["logicOp"].get<std::string>()) : vk::LogicOp::eClear;

				float constantR = 0.0f, constantG = 0.0f, constantB = 0.0f, constantA = 0.0f;
				if (blendJs.contains("constants")) {
					auto& constantsJs = blendJs["constants"];
					constantR = constantsJs.contains("R") ? constantsJs["R"].get<float>() : 0.0f;
					constantG = constantsJs.contains("G") ? constantsJs["G"].get<float>() : 0.0f;
					constantB = constantsJs.contains("B") ? constantsJs["B"].get<float>() : 0.0f;
					constantA = constantsJs.contains("A") ? constantsJs["A"].get<float>() : 0.0f;
				}

				if (blendJs.contains("attachments")) {
					std::vector<vk::PipelineColorBlendAttachmentState> attachments;
					for (auto& attachJs : blendJs["attachments"]) {
						vk::PipelineColorBlendAttachmentState attach;
						if (attachJs["blendEnable"].get<bool>()) {
							attach.blendEnable = true;

							auto& colorJs = attachJs["color"];
							attach.srcColorBlendFactor = GetVkBlendFactorFromStr(colorJs["srcFactor"].get<std::string>());
							attach.dstColorBlendFactor = GetVkBlendFactorFromStr(colorJs["dstFactor"].get<std::string>());
							attach.colorBlendOp = GetVkBlendOpFromStr(colorJs["blendOp"].get<std::string>());

							auto& alphaJs = attachJs["alpha"];
							attach.srcAlphaBlendFactor = GetVkBlendFactorFromStr(alphaJs["srcFactor"].get<std::string>());
							attach.dstAlphaBlendFactor = GetVkBlendFactorFromStr(alphaJs["dstFactor"].get<std::string>());
							attach.alphaBlendOp = GetVkBlendOpFromStr(alphaJs["blendOp"].get<std::string>());

							attach.colorWriteMask = static_cast<vk::ColorComponentFlags>(attachJs["colorWriteMask"].get<uint32_t>());
						}
						attachments.push_back(attach);
					}
					_factory.setBlendAttachments(attachments);
				}
				_factory.setBlend(logicalOpEnable, logicOp, constantR, constantG, constantB, constantA);
			}
			else {
				_factory.setBlend();
			}

		}

		void PipelineFactory::CreateDynamicStateFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			if (!_json.contains("dynamicState"))
				return;

			std::vector<vk::DynamicState> dynamicStates;
			for (auto& dynamicStateJs : _json["dynamicState"]) {
				dynamicStates.push_back(GetVkDynamicStateFromStr(dynamicStateJs.get<std::string>()));
			}

			_factory.setDynamicState(dynamicStates);
		}

		void PipelineFactory::CreatePushConstantFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			if (!_json.contains("pushConstant"))
				return;

			std::vector<vk::PushConstantRange> pushConstantRanges;
			for (auto& pushConstJs : _json["pushConstant"]) {
				vk::PushConstantRange pushConstant;
				pushConstant.stageFlags = GetVkShaderStageFromStr(pushConstJs["stage"].get<std::string>());
				pushConstant.offset = pushConstJs["offset"].get<uint32_t>();
				pushConstant.size = pushConstJs["size"].get<uint32_t>();
				pushConstantRanges.push_back(pushConstant);
			}

			_factory.setPushConstantRanges(pushConstantRanges);
		}

		void PipelineFactory::CreateDescriptorSetLayoutFromJson(const nlohmann::json& _json,
																PipelineFactory& _factory,
																const std::shared_ptr<Device>& _device) {
			if (!_json.contains("descriptorSetLayout"))
				throw Exception("Pipeline description file does not contain key: descriptorSetLayout");

			std::vector<std::vector<vk::DescriptorSetLayoutBinding>> sets;
			for (auto& setJs : _json["descriptorSetLayout"]) {
				uint32_t bdIndex = 0;
				sets.emplace_back();
				for (auto& bindingJs : setJs) {
					vk::DescriptorSetLayoutBinding binding;
					binding.binding = bdIndex++;
					binding.descriptorType = GetVkDescriptorTypeFromStr(bindingJs["type"].get<std::string>());
					binding.descriptorCount = bindingJs["count"].get<uint32_t>();
					binding.stageFlags = GetVkShaderStageFromStr(bindingJs["stage"].get<std::string>());
					sets.back().push_back(binding);
				}
			}

			std::vector<DescriptorSetLayout> descriptorSetLayouts;
			for (auto& set : sets) {
				auto setLayout = DescriptorSetLayout(_device);
				setLayout.addBindings(set);
				setLayout.create();
				descriptorSetLayouts.push_back(std::move(setLayout));
			}

			_factory.setDescriptorSetLayout(descriptorSetLayouts);
		}

		vk::ShaderStageFlagBits PipelineFactory::GetVkShaderStageFromStr(const std::string& _str) {
			if (_str == "vert")return vk::ShaderStageFlagBits::eVertex;
			if (_str == "frag")return vk::ShaderStageFlagBits::eFragment;
			if (_str == "tesc")return vk::ShaderStageFlagBits::eTessellationControl;
			if (_str == "tese")return vk::ShaderStageFlagBits::eTessellationEvaluation;
			return vk::ShaderStageFlagBits::eGeometry;
		}

		vk::DescriptorType PipelineFactory::GetVkDescriptorTypeFromStr(const std::string& _str) {
			if (_str == "Sampler")return vk::DescriptorType::eSampler;
			if (_str == "CombinedImageSampler")return vk::DescriptorType::eCombinedImageSampler;
			if (_str == "SampledImage")return vk::DescriptorType::eSampledImage;
			if (_str == "StorageImage")return vk::DescriptorType::eStorageImage;
			if (_str == "UniformTexelBuffer")return vk::DescriptorType::eUniformTexelBuffer;
			if (_str == "StorageTexelBuffer")return vk::DescriptorType::eStorageTexelBuffer;
			if (_str == "UniformBuffer")return vk::DescriptorType::eUniformBuffer;
			if (_str == "StorageBuffer")return vk::DescriptorType::eStorageBuffer;
			if (_str == "UniformBufferDynamic")return vk::DescriptorType::eUniformBufferDynamic;
			if (_str == "StorageBufferDynamic")return vk::DescriptorType::eStorageBufferDynamic;
			return vk::DescriptorType::eInputAttachment;
		}

		vk::DynamicState PipelineFactory::GetVkDynamicStateFromStr(const std::string& _str) {
			if (_str == "Viewport")return vk::DynamicState::eViewport;
			if (_str == "Scissor")return vk::DynamicState::eScissor;
			if (_str == "LineWidth")return vk::DynamicState::eLineWidth;
			if (_str == "DepthBias")return vk::DynamicState::eDepthBias;
			if (_str == "BlendConstants")return vk::DynamicState::eBlendConstants;
			if (_str == "DepthBounds")return vk::DynamicState::eDepthBounds;
			if (_str == "StencilCompareMask")return vk::DynamicState::eStencilCompareMask;
			if (_str == "StencilWriteMask")return vk::DynamicState::eStencilWriteMask;
			return vk::DynamicState::eStencilReference;
		}

		vk::BlendOp PipelineFactory::GetVkBlendOpFromStr(const std::string& _str) {
			if (_str == "Add")return vk::BlendOp::eAdd;
			if (_str == "Subtract")return vk::BlendOp::eSubtract;
			if (_str == "ReverseSubtract")return vk::BlendOp::eReverseSubtract;
			if (_str == "Min")return vk::BlendOp::eMin;
			return vk::BlendOp::eMax;
		}

		vk::BlendFactor PipelineFactory::GetVkBlendFactorFromStr(const std::string& _str) {
			if (_str == "Zero")return vk::BlendFactor::eZero;
			if (_str == "One")return vk::BlendFactor::eOne;
			if (_str == "SrcColor")return vk::BlendFactor::eSrcColor;
			if (_str == "OneMinusSrcColor")return vk::BlendFactor::eOneMinusSrcColor;
			if (_str == "DstColor")return vk::BlendFactor::eDstColor;
			if (_str == "OneMinusDstColor")return vk::BlendFactor::eOneMinusDstColor;
			if (_str == "SrcAlpha")return vk::BlendFactor::eSrcAlpha;
			if (_str == "OneMinusSrcAlpha")return vk::BlendFactor::eOneMinusSrcAlpha;
			if (_str == "DstAlpha")return vk::BlendFactor::eDstAlpha;
			if (_str == "OneMinusDstAlpha")return vk::BlendFactor::eOneMinusDstAlpha;
			if (_str == "ConstantColor")return vk::BlendFactor::eConstantColor;
			if (_str == "OneMinusConstantColor")return vk::BlendFactor::eOneMinusConstantColor;
			if (_str == "ConstantAlpha")return vk::BlendFactor::eConstantAlpha;
			if (_str == "OneMinusConstantAlpha")return vk::BlendFactor::eOneMinusConstantAlpha;
			if (_str == "SrcAlphaSaturate")return vk::BlendFactor::eSrcAlphaSaturate;
			if (_str == "Src1Color")return vk::BlendFactor::eSrc1Color;
			if (_str == "OneMinusSrc1Color")return vk::BlendFactor::eOneMinusSrc1Color;
			if (_str == "Src1Alpha")return vk::BlendFactor::eSrc1Alpha;
			return vk::BlendFactor::eOneMinusSrc1Alpha;
		}

		vk::LogicOp PipelineFactory::GetVkLogicOpFromStr(const std::string& _str) {
			if (_str == "Clear")return vk::LogicOp::eClear;
			if (_str == "And")return vk::LogicOp::eAnd;
			if (_str == "AndReverse")return vk::LogicOp::eAndReverse;
			if (_str == "Copy")return vk::LogicOp::eCopy;
			if (_str == "AndInverted")return vk::LogicOp::eAndInverted;
			if (_str == "NoOp")return vk::LogicOp::eNoOp;
			if (_str == "Xor")return vk::LogicOp::eXor;
			if (_str == "Or")return vk::LogicOp::eOr;
			if (_str == "Nor")return vk::LogicOp::eNor;
			if (_str == "Equivalent")return vk::LogicOp::eEquivalent;
			if (_str == "Invert")return vk::LogicOp::eInvert;
			if (_str == "OrReverse")return vk::LogicOp::eOrReverse;
			if (_str == "CopyInverted")return vk::LogicOp::eCopyInverted;
			if (_str == "OrInverted")return vk::LogicOp::eOrInverted;
			if (_str == "Nand")return vk::LogicOp::eNand;
			return vk::LogicOp::eSet;
		}

		vk::Format PipelineFactory::GetVkFormatFromStr(const std::string& _str) {
			if (_str == "R32_Sfloat")	return vk::Format::eR32Sfloat;
			if (_str == "R32G32_Sfloat")	return vk::Format::eR32G32Sfloat;
			if (_str == "R32G32B32_Sfloat")	return vk::Format::eR32G32B32Sfloat;
			if (_str == "R32G32B32A32_Sfloat")	return vk::Format::eR32G32B32A32Sfloat;
			if (_str == "R32_Int")	return vk::Format::eR32Sint;
			if (_str == "R32G32_Int")	return vk::Format::eR32G32Sint;
			if (_str == "R32G32B32_Int")	return vk::Format::eR32G32B32Sfloat;
			if (_str == "R32G32B32A32_Int")	return vk::Format::eR32G32B32A32Sfloat;
			if (_str == "R32_Uint")	return vk::Format::eR32Uint;
			if (_str == "R32G32_Uint")	return vk::Format::eR32G32Uint;
			if (_str == "R32G32B32_Uint")	return vk::Format::eR32G32B32Uint;
			if (_str == "R32G32B32A32_Uint")	return vk::Format::eR32G32B32A32Uint;
			if (_str == "R8_Unorm")	return vk::Format::eR8Unorm;
			if (_str == "R8G8_Unorm")	return vk::Format::eR8G8Unorm;
			if (_str == "R8G8B8_Unorm")	return vk::Format::eR8G8B8Unorm;
			if (_str == "R8G8B8A8_Unorm")	return vk::Format::eR8G8B8A8Unorm;
			return vk::Format::eUndefined;
		}

		vk::CompareOp PipelineFactory::GetVkCompareOpFromStr(const std::string& _str) {
			if (_str == "Never") return vk::CompareOp::eNever;
			if (_str == "Less") return vk::CompareOp::eLess;
			if (_str == "Equal") return vk::CompareOp::eEqual;
			if (_str == "LessOrEqual") return vk::CompareOp::eLessOrEqual;
			if (_str == "Greater") return vk::CompareOp::eGreater;
			if (_str == "NotEqual") return vk::CompareOp::eNotEqual;
			if (_str == "GreaterOrEqual") return vk::CompareOp::eGreaterOrEqual;
			return vk::CompareOp::eAlways;
		}

		vk::StencilOp PipelineFactory::GetVkStencilOpFromStr(const std::string& _str) {
			if (_str == "Keep")return vk::StencilOp::eKeep;
			if (_str == "Zero")return vk::StencilOp::eZero;
			if (_str == "Replace")return vk::StencilOp::eReplace;
			if (_str == "IncrementAndClamp")return vk::StencilOp::eIncrementAndClamp;
			if (_str == "DescrementAndClamp")return vk::StencilOp::eDecrementAndClamp;
			if (_str == "Invert")return vk::StencilOp::eInvert;
			if (_str == "IncrementAndWrap")return vk::StencilOp::eIncrementAndWrap;
			return vk::StencilOp::eDecrementAndWrap;
		}

		std::shared_ptr<Pipeline> PipelineFactory::CreatePipelineFromJson(const std::shared_ptr<RenderPass>& _renderPass,
																		  const uint32_t _subpassIndex,
																		  const nlohmann::json& _json,
																		  ArrayProxy<const vk::Viewport> _viewports,
																		  ArrayProxy<const vk::Rect2D> _scissors) {
			PipelineFactory factory;
			factory.begin();
			auto shaders = std::move(CreateShaderStageFromJson(_json, _renderPass->getDevice()));
			for (auto& shader : shaders)
				factory.setShader(shader);
			factory.setViewport(_viewports);
			factory.setScissor(_scissors);

			CreateVertexInputFromJson(_json, factory);
			CreateInputAssemblyFromJson(_json, factory);
			CreateRasterizationFromJson(_json, factory);
			/*factory.setRasterization(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
									 vk::FrontFace::eCounterClockwise);*/
			CreateDepthStencilFromJson(_json, factory);
			CreateBlendFromJson(_json, factory);
			CreateDynamicStateFromJson(_json, factory);
			CreatePushConstantFromJson(_json, factory);
			CreateDescriptorSetLayoutFromJson(_json, factory, _renderPass->getDevice());

			return factory.createPipeline(_renderPass, _subpassIndex);
		}

		void PipelineFactory::begin() {
			mPipelineStates.reset(new PipelineStates);
		}

		void PipelineFactory::setShader(const Shader& _shader, const vk::SpecializationInfo* _specInfo) const {
			if (mPipelineStates) {
				mPipelineStates->shaders[_shader.stage()] = { {}, _shader.stage(), _shader.get(), sDefaultEntry, _specInfo };
			}
		}

		void PipelineFactory::setVertexInput(ArrayProxy<const vk::VertexInputBindingDescription> _bindingDescri,
											 ArrayProxy<const vk::VertexInputAttributeDescription> _attriDescri) const {
			if (mPipelineStates) {
				mPipelineStates->inputBindings.assign(_bindingDescri.begin(), _bindingDescri.end());
				mPipelineStates->inputAttributes.assign(_attriDescri.begin(), _attriDescri.end());
			}
		}

		void PipelineFactory::setInputAssembly(const vk::PrimitiveTopology _topology,
											   const bool _primitiveRestart) const {
			if (mPipelineStates) {
				mPipelineStates->inputAssembly.topology = _topology;
				mPipelineStates->inputAssembly.primitiveRestartEnable = _primitiveRestart;
			}
		}

		void PipelineFactory::setViewport(ArrayProxy<const vk::Viewport> _viewports) const {
			if (mPipelineStates)
				mPipelineStates->viewports.assign(_viewports.begin(), _viewports.end());
		}

		void PipelineFactory::setScissor(ArrayProxy<const vk::Rect2D> _scissors) const {
			if (mPipelineStates)
				mPipelineStates->scissors.assign(_scissors.begin(), _scissors.end());
		}

		void PipelineFactory::setRasterization(const vk::PolygonMode _polygonMode,
											   const vk::CullModeFlags _cullMode,
											   const vk::FrontFace _frontFace,
											   const float _lineWidth,
											   const bool _depthClampEnable,
											   const bool _rasterizerDiscardEnable) const {
			if (mPipelineStates) {

				mPipelineStates->rasterization.polygonMode = _polygonMode;
				mPipelineStates->rasterization.cullMode = _cullMode;
				mPipelineStates->rasterization.frontFace = _frontFace;
				mPipelineStates->rasterization.lineWidth = _lineWidth;
				mPipelineStates->rasterization.depthClampEnable = _depthClampEnable;
				mPipelineStates->rasterization.rasterizerDiscardEnable = _rasterizerDiscardEnable;
			}
		}

		void PipelineFactory::setDepthBias(const bool _enable,
										   const float _constantFactor,
										   const float _slopeFactor,
										   const float _clamp) const {
			if (mPipelineStates) {
				mPipelineStates->rasterization.depthBiasEnable = _enable;
				mPipelineStates->rasterization.depthBiasConstantFactor = _constantFactor;
				mPipelineStates->rasterization.depthBiasSlopeFactor = _slopeFactor;
				mPipelineStates->rasterization.depthBiasClamp = _clamp;
			}
		}

		void PipelineFactory::setMultiSample(const vk::SampleCountFlagBits _samples,
											 const bool _sampleShading,
											 const float _minSampleShading,
											 const vk::SampleMask* _sampleMask,
											 const bool _alphaToCoverageEnable,
											 const bool _alphaToOneEnable) const {
			if (mPipelineStates) {
				mPipelineStates->multisample.sampleShadingEnable = _sampleShading;
				mPipelineStates->multisample.rasterizationSamples = _samples;
				mPipelineStates->multisample.minSampleShading = _minSampleShading;
				mPipelineStates->multisample.pSampleMask = _sampleMask;
				mPipelineStates->multisample.alphaToCoverageEnable = _alphaToCoverageEnable;
				mPipelineStates->multisample.alphaToOneEnable = _alphaToOneEnable;
			}
		}

		void PipelineFactory::setDepthTest(const bool _depthTestEnable,
										   const bool _depthWriteEnable,
										   const vk::CompareOp _depthCompareOp) const {
			if (mPipelineStates) {
				mPipelineStates->depthStencil.depthTestEnable = _depthTestEnable;
				mPipelineStates->depthStencil.depthWriteEnable = _depthWriteEnable;
				mPipelineStates->depthStencil.depthCompareOp = _depthCompareOp;
			}
		}

		void PipelineFactory::setDepthBoundsTest(const bool _enable,
												 const float _minBounds,
												 const float _maxBounds) const {
			if (mPipelineStates) {
				mPipelineStates->depthStencil.depthBoundsTestEnable = _enable;
				mPipelineStates->depthStencil.minDepthBounds = _minBounds;
				mPipelineStates->depthStencil.maxDepthBounds = _maxBounds;
			}
		}

		void PipelineFactory::setStencilTest(const bool _enable,
											 const vk::StencilOpState& _front,
											 const vk::StencilOpState& _back) const {
			if (mPipelineStates) {
				mPipelineStates->depthStencil.stencilTestEnable = _enable;
				mPipelineStates->depthStencil.front = _front;
				mPipelineStates->depthStencil.back = _back;
			}
		}

		void PipelineFactory::addDefaultBlendAttachment() const {
			if (mPipelineStates) {
				mPipelineStates->colorBlendAttachments.push_back(DefaultBlendAttachment);
			}
		}

		void PipelineFactory::setBlendAttachments(ArrayProxy<const vk::PipelineColorBlendAttachmentState> _attachments) const {
			if (mPipelineStates) {
				mPipelineStates->colorBlendAttachments.assign(_attachments.begin(), _attachments.end());
			}
		}

		void PipelineFactory::setBlend(const bool _logicalOpEnable,
									   const vk::LogicOp _logicOp,
									   const float _constantR,
									   const float _constantG,
									   const float _constantB,
									   const float _constantA) const {
			if (mPipelineStates) {
				mPipelineStates->colorBlend.logicOpEnable = _logicalOpEnable;
				mPipelineStates->colorBlend.logicOp = _logicOp;
				mPipelineStates->colorBlend.blendConstants[0] = _constantR;
				mPipelineStates->colorBlend.blendConstants[1] = _constantG;
				mPipelineStates->colorBlend.blendConstants[2] = _constantB;
				mPipelineStates->colorBlend.blendConstants[3] = _constantA;
			}
		}

		void PipelineFactory::setDynamicState(ArrayProxy<const vk::DynamicState> _dynamicStates) const {
			if (mPipelineStates)
				mPipelineStates->dynamicStates.assign(_dynamicStates.begin(), _dynamicStates.end());
		}

		void PipelineFactory::setDescriptorSetLayout(ArrayProxy<DescriptorSetLayout> _setLayouts) const {
			if (mPipelineStates)
				mPipelineStates->descriptorSetLayouts.assign(_setLayouts.begin(), _setLayouts.end());
		}

		void PipelineFactory::setPushConstantRanges(ArrayProxy<const vk::PushConstantRange> _ranges) const {
			if (mPipelineStates)
				mPipelineStates->pushConstantRanges.assign(_ranges.begin(), _ranges.end());
		}

		std::shared_ptr<Pipeline> PipelineFactory::createPipeline(const std::shared_ptr<RenderPass>& _renderPass,
																  const uint32_t _subpassIndex) {
			// create pipeline layout
			std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
			descriptorSetLayouts.reserve(mPipelineStates->descriptorSetLayouts.size());
			for (auto& layout : mPipelineStates->descriptorSetLayouts)
				descriptorSetLayouts.emplace_back(layout.get());

			vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
			layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
			layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			layoutCreateInfo.pPushConstantRanges = mPipelineStates->pushConstantRanges.data();
			layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(mPipelineStates->pushConstantRanges.size());

			auto pipelineLayout = _renderPass->getDevice()->get().createPipelineLayout(layoutCreateInfo);

			vk::PipelineViewportStateCreateInfo viewportState = {};
			viewportState.pViewports = mPipelineStates->viewports.data();
			viewportState.viewportCount = static_cast<uint32_t>(mPipelineStates->viewports.size());
			viewportState.pScissors = mPipelineStates->scissors.data();
			viewportState.scissorCount = static_cast<uint32_t>(mPipelineStates->scissors.size());

			if (mPipelineStates->colorBlendAttachments.empty())
				addDefaultBlendAttachment();

			mPipelineStates->colorBlend.pAttachments = mPipelineStates->colorBlendAttachments.data();
			mPipelineStates->colorBlend.attachmentCount = static_cast<uint32_t>(mPipelineStates->colorBlendAttachments.size());

			vk::PipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.pDynamicStates = mPipelineStates->dynamicStates.data();
			dynamicState.dynamicStateCount = static_cast<uint32_t>(mPipelineStates->dynamicStates.size());

			std::vector<vk::PipelineShaderStageCreateInfo> stages;
			stages.reserve(mPipelineStates->shaders.size());
			for (auto& pair : mPipelineStates->shaders) {
				stages.emplace_back(pair.second);
			}

			vk::PipelineVertexInputStateCreateInfo inputState;
			inputState.pVertexBindingDescriptions = mPipelineStates->inputBindings.data();
			inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(mPipelineStates->inputBindings.size());
			inputState.pVertexAttributeDescriptions = mPipelineStates->inputAttributes.data();
			inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(mPipelineStates->inputAttributes.size());

			vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
			pipelineCreateInfo.pStages = stages.data();
			pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages.size());
			pipelineCreateInfo.pVertexInputState = &inputState;
			pipelineCreateInfo.pInputAssemblyState = &mPipelineStates->inputAssembly;
			pipelineCreateInfo.pViewportState = &viewportState;
			pipelineCreateInfo.pRasterizationState = &mPipelineStates->rasterization;
			pipelineCreateInfo.pMultisampleState = &mPipelineStates->multisample;
			pipelineCreateInfo.pDepthStencilState = &mPipelineStates->depthStencil;
			pipelineCreateInfo.pColorBlendState = &mPipelineStates->colorBlend;

			if (dynamicState.dynamicStateCount == 0)
				pipelineCreateInfo.pDynamicState = nullptr;
			else
				pipelineCreateInfo.pDynamicState = &dynamicState;

			pipelineCreateInfo.layout = pipelineLayout;
			pipelineCreateInfo.renderPass = _renderPass->get();
			pipelineCreateInfo.subpass = _subpassIndex;
			pipelineCreateInfo.basePipelineHandle = nullptr;	//基础管线（vulkan允许在已经存在的管线上派生新的管线）
			pipelineCreateInfo.basePipelineIndex = -1;

			auto pipeline = _renderPass->getDevice()->get().createGraphicsPipeline(nullptr, pipelineCreateInfo);

			return std::shared_ptr<Pipeline>(new Pipeline(_renderPass, mPipelineStates->descriptorSetLayouts, _subpassIndex, pipeline, pipelineLayout));
		}

		void PipelineFactory::clear() {
			mPipelineStates.reset();
		}

	}
}
