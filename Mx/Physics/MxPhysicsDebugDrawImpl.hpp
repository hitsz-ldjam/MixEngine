#pragma once

#ifndef MX_PHYSICS_DEBUG_DRAW_IMPL_H_
#define MX_PHYSICS_DEBUG_DRAW_IMPL_H_

// todo: delete debug code
#include "../Scene/MxSceneManager.h"
#include "../Component/Camera/MxCamera.h"
// -----

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_

#include "../Math/MxMatrix4.h"

#include <SDL2/SDL_video.h>
#include <glad/glad.h>

#include <memory>
//#include <fstream>
//#include <sstream>
#include <vector>
#include <cstdio>

namespace Mix::Physics::DebugDrawImpl {
    namespace glUtils {
        using glID = GLuint;

        struct glObject {
            virtual ~glObject() = default;
            glObject(const glObject&) = default;
            glID id() const noexcept { return mID; }
        protected:
            explicit glObject() : mID(0) {}
            glID mID;
        };

        namespace Shader {
            const GLsizei errMsgLen = 128;

            //inline std::string loadFile(const char* _path) {
            //    std::ifstream ifs(_path);
            //    std::stringstream ss;
            //    ss << ifs.rdbuf();
            //    return ss.str();
            //}

            const char* const vertShader =
                "#version 330 core\n"
                "layout (location = 0) in vec3 in_pos;"
                "layout (location = 1) in vec3 in_color;"
                "out vec3 v_color;"
                "uniform mat4 model;"
                "uniform mat4 view;"
                "uniform mat4 projection;"
                "void main() {"
                "    gl_Position = projection * view * model * vec4(in_pos, 1);"
                "    v_color = in_color;"
                "}";

            const char* const fragShader =
                "#version 330 core\n"
                "in vec3 v_color;"
                "out vec4 out_color;"
                "void main() {"
                "    out_color = vec4(v_color, 1);"
                "}";

            class Source final : public glObject {
            public:
                Source(const char* _code, const GLenum _type) {
                    mID = glCreateShader(_type);
                    glShaderSource(mID, 1, &_code, nullptr);
                }

                ~Source() {
                    glDeleteShader(mID);
                }

                void compile() const {
                    glCompileShader(mID);
                    int success;
                    glGetShaderiv(mID, GL_COMPILE_STATUS, &success);
                    if(!success) {
                        char infoLog[errMsgLen];
                        glGetShaderInfoLog(mID, errMsgLen, nullptr, infoLog);
                        throw std::runtime_error(infoLog);
                    }
                }
            };

            class Program final : public glObject {
            public:
                Program() {
                    mID = glCreateProgram();
                }

                void attachShader(const Source& _shader) const {
                    glAttachShader(mID, _shader.id());
                }

                void link() const {
                    glLinkProgram(mID);
                    int success;
                    glGetProgramiv(mID, GL_LINK_STATUS, &success);
                    if(!success) {
                        char infoLog[errMsgLen];
                        glGetProgramInfoLog(mID, errMsgLen, nullptr, infoLog);
                        throw std::runtime_error(infoLog);
                    }
                }

                void useSelf() const {
                    glUseProgram(mID);
                }

                void setUniform(const char* _uniform, const Matrix4& _value) const {
                    glUniformMatrix4fv(uniformLocation(_uniform), 1, GL_FALSE, &_value[0][0]);
                }

            private:
                glID uniformLocation(const char* _uniform) const {
                    return glGetUniformLocation(mID, _uniform);
                }
            };
        }

        class VertexArray final : public glObject {
        public:
            VertexArray() {
                glGenVertexArrays(1, &mID);
            }

            ~VertexArray() {
                glDeleteVertexArrays(1, &mID);
            }

            void bind() const {
                glBindVertexArray(mID);
            }

            void unbind() const {
                glBindVertexArray(0);
            }
        };

        template<typename _Ty, GLenum _Target>
        class BufferObject final : public glObject {
        public:
            BufferObject() {
                glGenBuffers(1, &mID);
            }

            ~BufferObject() {
                glDeleteBuffers(1, &mID);
            }

            constexpr GLenum bindTarget() const noexcept {
                return _Target;
            }

            void bind() const {
                glBindBuffer(_Target, mID);
            }

            void unbind() const {
                glBindBuffer(_Target, 0);
            }

            void fillData(const std::vector<_Ty>& _data, const GLenum _usage = GL_STATIC_DRAW) const {
                bind();
                glBufferData(_Target, _data.size() * sizeof(_Ty), _data.data(), _usage);
            }

            void fillData(std::vector<_Ty>&& _data, const GLenum _usage = GL_STATIC_DRAW) const {
                bind();
                glBufferData(_Target, _data.size() * sizeof(_Ty), _data.data(), _usage);
            }
        };

        using VertexBuffer = BufferObject<float, GL_ARRAY_BUFFER>;

        inline void linkVertexAttributes(const unsigned _layoutLocation,
                                         const unsigned _numComponents,
                                         const unsigned _stride,
                                         const unsigned _offset) {
            glVertexAttribPointer(_layoutLocation,
                                  _numComponents,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  _stride * sizeof(float),
                                  reinterpret_cast<void*>(_offset * sizeof(float)));
            glEnableVertexAttribArray(_layoutLocation);
        }
    }

    class GLHandler final {
    public:
        GLHandler() : mWindow(nullptr, [](SDL_Window* _w) { if(_w) SDL_DestroyWindow(_w); }),
                      mProgram(nullptr),
                      mVAO(nullptr),
                      mVBO(nullptr) {}

        void pushLine(const Vector3f& _from,
                      const Vector3f& _to,
                      const Vector3f& _color) {
            mNewData.insert(mNewData.end(), std::begin(_from.linear), std::end(_from.linear));
            mNewData.insert(mNewData.end(), std::begin(_color.linear), std::end(_color.linear));
            mNewData.insert(mNewData.end(), std::begin(_to.linear), std::end(_to.linear));
            mNewData.insert(mNewData.end(), std::begin(_color.linear), std::end(_color.linear));
        }

        void awake() {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            mWindow.reset(SDL_CreateWindow("Physics Debug",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           1024,
                                           768,
                                           SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL));

            SDL_GL_CreateContext(mWindow.get());

            gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress));

            {
                int w, h;
                SDL_GL_GetDrawableSize(mWindow.get(), &w, &h);
                glViewport(0, 0, w, h);
            }

            glEnable(GL_DEPTH_TEST);

            puts((char*)glGetString(GL_VERSION));
            puts((char*)glGetString(GL_RENDERER));

            // ----- set up finished -----

            using namespace glUtils;

            mProgram.reset(new Shader::Program);
            {
                Shader::Source vert(Shader::vertShader, GL_VERTEX_SHADER);
                vert.compile();
                Shader::Source frag(Shader::fragShader, GL_FRAGMENT_SHADER);
                frag.compile();
                mProgram->attachShader(vert);
                mProgram->attachShader(frag);
                mProgram->link();
            }

            mVAO.reset(new VertexArray);
            mVBO.reset(new VertexBuffer);
            mVAO->bind();
            mVBO->bind();
            linkVertexAttributes(0, 3, 6, 0); // position
            linkVertexAttributes(1, 3, 6, 3); // color
            mVBO->unbind();
            mVAO->unbind();

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        void render() {
            // keep the previous frame when fixedUpdate() is not called
            if(mNewData.empty()) return;

            glClearColor(.3f, 0, .4f, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mProgram->useSelf();

            // todo: pass from Camera
            static auto camera = SceneManager::Get()->getActiveScene()->getMainCamera();
            {
                mProgram->setUniform("model", Matrix4::Identity);
                mProgram->setUniform("view", camera->getViewMat());
                auto proj = camera->getProjMat();
                //proj[0][0] *= -1.0f;
                //proj[1][1] *= -1.0f;
                mProgram->setUniform("projection", proj);
            }

            mVAO->bind();
            mVBO->fillData(mNewData, GL_DYNAMIC_DRAW);
            //glUtils::linkVertexAttributes(0, 3, 6, 0);
            //glUtils::linkVertexAttributes(1, 3, 6, 3);
            mVBO->unbind();
            glDrawArrays(GL_LINES, 0, mNewData.size() / 6);
            mVAO->unbind();

            mNewData.clear();

            SDL_GL_SwapWindow(mWindow.get());
        }

    private:
        std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> mWindow;
        std::unique_ptr<glUtils::Shader::Program> mProgram;
        std::unique_ptr<glUtils::VertexArray> mVAO;
        std::unique_ptr<glUtils::VertexBuffer> mVBO;

        std::vector<float> mNewData;
    };
}

#endif

#endif
