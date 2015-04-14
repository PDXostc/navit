#ifndef CONTEXT_H
#define CONTEXT_H

#include <fruit/fruit_forward_decls.h>

namespace NXE {
class INavitIPC;
class INavitProcess;
class IGPSProvider;
class IMapDownloader;

namespace DI {
    typedef fruit::Injector<INavitIPC, INavitProcess, IGPSProvider, IMapDownloader> Injector;
    typedef fruit::Component<INavitIPC, INavitProcess, IGPSProvider, IMapDownloader> Components;
} // DI
}
#endif // CONTEXT_H
