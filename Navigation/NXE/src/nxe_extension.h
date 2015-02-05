#ifndef NXEXTENSION_H
#define NXEXTENSION_H

#include "common/extension.h"

namespace NXE {
class NavitProcess;

class NXExtension : public common::Extension
{
public:
    NXExtension();
    ~NXExtension();
private:
    // From common::Extension
    virtual common::Instance *CreateInstance() override;
    static void OnShutdown(XW_Extension xw_extension);
    static void OnInstanceCreated(XW_Instance xw_instance);
    static void OnInstanceDestroyed(XW_Instance xw_instance);

private:
    std::shared_ptr<NavitProcess> m_navitProcess;

};

}

#endif // NXEXTENSION_H
