#pragma once
#ifndef MX_VK_RENDER_H_
#define MX_VK_RENDER_H_

#include "../../Math/MxMatrix4.h"

namespace Mix {
	namespace Vulkan {
		namespace Uniform {
			struct AbstractUniform {
			protected:
				AbstractUniform() = default;
			};

			struct AbstractLightUniform :public AbstractUniform {
				Math::Vector4f color;
			protected:
				AbstractLightUniform() :color(0.0f, 0.0f, 0.0f, 1.0f) {}
			};

			struct AbstractCameraUniform :public AbstractUniform {
				union {
					Math::Vector4f _vec4_1_;
					Math::Vector3f position;
				};

				union {
					Math::Vector4f _vec4_2_;
					Math::Vector3f forward;
				};

				Math::Matrix4 viewMat;
				Math::Matrix4 projMat;
			protected:
				AbstractCameraUniform() :
					position(0),
					forward(Math::Vector3f::Forward),
					viewMat(Math::Matrix4::Identity),
					projMat(Math::Matrix4::Identity) {
				}
			};

			struct MeshUniform :public AbstractUniform {
				Math::Matrix4 modelMat;

				MeshUniform() :
					modelMat(Math::Matrix4::Identity) {
				}
			};

			struct DirectionLightUniform :public AbstractLightUniform {
				union {
					Math::Vector4f _vec4_1_;
					Math::Vector3f direction;
				};

				DirectionLightUniform() :
					direction(Math::Vector3f::Forward) {
				}
			};

			struct PointLightUniform :public AbstractLightUniform {
				float constant;
				Math::Vector3f position;
				float linear;
				float quadratic;

				PointLightUniform() :
					constant(0),
					position(0),
					linear(0),
					quadratic(0) {
				}
			};

			struct SpotLightUniform :public AbstractLightUniform {
				Math::Vector3f position;
				float innerCutoff;
				Math::Vector3f direction;
				float outerCutoff;
				float constant;
				float linear;
				float quadratic;

				SpotLightUniform() :
					position(0),
					innerCutoff(0),
					direction(Math::Vector3f::Forward),
					outerCutoff(0),
					constant(0),
					linear(0),
					quadratic(0) {
				}
			};

			struct CameraUniform :public AbstractCameraUniform {
				CameraUniform() = default;
			};
		}
	}
}

#endif // !MX_VK_RENDER_H_
