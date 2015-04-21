#include <gtest/gtest.h>

#include "mocks/navitprocessmock.h"
#include "mocks/navitcontrollermock.h"
#include "mocks/gpsmock.h"
#include "mocks/mapdownloadermock.h"
#include "mocks/speechmock.h"
#include "nxe_instance.h"
#include "navitcontroller.h"
#include "inavitipc.h"
#include "settingtags.h"
#include "jsonmessage.h"
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

    bool bData = false;
    NXE::JSONMessage response;

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
        EXPECT_CALL(*mock_mapd, setListener(::testing::_));
    }

    void callback(const NXE::JSONMessage& resp)
    {
        bData = true;
        response = resp;
    }
};

TEST_F(NavitInstanceTest, moveBy_without_data)
{
    setupMocks();

    NXE::NXEInstance instance{ injector };
    instance.Initialize();
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.HandleMessage(NXE::JSONMessage{0, "moveBy"}));
    EXPECT_TRUE(bData);
}

TEST_F(NavitInstanceTest, zoomBy_proper)
{
    using ::testing::Return;

    // Arrange
    ASSERT_FALSE(bData);
    setupMocks();
    EXPECT_CALL(*mock_ipc, zoom()).WillOnce(Return(10));

    NXE::NXEInstance instance{ injector };
    instance.Initialize();
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));

    // Act
    EXPECT_NO_THROW(instance.HandleMessage(TestUtils::zoomMessage()));
    EXPECT_TRUE(bData);

    // Assert
    EXPECT_EQ(response.error, "");
    ASSERT_FALSE(response.data.empty());
    EXPECT_EQ(response.data.get<double>("zoom"), 10);
}
