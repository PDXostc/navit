#include <vector>
#include <string>
#include <algorithm>
#include "log.h"
#include "nxe_instance.h"
#include "dbuscontroller.h"
#include "navitdbus.h"
#include "navitprocessimpl.h"
#include "mapdownloaderdbus.h"
#include "context.h"
#include "gpsdprovider.h"
#include "speechimpldbus.h"

using namespace NXE;

struct NXETizenTest {
    DBusController dbusController;
    INavitIPC * ipc { new NavitDBus{dbusController}};
    IMapDownloader * md { new MapDownloaderDBus{dbusController}};
    NXE::ISpeech* speech { new NXE::SpeechImplDBus{dbusController}};
    NXE::DI::Injector injector{ std::make_tuple(
                    std::shared_ptr<NXE::INavitIPC>(ipc),
                    std::shared_ptr<NXE::INavitProcess>(new NXE::NavitProcessImpl),
                    std::shared_ptr<NXE::IGPSProvider>(new GPSDProvider),
                    std::shared_ptr<NXE::IMapDownloader>(new MapDownloaderDBus{dbusController}),
                    std::shared_ptr<NXE::ISpeech>(new SpeechImplDBus{dbusController})
                    )};
    NXEInstance instance{ injector };
};

//TEST_F(NXETizenTest, checkIfMapDownloaderDBusServiceExists)
//{
    //IMapDownloader* downloader = injector.get<IMapDownloader* >();
    //try {
        //auto maps = downloader->maps();
    //}
    //catch (const std::exception& ex) {
        //nError() << "Exception during maps " << ex.what();
        //FAIL();
    //}
//}

//TEST_F(NXETizenTest, checkSpeech)
//{
    //try {
        //speech->say("This is a test");
    //}
    //catch (const std::exception& ex) {
        //nError() << "Exception during speech " << ex.what();
    //}
//}

int main(int argc, char* argv[])
{
    const std::vector<std::string> arguments{ argv + 1, argv + argc };

    bool debug = std::find(arguments.begin(), arguments.end(), "--debug") != arguments.end();
    bool perf = std::find(arguments.begin(), arguments.end(), "--perf") != arguments.end();
    bool extNavit = std::find(arguments.begin(), arguments.end(), "--no-navit") != arguments.end();

    auto logger = spdlog::stdout_logger_mt("nxe");
    auto perfLogger = spdlog::stdout_logger_mt("perf");
    if (perf) {
        perfLogger->set_level(spdlog::level::info);
    } else {
        perfLogger->set_level(spdlog::level::off);
    }
    if (debug) {
        logger->set_level(spdlog::level::trace);
        perfLogger->set_level(spdlog::level::info);
    }
    else {
        logger->set_level(spdlog::level::off);
    }

    spdlog::set_pattern("[%H:%M:%S.%e] [%t] [%l] %v");
    //::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
    return 0;
}
