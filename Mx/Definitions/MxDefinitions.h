#pragma once

#ifndef MX_DEFINITIONS_H_
#define MX_DEFINITIONS_H_

#ifndef MX_ENGINE_NAME
#define MX_ENGINE_NAME "MixEngine"
#endif

#ifndef MX_ENGINE_VERSION_MAJOR
#define MX_ENGINE_VERSION_MAJOR 0
#endif

#ifndef MX_ENGINE_VERSION_MINOR
#define MX_ENGINE_VERSION_MINOR 0
#endif

#ifndef MX_ENGINE_VERSION_PATCH
#define MX_ENGINE_VERSION_PATCH 1
#endif

#ifndef MX_DEBUG_MODE
#   if defined(_DEBUG)||defined(DEBUG)
#       define MX_DEBUG_MODE 1
#   else
#       define MX_DEBUG_MODE 0
#   endif
#endif

#ifndef MX_ASSERT
#if MX_DEBUG_MODE
#   include <cassert>
#   define MX_ASSERT assert
#else
#   define MX_ASSERT(expr) ((void)0)
#endif
#endif

namespace Mix {
    class Scene;
    class SceneManager;

    class SceneObject;
    class GameObject;
    class Component;
    class Behaviour;
    class AudioListener;
    class AudioSource;
    class MeshFilter;
    class RigidBody;
    class Transform;
    class Camera;
    class Renderer;

    template<typename _Ty>
    class SceneObjectHandle;

    using HSceneObject = SceneObjectHandle<SceneObject>;
    using HGameObject = SceneObjectHandle<GameObject>;
    using HComponent = SceneObjectHandle<Component>;
    using HBehaviour = SceneObjectHandle<Behaviour>;
    using HRenderer = SceneObjectHandle<Renderer>;
    using HAdudioListener = SceneObjectHandle<AudioListener>;
    using HAudioSource = SceneObjectHandle<AudioSource>;
    using HMeshFilter = SceneObjectHandle<MeshFilter>;
    using HRigidBody = SceneObjectHandle<RigidBody>;
    using HTransform = SceneObjectHandle<Transform>;
    using HCamera = SceneObjectHandle<Camera>;

}

#endif
