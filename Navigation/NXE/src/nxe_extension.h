#ifndef NXEXTENSION_H
#define NXEXTENSION_H

#include "common/extension.h"
#include <memory>

namespace NXE {
class NavitProcess;
class NavitController;

/*!
 * \brief The NXExtension class describes an extension for Crosswalk to talk with Navit application
 * The object is created in xwalk process space and it's used to create instances of NXE::NXEInstance
 * which actually does the connection
 */
class NXExtension : public common::Extension
{
public:
    NXExtension();
    ~NXExtension();
private:
    // From common::Extension

    /*!
     * \brief Creates an instance of NXE::NXEInstance type
     * \return Pointer to newly created object. Ownership is transferred to xwalk
     */
    virtual common::Instance *CreateInstance() override;

    static void OnShutdown(XW_Extension xw_extension);
    static void OnInstanceCreated(XW_Instance xw_instance);
    static void OnInstanceDestroyed(XW_Instance xw_instance);

private:
    std::shared_ptr<NavitProcess> m_navitProcess;
    std::shared_ptr<NavitController> m_navitController;

};

}

#endif // NXEXTENSION_H
