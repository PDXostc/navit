#ifndef NXEXTENSION_H
#define NXEXTENSION_H

#include "common/extension.h"

namespace NXE {

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
};

}

#endif // NXEXTENSION_H
