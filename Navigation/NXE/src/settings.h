#ifndef NXE_SETTINGS_H
#define NXE_SETTINGS_H

#include <boost/property_tree/ptree.hpp>

namespace NXE {

class Settings {
public:
    Settings();
    ~Settings();

    template <typename T>
    typename T::type get()
    {
        return m_tree.get<typename T::type>(T::name());
    }

private:
    boost::property_tree::ptree m_tree;
    const std::string m_configPath;
};

} // namespace NXE

#endif // NXE_SETTINGS_H
