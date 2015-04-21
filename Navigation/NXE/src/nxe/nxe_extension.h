#ifndef NXEXTENSION_H
#define NXEXTENSION_H

#include <memory>

namespace NXE {
class NavitProcess;
class NavitController;

struct NXExtensionPrivate;

/*!
 * \brief The NXExtension class describes an extension for Crosswalk to talk with Navit application
 * The object is created in xwalk process space and it's used to create instances of NXE::NXEInstance
 * which actually does the connection
 */
class NXExtension {
public:
    static void createLogger();

private:
};
}

#endif // NXEXTENSION_H
