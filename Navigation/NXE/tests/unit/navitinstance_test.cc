
#include <gtest/gtest.h>

#include "mocks/navitprocessmock.h"
#include "mocks/navitcontrollermock.h"
#include "nxe_instance.h"
#include "settingtags.h"
#include "jsonmessage.h"
#include "../testutils.h"

using ::testing::StrictMock;

struct NavitInstanceTest : public ::testing::Test {

    std::shared_ptr<StrictMock<NavitProcessMock> > mock{ new StrictMock<NavitProcessMock>{} };
    std::shared_ptr<NXE::NavitProcess> process{ mock };
    std::shared_ptr<StrictMock<NavitControllerMock> > controllerMock { new StrictMock<NavitControllerMock>{} };
    std::shared_ptr<NXE::NavitController> controller { controllerMock };

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }
};

TEST_F(NavitInstanceTest, moveBy_without_data)
{
    using ::testing::Return;
    using ::testing::_;
    // Arrange
    NavitProcessMock* mock = dynamic_cast<NavitProcessMock*>(process.get());
    EXPECT_CALL(*mock, isRunning()).WillOnce(Return(false));
    EXPECT_CALL(*mock, start());
    EXPECT_CALL(*mock, setProgramPath("/this/is/not/a/real/path"));

    NXE::NXEInstance instance{ process, controller };
    const std::string incomingMessage = "{\"id\":0, \"call\":\"moveBy\"}";
    EXPECT_ANY_THROW(instance.HandleMessage(incomingMessage.data()));
}

TEST_F(NavitInstanceTest, moveBy_with_data)
{
    using ::testing::Return;
    using ::testing::_;
    // Arrange
    NavitProcessMock* mock = dynamic_cast<NavitProcessMock*>(process.get());
    EXPECT_CALL(*mock, isRunning()).WillOnce(Return(false));
    EXPECT_CALL(*mock, start());
    EXPECT_CALL(*mock, setProgramPath("/this/is/not/a/real/path"));

    NXE::NXEInstance instance{ process, controller };
    const std::string incomingMessage =
            "{ \
                \"id\":0, \
                \"call\":\"moveBy\" \
                \"data\": { \
                    \"x\": -15,\
                    \"y\": -15 \
                } \
            }";
    EXPECT_NO_THROW(instance.HandleMessage(incomingMessage.data()));
}
