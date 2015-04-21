#ifndef ISPEECH_H
#define ISPEECH_H

#include <string>

namespace NXE {

class ISpeech {
public:
    virtual ~ISpeech() {}
    virtual void say(const std::string& command) = 0;
};

}

#endif // ISPEECH_H

