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
				Vector4f color;
			protected:
				AbstractLightUniform() :color(0.0f, 0.0f, 0.0f, 1.0f) {}
			};

			struct AbstractCameraUniform :public AbstractUniform {
				union {
					Vector4f _vec4_1_;
					Vector3f position;
				};

				union {
					Vector4f _vec4_2_;
					Vector3f forward;
				};

				Matrix4 viewMat;
				Matrix4 projMat;
			protected:
				AbstractCameraUniform() :
					position(0),
					forward(Vector3f::Forward),
					viewMat(Matrix4::Identity),
					projMat(Matrix4::Identity) {
				}
			};

			struct MeshUniform :public AbstractUniform {
				Matrix4 modelMat;

				MeshUniform() :
					modelMat(Matrix4::Identity) {
				}
			};

			struct DirectionLightUniform :public AbstractLightUniform {
				union {
					Vector4f _vec4_1_;
					Vector3f direction;
				};

				DirectionLightUniform() :
					direction(Vector3f::Forward) {
				}
			};

			struct PointLightUniform :public AbstractLightUniform {
				float constant;
				Vector3f position;
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
				Vector3f position;
				float innerCutoff;
				Vector3f direction;
				float outerCutoff;
				float constant;
				float linear;
				float quadratic;

				SpotLightUniform() :
					position(0),
					innerCutoff(0),
					direction(Vector3f::Forward),
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
