#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "common/extension.h"
#include "context.h"

#include <memory>
#include <vector>

namespace NXE {
class INavitProcess;
class INavitIPC;
class NavitController;
class IGPSProvider;
struct JSONMessage;

class NXEInstancePrivate;
class NXEInstance : public common::Instance {
public:
    typedef std::function<void(const std::string&)> MessageCb_type;
    typedef std::function<void(const NXE::JSONMessage&)> MessageCbJSON_type;

    NXEInstance() = delete;
    NXEInstance(DI::Injector& impls);
    ~NXEInstance();

    virtual void Initialize() override;

    void HandleMessage(const char* msg) override;
    void registerMessageCallback(const MessageCb_type& cb);
    void registerMessageCallback(const MessageCbJSON_type& cb);

    bool HandleMessage(const NXE::JSONMessage& msg);

    std::vector<double> renderMeasurements() const;

private:
    std::unique_ptr<NXEInstancePrivate> d;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
