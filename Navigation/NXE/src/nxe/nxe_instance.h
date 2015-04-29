#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "context.h"
#include "calls.h"
#include "imapdownloader.h"
#include "igpsprovider.h"

#include <memory>
#include <vector>
#include <tuple>
#include <type_traits>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/include/at_key.hpp>

namespace NXE {
class INavitProcess;
class INavitIPC;
class NavitController;
class IGPSProvider;
struct JSONMessage;

struct NXEInstancePrivate;
class NXEInstance {
public:
    typedef std::function<void(const NXE::JSONMessage&)> MessageCbJSON_type;

    NXEInstance() = delete;
    NXEInstance(DI::Injector& impls);
    ~NXEInstance();

    virtual void Initialize();

    void registerMessageCallback(const MessageCbJSON_type& cb);

    void setWaylandSocketName(const std::string& socketName);

    template<typename T, typename...Args>
    typename T::ReturnType HandleMessage123(Args... args)
    {
        auto fn = boost::fusion::at_key<T>(fusion_list);
        // fn function will throw on error
        return fn(std::forward<Args>(args)...);
    }

    void setMapDownloaderListener(const MapDownloaderListener& listener);
    void setPositionUpdateListener(const NXE::IGPSProvider::PositionUpdateCb& listener);

private:
    std::unique_ptr<NXEInstancePrivate> d;
    map_cb_type fusion_list;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
