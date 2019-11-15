#include "MxAudioListener.h"

#include "../../Audio/MxAudioCore.h"
#include "../../GameObject/MxGameObject.h"
#include "../RigidBody/MxRigidBody.h"

#include <fmod/fmod.hpp>

namespace Mix {
    MX_IMPLEMENT_RTTI(AudioListener, Behaviour);

    void AudioListener::start() {
        mCore = Audio::Core::Get()->getCore();
    }

    void AudioListener::lateUpdate() {
        Vector3f pos = transform()->getPosition(),
                 vel(0),
                 forward = transform()->forward(),
                 up = transform()->up();
        if(auto rb = mGameObject->getComponent<RigidBody>())
            vel = rb->getLinearVelocity();
        mCore->set3DListenerAttributes(mListenerIdx,
                                       reinterpret_cast<FMOD_VECTOR*>(pos.linear),
                                       reinterpret_cast<FMOD_VECTOR*>(vel.linear),
                                       reinterpret_cast<FMOD_VECTOR*>(forward.linear),
                                       reinterpret_cast<FMOD_VECTOR*>(up.linear));
    }
}
