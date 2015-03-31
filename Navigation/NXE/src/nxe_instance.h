#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "common/extension.h"
#include <memory>
#include <vector>

#include <fruit/fruit_forward_decls.h>

namespace NXE {
class INavitProcess;
class INavitIPC;
class NavitController;

class NXEInstancePrivate;
class NXEInstance : public common::Instance
{
public:
    typedef fruit::Injector<INavitIPC, INavitProcess> DepInInterfaces;

    typedef std::function<void (const std::string &)> MessageCb_type;
    NXEInstance() = delete;
    NXEInstance(DepInInterfaces & impls);
    ~NXEInstance();

    virtual void Initialize() override;

    void HandleMessage(const char *msg) override;
    void registerMessageCallback(const MessageCb_type &cb);

    std::vector<double> renderMeasurements() const;

private:
    std::unique_ptr<NXEInstancePrivate> d;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
