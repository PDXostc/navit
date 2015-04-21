#ifndef SPEECHMOCK_H
#define SPEECHMOCK_H

#include "ispeech.h"
#include <gmock/gmock.h>

struct SpeechMock : public NXE::ISpeech {
    using Inject = SpeechMock();
    MOCK_METHOD1(say, void(const std::string&));
};

#endif // SPEECHMOCK_H

