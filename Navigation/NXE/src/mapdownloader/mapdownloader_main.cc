#include "mapdownloaderdbusserver.h"
#include "mdlog.h"

#include <dbus-c++/dbus.h>
#include <unistd.h>
#include <signal.h>

::DBus::BusDispatcher dispatcher;

void signalHandler(int signal)
{
    mdInfo() << "Signal " << signal << " captured, exiting";
    dispatcher.leave();
}

int main(int argc, char* argv[])
{
    const std::vector<std::string> arguments{ argv + 1, argv + argc };
    bool debug = std::find(arguments.begin(), arguments.end(), "--debug") != arguments.end();

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    std::shared_ptr<spdlog::sinks::sink> out{ new spdlog::sinks::stdout_sink_mt{} };
    std::shared_ptr<spdlog::sinks::sink> fileOut{ new spdlog::sinks::simple_file_sink_mt{"/tmp/md.log"} };
    std::shared_ptr<spdlog::sinks::sink> syslogOut{ new spdlog::sinks::syslog_sink {"md", 0, LOG_DAEMON	} };
    spdlog::create("md", { out, fileOut, syslogOut });
    if(debug) {
        spdlog::set_level(spdlog::level::trace);
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    ::DBus::default_dispatcher = &dispatcher;
    ::DBus::Connection con = ::DBus::Connection::SessionBus();
    con.request_name("org.nxe.MapDownloader");

    try {
        md::MapDownloaderDBusServer server{ con };

        mdInfo() << "Starting dbus map downloader service";
        dispatcher.enter();
    } catch( const std::exception &ex)  {
        mdError() << "An error occured while starting DBus service. Err= " << ex.what();
        return -1;
    }

    return 0;
}
