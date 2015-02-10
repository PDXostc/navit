
#include <gtest/gtest.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;

struct TestListener : public ::testing::EmptyTestEventListener
{

    void OnTestStart(const ::testing::TestInfo &info) override {
        BOOST_LOG_TRIVIAL(debug) << "Test " << info.name() << " started";
    }

    void OnTestEnd(const ::testing::TestInfo &info) override {
        BOOST_LOG_TRIVIAL(debug) << "Test " << info.name() << " finished";
    }
};

int main(int argc, char* argv[])
{
    logging::add_common_attributes();
    std::string name = std::string(argv[0]) + ".log";
    auto sink = logging::add_file_log(name);

    // in tests we log everything
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::trace
    );

    sink->set_formatter (
                   expr::format("[%1%]<%2%>(%3%): %4%")
                   % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                   % logging::trivial::severity
                   % expr::attr<boost::log::attributes::current_thread_id::value_type >("ThreadID")
                   % expr::smessage
                   );

    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new TestListener);
    return RUN_ALL_TESTS();
}
