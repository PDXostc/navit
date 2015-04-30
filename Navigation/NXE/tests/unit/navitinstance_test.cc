#include <gtest/gtest.h>

#include "mocks/navitprocessmock.h"
#include "mocks/navitcontrollermock.h"
#include "mocks/gpsmock.h"
#include "mocks/mapdownloadermock.h"
#include "mocks/speechmock.h"
#include "nxe_instance.h"
#include "inavitipc.h"
#include "settingtags.h"
#include "../testutils.h"
#include "log.h"

#include <fruit/injector.h>

using ::testing::StrictMock;
const std::string navitPath{ NAVIT_PATH };

struct NavitInstanceTest : public ::testing::Test {

    NXE::DI::Injector injector{ []() -> NXE::DI::Components {
        return fruit::createComponent()
                .bind<NXE::INavitIPC, NavitIPCMock>()
                .bind<NXE::INavitProcess, NavitProcessMock>()
                .bind<NXE::IGPSProvider, GPSMock>()
                .bind<NXE::IMapDownloader, MapDownloaderMock>()
                .bind<NXE::ISpeech, SpeechMock>();
    }() };

    NavitProcessMock* mock_process{ (dynamic_cast<NavitProcessMock*>(injector.get<NXE::INavitProcess*>())) };
    NavitIPCMock* mock_ipc{ (dynamic_cast<NavitIPCMock*>(injector.get<NXE::INavitIPC*>())) };
    MapDownloaderMock* mock_mapd{ (dynamic_cast<MapDownloaderMock*>(injector.get<NXE::IMapDownloader*>())) };

    NXE::INavitIPC::SpeechSignal speechS;
    NXE::INavitIPC::InitializedSignal initS;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void setupMocks()
    {
        using ::testing::Return;
        using ::testing::ReturnRef;
        EXPECT_CALL(*mock_process, start()).WillRepeatedly(Return(true));
        EXPECT_CALL(*mock_process, stop());
        EXPECT_CALL(*mock_ipc, speechSignal()).WillRepeatedly(ReturnRef(speechS));
        EXPECT_CALL(*mock_ipc, initializedSignal()).WillRepeatedly(ReturnRef(initS));
    }
};

TEST_F(NavitInstanceTest, zoomBy_proper)
{
    using ::testing::Return;

    // Arrange
    setupMocks();
    EXPECT_CALL(*mock_ipc, zoom()).WillOnce(Return(10));

    NXE::NXEInstance instance{ injector };
    instance.Initialize();

    // Act
//    EXPECT_NO_THROW(instance.HandleMessage(TestUtils::zoomMessage()));
    int zoom = instance.HandleMessage<ZoomMessageTag>();
    EXPECT_EQ(zoom, 10);
}
