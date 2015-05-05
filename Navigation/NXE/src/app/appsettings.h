#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <boost/property_tree/ptree.hpp>

namespace Tags {
    struct EnablePoi {
        typedef bool ValueType;
        constexpr static const char* path {"enablePoi"};
    };

    struct Orientation {
        typedef std::string ValueType;
        constexpr static const char* path {"orientation"};
    };

    struct Ftu {
        typedef bool ValueType;
        constexpr static const char* path {"ftu"};
    };
    struct TopBarLocationVisible {
        typedef bool ValueType;
        constexpr static const char* path {"topBarLocationVisible"};
    };
}

class AppSettings
{
public:
    AppSettings();
    ~AppSettings();

    template<typename T>
    void set(typename T::ValueType val)
    {
        m_tree.put(T::path, val);
        save();
    }

    template<typename T>
    typename T::ValueType get() const
    {
        return m_tree.get<typename T::ValueType>(T::path);
    }

    template<typename RetVal>
    RetVal get(const std::string& key)
    {
        return m_tree.get<RetVal> ( key );
    }

    void save();

private:
    const std::string m_configPath;
    boost::property_tree::ptree m_tree;
};

#endif // APPSETTINGS_H
