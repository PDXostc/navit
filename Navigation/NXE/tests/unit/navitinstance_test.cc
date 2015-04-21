#include <gtest/gtest.h>

#include "mocks/navitprocessmock.h"
#include "mocks/navitcontrollermock.h"
#include "mocks/gpsmock.h"
#include "mocks/mapdownloadermock.h"
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
                .bind<NXE::IMapDownloader, MapDownloaderMock>();
    }() };

    NavitProcessMock* mock_process{ (dynamic_cast<NavitProcessMock*>(injector.get<NXE::INavitProcess*>())) };
    NavitIPCMock* mock_ipc{ (dynamic_cast<NavitIPCMock*>(injector.get<NXE::INavitIPC*>())) };
    MapDownloaderMock* mock_mapd{ (dynamic_cast<MapDownloaderMock*>(injector.get<NXE::IMapDownloader*>())) };

    bool bData = false;
    std::string response;

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

    void callback(const std::string& str)
    {
        ASSERT_NE(str, "error");
        nDebug() << "Callback data=" << str;
        bData = true;
        response = str;
    }
};

TEST_F(NavitInstanceTest, moveBy_without_data)
{
    setupMocks();

    NXE::NXEInstance instance{ injector };
    instance.Initialize();
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));
    const std::string incomingMessage = "{\"id\":0, \"call\":\"moveBy\"}";
    EXPECT_NO_THROW(instance.HandleMessage(incomingMessage.data()));
    EXPECT_TRUE(bData);
}

TEST_F(NavitInstanceTest, moveBy_with_data)
{
    // Arrange
    ASSERT_FALSE(bData);
    setupMocks();
    EXPECT_CALL(*mock_ipc, moveBy(-15, -15));

    NXE::NXEInstance instance{ injector };
    instance.Initialize();
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));
    const std::string incomingMessage = "{ \
                \"id\":0, \
                \"call\":\"moveBy\", \
                \"data\": { \
                    \"x\": -15,\
                    \"y\": -15 \
                } \
            }";
    EXPECT_NO_THROW(instance.HandleMessage(incomingMessage.data()));
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
    const std::string incomingMessage = "{ \
                \"id\":15, \
                \"call\":\"zoom\" \
            }";

    // Act
    EXPECT_NO_THROW(instance.HandleMessage(incomingMessage.data()));
    EXPECT_TRUE(bData);
    EXPECT_NE(response, "");
    NXE::JSONMessage json = NXE::JSONUtils::deserialize(response);

    // Assert
    ASSERT_FALSE(json.data.empty());
    EXPECT_EQ(json.data.get<double>("zoom"), 10);
}
