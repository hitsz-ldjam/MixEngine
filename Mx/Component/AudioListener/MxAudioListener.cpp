#include "MxAudioListener.h"

#include "../../Manager/AudioManager/MxAudioManager.h"
#include "../../GameObject/MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioListener, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioListener)

    int AudioListener::sListenerNum = 0;

    AudioListener::AudioListener() {
        auto mgr = FindObjectOfType<AudioManager>();
        if(!mgr || !(mCore = mgr->core()))
            throw AudioManagerNotInitializedError();

        mListenerIdx = sListenerNum++;
        mCore->set3DNumListeners(sListenerNum);
    }

    void AudioListener::update() {
        if(!mGameObject)
            throw IndependentComponentError();

        auto trans = mGameObject->transform();

        FMOD_VECTOR pos = Audio::glmVec3ToFmodVec(trans.position()),
                    vel = {0, 0, 0},
                    forward = Audio::glmVec3ToFmodVec(trans.forward()),
                    up = Audio::glmVec3ToFmodVec(trans.up());

        // todo: set up velocity
        //Rigidbody* rb = mGameObject->getComponent<Rigidbody>();
        //if(rb)
        //    vel = Audio::glmVec3ToFmodVec(rb->velocity());

        mCore->set3DListenerAttributes(mListenerIdx, &pos, &vel, &forward, &up);
    }
}
