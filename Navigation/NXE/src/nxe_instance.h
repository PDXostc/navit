#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "common/extension.h"
#include <memory>

namespace NXE {
class NavitProcess;
class NavitController;
class NavitIPCInterface;

class NXEInstancePrivate;
class NXEInstance : public common::Instance
{
public:
    typedef std::function<void (const std::string &)> MessageCb_type;
    NXEInstance() = delete;
    NXEInstance(std::weak_ptr<NavitProcess> navitProcess, std::weak_ptr<NavitIPCInterface> ipc);
    ~NXEInstance();

    virtual void Initialize() override;

    void HandleMessage(const char *msg) override;
    void registerMessageCallback(const MessageCb_type &cb);
private:
    std::unique_ptr<NXEInstancePrivate> d;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
