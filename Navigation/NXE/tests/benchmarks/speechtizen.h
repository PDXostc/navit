#ifndef NXE_SPEECHTIZEN_H
#define NXE_SPEECHTIZEN_H

#include "ispeech.h"
#include <fruit/macro.h>

namespace NXE {

class SpeechTizen : public NXE::ISpeech
{
public:
    INJECT(SpeechTizen());
    ~SpeechTizen();
};

} // namespace NXE

#endif // NXE_SPEECHTIZEN_H
