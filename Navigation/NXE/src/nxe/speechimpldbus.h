#ifndef NXE_SPEECHIMPLDBUS_H
#define NXE_SPEECHIMPLDBUS_H

#include "ispeech.h"
#include <fruit/macro.h>
#include <memory>

namespace NXE {
class DBusController;

struct SpeechImplDBusPrivate;
class SpeechImplDBus : public NXE::ISpeech
{
public:
    INJECT(SpeechImplDBus(DBusController& ctrl));
    ~SpeechImplDBus();

    void say(const std::string& command) override;
private:
    std::unique_ptr<SpeechImplDBusPrivate> d;

};

} // namespace NXE

#endif // NXE_SPEECHIMPLDBUS_H
