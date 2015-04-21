#ifndef SETTINGTAGS_H
#define SETTINGTAGS_H

#include <string>

namespace SettingsTags {
namespace Navit {

    struct Path {
        typedef std::string type;
        static std::string name() noexcept { return "navit.path"; }
    };

    struct AutoStart {
        typedef bool type;
        static std::string name() noexcept { return "navit.autostart"; }
    };

    struct ExternalNavit {
        typedef bool type;
        static std::string name() noexcept { return "navit.external"; }
    };

    struct GraphicPlatform {
        typedef std::string type;
        static std::string name() noexcept { return "navit.platform"; }
    };
} // Navit

struct LogPath {
    typedef std::string type;
    static std::string name() noexcept { return "logPath"; }
};
struct LogLevel {
    typedef std::string type;
    static std::string name() noexcept { return "logLevel"; }
};

} // SettingsTags

#endif // SETTINGTAGS_H
