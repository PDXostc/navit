#ifndef NXE_SPEECHNULL_H
#define NXE_SPEECHNULL_H

#include "ispeech.h"
#include <fruit/macro.h>
#include <memory>

namespace NXE {

struct SpeechImplPrivate;
class SpeechImpl : public NXE::ISpeech
{
public:
    INJECT(SpeechImpl());
    ~SpeechImpl();

    void say(const std::string& command) override;
private:
    std::unique_ptr<SpeechImplPrivate> d;

};

} // namespace NXE

#endif // NXE_SPEECHNULL_H
