#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "navitdbus.h"
#include "testutils.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>

struct NXEInstanceTest : public ::testing::Test {
protected:
    std::shared_ptr<NXE::NavitProcess> np{ new NXE::NavitProcessImpl };
    std::shared_ptr<NXE::NavitController> nc{ new NXE::NavitDBus };
    NXE::NXEInstance instance{ np, nc };

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }
};

TEST_F(NXEInstanceTest, proper_navit)
{
    std::string msg{ TestUtils::moveByMessage(1, 1) };
    EXPECT_NO_THROW(instance.HandleMessage(msg.data()));

    std::chrono::milliseconds dura(20 * 1000);
    std::this_thread::sleep_for(dura);
}
