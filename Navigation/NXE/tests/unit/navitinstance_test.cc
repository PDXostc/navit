
#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "mocks/navitprocessmock.h"
#include "nxe_instance.h"

using ::testing::StrictMock;
namespace bpt = boost::property_tree;

struct NavitInstanceTest : public ::testing::Test{

    std::shared_ptr<StrictMock<NavitProcessMock>> mock { new StrictMock<NavitProcessMock>{}};
    std::shared_ptr<NXE::NavitProcess> process {mock};

  static void SetUpTestCase() {

      // create simple json config file
      bpt::ptree config;
      config.put("navit.path", "/this/is/not/a/real/path");
      bpt::write_json("nxe.conf", config);
  }
};

TEST_F(NavitInstanceTest, handleMsg)
{
    using ::testing::Return;
    using ::testing::_;
    // Arrange
    NavitProcessMock* mock = dynamic_cast<NavitProcessMock*>(process.get());
    EXPECT_CALL(*mock, isRunning()).WillOnce(Return(false));
    EXPECT_CALL(*mock, start());
    EXPECT_CALL(*mock, setProgramPath("/this/is/not/a/real/path"));

    // Act
    NXE::NXEInstance instance { process };
    instance.HandleMessage("");

    // Assert
}

