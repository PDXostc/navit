#include "benchmark/benchmark.h"

#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "inavitipc.h"
#include "navitdbus.h"
#include "testutils.h"
#include "mocks/navitprocessmock.h"
#include "mocks/navitcontrollermock.h"
#include "mocks/gpsmock.h"
#include "testutils.h"

#include <fruit/component.h>
#include <fruit/injector.h>
#include <gmock/gmock.h>

namespace bpt = boost::property_tree;

struct RenderTest {
    NXE::DI::Injector injector{ []() -> NXE::DI::Components {
        return fruit::createComponent()
                .bind<NXE::INavitIPC, NavitIPCMock>()
                .bind<NXE::INavitProcess, NavitProcessMock>()
                .bind<NXE::IGPSProvider, GPSMock>();
    }() };

    NXE::NXEInstance instance{ injector };
    NavitProcessMock* mock_process{ (dynamic_cast<NavitProcessMock*>(injector.get<NXE::INavitProcess*>())) };
    NavitIPCMock* mock_ipc{ (dynamic_cast<NavitIPCMock*>(injector.get<NXE::INavitIPC*>())) };
    NXE::INavitIPC::SpeechSignal speechS;
    NXE::INavitIPC::InitializedSignal initS;

    void setupMocks()
    {
        using ::testing::ReturnRef;
        EXPECT_CALL(*(mock_process), setProgramPath(::testing::_));
        EXPECT_CALL(*(mock_process), start());
        EXPECT_CALL(*(mock_process), stop());
        EXPECT_CALL(*(mock_ipc), start());
        EXPECT_CALL(*(mock_ipc), speechSignal()).WillOnce(ReturnRef(speechS));
        EXPECT_CALL(*(mock_ipc), initializedSignal()).WillOnce(ReturnRef(initS));
    }
};

void emptyMessageParsing(benchmark::State& state)
{
    RenderTest t;
    t.setupMocks();
    t.instance.Initialize();

    while (state.KeepRunning()) {
        t.instance.HandleMessage("");
    }
}

void parseMoveBy(::benchmark::State& state)
{
    using ::testing::AtLeast;

    RenderTest t;
    t.setupMocks();
    t.instance.Initialize();

    EXPECT_CALL(*(t.mock_ipc), moveBy(10, 10)).Times(AtLeast(state.max_iterations));
    const std::string a{ TestUtils::moveByMessage(10, 10) };

    while (state.KeepRunning()) {
        t.instance.HandleMessage(a.c_str());
    }
}

void zoomBy(::benchmark::State& state)
{
    using ::testing::AtLeast;

    RenderTest t;
    t.setupMocks();
    t.instance.Initialize();

    EXPECT_CALL(*(t.mock_ipc), zoomBy(2)).Times(AtLeast(state.max_iterations));
    const std::string a{ TestUtils::zoomByMessage(2) };

    while (state.KeepRunning()) {
        t.instance.HandleMessage(a.c_str());
    }
}

BENCHMARK(emptyMessageParsing);
BENCHMARK(parseMoveBy);
BENCHMARK(zoomBy);
