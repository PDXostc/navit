#ifndef NXE_NXEINSTANCE_H
#define NXE_NXEINSTANCE_H

#include "common/extension.h"

namespace NXE {

class NXEInstance : public common::Instance
{
public:
    NXEInstance();
    ~NXEInstance();

    void HandleMessage(const char *msg) override;
};

} // namespace NXE

#endif // NXE_NXEINSTANCE_H
