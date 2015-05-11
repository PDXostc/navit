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

using ::testing::StrictMock;
const std::string navitPath{ NAVIT_PATH };

struct NavitInstanceTest : public ::testing::Test {

    NXE::DI::Injector injector{ std::make_tuple(
                    std::shared_ptr<NXE::INavitIPC>(new NavitIPCMock),
                    std::shared_ptr<NXE::INavitProcess>(new NavitProcessMock),
                    std::shared_ptr<NXE::IGPSProvider>(new GPSMock),
                    std::shared_ptr<NXE::IMapDownloader>(new MapDownloaderMock),
                    std::shared_ptr<NXE::ISpeech>(new SpeechMock)
                    )};

    NavitProcessMock* mock_process{ (dynamic_cast<NavitProcessMock*>(NXE::get<std::shared_ptr<NXE::INavitProcess>>(injector).get())) };
    NavitIPCMock* mock_ipc{ (dynamic_cast<NavitIPCMock*>(NXE::get<std::shared_ptr<NXE::INavitIPC>>(injector).get())) };
    MapDownloaderMock* mock_mapd{ (dynamic_cast<MapDownloaderMock*>(NXE::get<std::shared_ptr<NXE::IMapDownloader>>(injector).get())) };

    NXE::INavitIPC::SpeechSignalType speechS;

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
    int zoom = instance.HandleMessage<ZoomMessageTag>();
    EXPECT_EQ(zoom, 10);
}
