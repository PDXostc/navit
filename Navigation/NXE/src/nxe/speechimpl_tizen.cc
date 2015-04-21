#include "speechimpl.h"
#include "log.h"
#include <tts.h>

#include <functional>

namespace NXE {

void state_changed_cb(tts_h tts, tts_state_e previous, tts_state_e current, void* user_data);

struct SpeechImplPrivate {
    tts_h handle;
    bool usable{ false };
    std::string defaultVoice;
    tts_voice_type_e voiceType;

    void refreshVoice() {
        std::string buff;
        buff.reserve(4);
        char* b = const_cast<char*>(buff.data());
        tts_get_default_voice(handle, &b, &voiceType);
    }

    std::function<void(tts_state_e, tts_state_e)> stateCb = { [](tts_state_e previous, tts_state_e current) {
        nInfo() << "TTS state changed. Prev= " << previous << " current= " << current;
    } };
};

SpeechImpl::SpeechImpl()
    : d(new SpeechImplPrivate)
{
    int ret = tts_create(&(d->handle));

    if (ret != TTS_ERROR_NONE) {
        nFatal() << "Unable to create TTS service";
    }
    else {
        ret = tts_prepare(d->handle);
        if (ret != TTS_ERROR_NONE) {
            nFatal() << "Unable to prepare TTS service. ec=" << ret;
        }
        else {
            d->usable = true;
        }
    }

    if (tts_set_state_changed_cb(d->handle, state_changed_cb, d.get())) {
        nError() << "Unable to set state changed callback";
    }
}

SpeechImpl::~SpeechImpl()
{
    if (d->usable) {
        tts_destroy(d->handle);
    }
}

void SpeechImpl::say(const std::string& command)
{
    if (!d->usable) {
        nFatal() << "Tizen TTS is not properly initialized";
        return;
    }
    nInfo() << "Trying to say " << command;
    d->refreshVoice();

    int utt_id;
    int ret = tts_add_text(d->handle, command.c_str(), d->defaultVoice.c_str(), static_cast<tts_voice_type_e>(d->voiceType), TTS_SPEED_AUTO, &utt_id);

    if (TTS_ERROR_NONE != ret) {
        nError() << "Unable to add text " << command;
        return;
    }

    ret = tts_play(d->handle);
    if (TTS_ERROR_NONE != ret) {
        nError() << "Tizen TTS cannot say text " << command;
    }
}

/// callbacks

void state_changed_cb(tts_h tts, tts_state_e previous, tts_state_e current, void* user_data)
{
    SpeechImplPrivate* d = reinterpret_cast<SpeechImplPrivate*>(user_data);
    d->stateCb(previous, current);
}

} // namespace NXE
