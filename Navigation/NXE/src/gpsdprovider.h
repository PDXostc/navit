#ifndef NXE_GPSDPROVIDER_H
#define NXE_GPSDPROVIDER_H

#include "igpsprovider.h"
#include <memory>

namespace NXE {

struct GPSDProviderPrivate;
class GPSDProvider : public IGPSProvider
{
public:
    GPSDProvider();
    ~GPSDProvider();

    virtual Position position() override;
private:
    std::unique_ptr<GPSDProviderPrivate> d;
};

} // namespace NXE

#endif // NXE_GPSDPROVIDER_H
