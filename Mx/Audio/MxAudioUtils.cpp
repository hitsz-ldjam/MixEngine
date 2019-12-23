#include "MxAudioUtils.h"
#include <fmod/fmod.hpp>

namespace Mix::Audio {
    void FMODChannelParam::importFrom(FMOD::Channel* _channel) {
        if(!_channel) return;
        _channel->getPitch(&pitch);
        _channel->getFrequency(&frequency);
        _channel->getPriority(&priority);
        _channel->getVolume(&volume);
        _channel->getVolumeRamp(&volumRamp);
        _channel->getMute(&mute);
        _channel->get3DMinMaxDistance(&minMaxDistance.x, &minMaxDistance.y);
        _channel->get3DLevel(&dopplerLevel);
    }

    void FMODChannelParam::exportTo(FMOD::Channel* _channel) {
        if(!_channel) return;
        _channel->setPitch(pitch);
        _channel->setFrequency(frequency);
        _channel->setPriority(priority);
        _channel->setVolume(volume);
        _channel->setVolumeRamp(volumRamp);
        _channel->setMute(mute);
        _channel->set3DMinMaxDistance(minMaxDistance.x, minMaxDistance.y);
        _channel->set3DLevel(dopplerLevel);
    }
}
