#ifndef NXE_GPSDPROVIDER_H
#define NXE_GPSDPROVIDER_H

#include "igpsprovider.h"
#include <memory>

#include <fruit/macro.h>

namespace NXE {

struct GPSDProviderPrivate;
class GPSDProvider : public IGPSProvider {
public:
    INJECT(GPSDProvider());
    ~GPSDProvider();

    virtual Position position() const override;

private:
    std::unique_ptr<GPSDProviderPrivate> d;
};

} // namespace NXE

#endif // NXE_GPSDPROVIDER_H
