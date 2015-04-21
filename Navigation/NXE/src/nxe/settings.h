#ifndef NXE_SETTINGS_H
#define NXE_SETTINGS_H

#include <boost/property_tree/ptree.hpp>

namespace NXE {

// DO NOT USE nDebug, nLog or other logging macros here!
struct Settings {
public:
    Settings();

    const std::string& configPath() const noexcept
    {
        return m_configPath;
    }

    template <typename T>
    typename T::type get()
    {
        return m_tree.get<typename T::type>(T::name());
    }

    template <typename T>
    void set(const typename T::type& value)
    {
        m_tree.put(T::name(), value);
    }

    void save();

private:
    boost::property_tree::ptree m_tree;
    const std::string m_configPath;
};

} // namespace NXE

#endif // NXE_SETTINGS_H
