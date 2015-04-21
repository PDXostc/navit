#include "speechimpl.h"
#include "log.h"

namespace NXE {

struct SpeechImplPrivate {};

SpeechImpl::SpeechImpl()
{
}

SpeechImpl::~SpeechImpl()
{
}

void SpeechImpl::say(const std::string& command)
{
    nInfo() << "Trying to say " << command;
}

} // namespace NXE
