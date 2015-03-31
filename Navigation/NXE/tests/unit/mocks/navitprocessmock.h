#ifndef NAVITPROCESSMOCK
#define NAVITPROCESSMOCK

#include "inavitprocess.h"
#include <gmock/gmock.h>

struct NavitProcessMock : public NXE::INavitProcess
{
    using Inject = NavitProcessMock();
    MOCK_METHOD1(setProgramPath, void(const std::string&));
    MOCK_METHOD0(start, bool());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD0(isRunning, bool());
    MOCK_METHOD1(setArgs, void(const std::list<std::string> &));
};

#endif // NAVITPROCESSMOCK
