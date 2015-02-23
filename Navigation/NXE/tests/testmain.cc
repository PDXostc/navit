
#include <gtest/gtest.h>
#include "log.h"

struct TestListener : public ::testing::EmptyTestEventListener
{

    void OnTestStart(const ::testing::TestInfo &info) override {
//        BOOST_LOG_TRIVIAL(debug) << "Test " << info.name() << " started";
    }

    void OnTestEnd(const ::testing::TestInfo &info) override {
//        BOOST_LOG_TRIVIAL(debug) << "Test " << info.name() << " finished";
    }
};

int main(int argc, char* argv[])
{
//    logging::add_common_attributes();
//    std::string name = std::string(argv[0]) + ".log";
//    auto sink = logging::add_file_log(name);
//    auto consSink = logging::add_console_log(std::cout);

//    // in tests we log everything
//    logging::core::get()->set_filter
//    (
//        logging::trivial::severity >= logging::trivial::trace
//    );
//    sink->locked_backend()->auto_flush(true);
//    consSink->locked_backend()->auto_flush(true);

//    sink->set_formatter (
//                   expr::format("[%1%]<%2%>(%3%): %4%")
//                   % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
//                   % logging::trivial::severity
//                   % expr::attr<boost::log::attributes::current_thread_id::value_type >("ThreadID")
//                   % expr::smessage
//                   );
//    consSink->set_formatter (
//                   expr::format("[%1%]<%2%>(%3%): %4%")
//                   % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
//                   % logging::trivial::severity
//                   % expr::attr<boost::log::attributes::current_thread_id::value_type >("ThreadID")
//                   % expr::smessage
//                   );

//    auto logger = spdlog::rotating_logger_mt("nxe_logger", "nxelog", 1048576 * 5, 3);
//    logger->set_level(spdlog::level::debug);
    nDebug() << "Hallo!";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new TestListener);
    return RUN_ALL_TESTS();
}
