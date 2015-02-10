
#include <gtest/gtest.h>
#include "mocks/navitprocessmock.h"
#include "nxe_instance.h"

using ::testing::StrictMock;

struct NavitInstanceTest : public ::testing::Test{

    std::shared_ptr<StrictMock<NavitProcessMock>> mock { new StrictMock<NavitProcessMock>{}};

    std::shared_ptr<NXE::NavitProcess> process {mock};
};

TEST_F(NavitInstanceTest, handleMsg)
{
    using ::testing::Return;
    // Arrange
    NXE::NXEInstance instance { process };
    NavitProcessMock* mock = dynamic_cast<NavitProcessMock*>(process.get());
    EXPECT_CALL(*mock, isRunning()).WillOnce(Return(false));
    EXPECT_CALL(*mock, start());

    // Act
    instance.HandleMessage("");

    // Assert
}

