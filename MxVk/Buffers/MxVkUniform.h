#pragma once
#ifndef MX_VK_RENDER_H_
#define MX_VK_RENDER_H_

#include <glm/glm.hpp>

namespace Mix {
    namespace Graphics {
        namespace Uniform {
            struct AbstractUniform {
            protected:
                AbstractUniform() = default;
            };

            struct AbstractLightUniform :public AbstractUniform {
                glm::vec4 color;
            protected:
                AbstractLightUniform() :color() {}
            };

            struct AbstractCameraUniform :public AbstractUniform {
                union {
                    glm::vec4 _vec4_1_;
                    glm::vec3 position;
                };

                union {
                    glm::vec4 _vec4_2_;
                    glm::vec3 forward;
                };

                glm::mat4 viewMat;
                glm::mat4 projMat;
            protected:
                AbstractCameraUniform() = default;
            };

            struct MeshUniform :public AbstractUniform {
                glm::mat4 modelMat;
                glm::mat4 normMat;
            };

            struct DirectionLightUniform :public AbstractLightUniform {
                union {
                    glm::vec4 _vec4_1_;
                    glm::vec3 direction;
                };
            };

            struct PointLightUniform :public AbstractLightUniform {
                float constant;
                glm::vec3 position;
                float linear;
                float quadratic;
            };

            struct SpotLightUniform :public AbstractLightUniform {
                glm::vec3 position;
                float innerCutoff;
                glm::vec3 direction;
                float outerCutoff;
                float constant;
                float linear;
                float quadratic;

            };

            struct CameraUniform :public AbstractCameraUniform {
                CameraUniform() = default;
            };
        }
    }
}

#endif // !MX_VK_RENDER_H_
