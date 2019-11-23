#pragma once

#ifndef MX_AUDIO_SOURCE_H_
#define MX_AUDIO_SOURCE_H_

#include "../Behaviour/MxBehaviour.h"
#include "../../Math/MxVector3.h"

namespace FMOD {
    class Channel;
}


namespace Mix {
    struct FMODChannelParam {
        bool mute;
        bool paused;
        float volume;
        bool volumRamp;
        float pitch;
        Vector2f distance;
        float level;
        float Frequency;
        int Priority;

        void importChannel(FMOD::Channel& _Channel);

        void exportChannel(FMOD::Channel& _Channel);
    };


    class AudioSource :public Component {
        MX_DECLARE_RTTI;

    public:
        void setMute(const bool _mute);

        bool getMute() const;

        bool getPaused() const;

        void setPaused(const bool _paused);

        float getVolume() const;

        void setVolume(const float _volume);

        bool getVolumeRamp() const;

        void setVolumeRamp(const bool _volumeRamp);

        //todo getAudibility 

        float getPitch() const;

        void setPitch(const float _pitch);
        ;


        Vector2f get3DMinMaxDistance() const;

        void set3DMinMaxDistance(const Vector2f& _distance);
        //todo get3DCustomRolloff

        float get3DDopplerLevel() const;

        void set3DDopplerLevel(const float _level);



        float getFrequency() const;

        void setFrequency(const float _frequency);

        int getPriority() const;

        void setPriority(const int _priority);


        FMOD::Channel* mChannel;
        mutable FMODChannelParam mChannelParam;
    };
}

#endif
