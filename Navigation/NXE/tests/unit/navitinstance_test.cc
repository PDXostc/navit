
#include <gtest/gtest.h>

#include "mocks/navitprocessmock.h"
#include "mocks/navitcontrollermock.h"
#include "nxe_instance.h"
#include "navitcontroller.h"
#include "navitipc.h"
#include "settingtags.h"
#include "jsonmessage.h"
#include "../testutils.h"
#include "log.h"

using ::testing::StrictMock;
const std::string navitPath { NAVIT_PATH };

struct NavitInstanceTest : public ::testing::Test {

    std::shared_ptr<StrictMock<NavitProcessMock> > processMock{ new StrictMock<NavitProcessMock>{} };
    std::shared_ptr<NXE::NavitProcess> process{ processMock };
    std::shared_ptr<StrictMock<NavitIPCMock> > ipcMock { new StrictMock<NavitIPCMock>{} };
    std::shared_ptr<NXE::NavitIPCInterface> ipc { ipcMock };

    NavitProcessMock *mock_process { (dynamic_cast<NavitProcessMock*>(process.get())) };
    NavitIPCMock *mock_ipc { (dynamic_cast<NavitIPCMock*>(ipc.get())) };

    bool bData = false;
    std::string response;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void setupMocks() {
        using ::testing::Return;
        EXPECT_CALL(*mock_process, isRunning()).WillOnce(Return(false));
        EXPECT_CALL(*mock_process, start()).WillRepeatedly(Return(true));
        EXPECT_CALL(*mock_process, setProgramPath(navitPath));
        EXPECT_CALL(*mock_process, stop());
        EXPECT_CALL(*mock_ipc, start());
    }

    void callback(const std::string &str) {
        ASSERT_NE(str, "error");
        nDebug() << "Callback data=" << str;
        bData = true;
        response = str;
    }
};

TEST_F(NavitInstanceTest, moveBy_without_data)
{
    TestUtils::Timer t;
    setupMocks();

    NXE::NXEInstance instance{ process, ipc };
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));
    const std::string incomingMessage = "{\"id\":0, \"call\":\"moveBy\"}";
    EXPECT_NO_THROW(instance.HandleMessage(incomingMessage.data()));
    EXPECT_TRUE(bData);
}

TEST_F(NavitInstanceTest, moveBy_with_data)
{
    TestUtils::Timer t;
    // Arrange
    ASSERT_FALSE(bData);
    setupMocks();
    EXPECT_CALL(*mock_ipc, moveBy(-15,-15));

    NXE::NXEInstance instance{ process, ipc };
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));
    const std::string incomingMessage =
            "{ \
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
    TestUtils::Timer t;
    using ::testing::Return;

    // Arrange
    ASSERT_FALSE(bData);
    setupMocks();
    EXPECT_CALL(*mock_ipc, zoom()).WillOnce(Return(10));

    NXE::NXEInstance instance{ process, ipc };
    instance.registerMessageCallback(std::bind(&NavitInstanceTest::callback, this, std::placeholders::_1));
    const std::string incomingMessage =
            "{ \
                \"id\":15, \
                \"call\":\"zoom\" \
            }";

    // Act
    EXPECT_NO_THROW(instance.HandleMessage(incomingMessage.data()));
    EXPECT_TRUE(bData);
    EXPECT_NE(response,"");
    NXE::JSONMessage json = NXE::JSONUtils::deserialize(response);

    // Assert
    ASSERT_FALSE(json.data.empty());
    EXPECT_EQ(json.data.get<double>("zoom"),10);
}
