#include "nxe_extension.h"
#include "nxe_instance.h"

using namespace NXE;

extern const char kAscii_nxe_api[];

common::Extension* CreateExtension()
{
    return new NXExtension();
}

NXExtension::NXExtension()
{
    SetExtensionName("nxe");
    SetJavaScriptAPI(kAscii_nxe_api);
}

NXExtension::~NXExtension()
{
}

common::Instance *NXExtension::CreateInstance()
{
    return new NXEInstance();
}

void NXExtension::OnShutdown(XW_Extension xw_extension)
{

}

void NXExtension::OnInstanceCreated(XW_Instance xw_instance)
{

}

void NXExtension::OnInstanceDestroyed(XW_Instance xw_instance)
{

}

