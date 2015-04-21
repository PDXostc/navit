#include "navitprocessimpl.h"
#include "nxe_extension.h"
#include "testutils.h"
#include <navitdbus.h>

#include <iostream>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

class NavitProcessTest : public ::testing::Test {
protected:
    NXE::NavitProcessImpl process;
};

TEST_F(NavitProcessTest, failure_nxe_file_not_present)
{
    TestUtils::removeNXEConfFile();
    EXPECT_ANY_THROW(process.start());
    EXPECT_FALSE(process.isRunning());
}

TEST_F(NavitProcessTest, success_start_pathSet)
{
    TestUtils::createNXEConfFile();
    EXPECT_TRUE(process.start());
    EXPECT_TRUE(process.isRunning());
    TestUtils::removeNXEConfFile();
}

TEST_F(NavitProcessTest, process_externally_killed)
{
    TestUtils::createNXEConfFile();

    EXPECT_TRUE(process.start());
    EXPECT_TRUE(process.isRunning());

    std::thread t { [](){
                    system("killall navit");
    }};

    std::chrono::milliseconds dura(1000 * 2);
    std::this_thread::sleep_for(dura);

    t.join();
    TestUtils::removeNXEConfFile();
}
