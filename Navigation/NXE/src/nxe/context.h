#ifndef CONTEXT_H
#define CONTEXT_H

#include <fruit/fruit_forward_decls.h>

namespace NXE {
class INavitIPC;
class INavitProcess;
class IGPSProvider;
class IMapDownloader;
class ISpeech;

namespace DI {
    typedef fruit::Injector<INavitIPC, INavitProcess, IGPSProvider, IMapDownloader, ISpeech> Injector;
    typedef fruit::Component<INavitIPC, INavitProcess, IGPSProvider, IMapDownloader, ISpeech> Components;
} // DI
}
#endif // CONTEXT_H
