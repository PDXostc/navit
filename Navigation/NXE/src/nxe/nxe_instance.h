#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "context.h"

#include <memory>
#include <vector>
#include <tuple>

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

    bool HandleMessage(const NXE::JSONMessage& msg);

    void setWaylandSocketName(const std::string& socketName);

private:
    std::unique_ptr<NXEInstancePrivate> d;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
