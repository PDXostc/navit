#ifndef NAVITPROCESSMOCK
#define NAVITPROCESSMOCK

#include "inavitprocess.h"
#include <gmock/gmock.h>

struct NavitProcessMock : public NXE::INavitProcess {
    using Inject = NavitProcessMock();
    MOCK_METHOD0(start, bool());
    MOCK_METHOD0(stop, void());
    MOCK_METHOD0(isRunning, bool());
    MOCK_METHOD1(setSocketName, void(const std::string& ));
};

#endif // NAVITPROCESSMOCK
