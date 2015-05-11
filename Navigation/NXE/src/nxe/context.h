#ifndef CONTEXT_H
#define CONTEXT_H

#include <tuple>
#include <memory>
namespace NXE {
class INavitIPC;
class INavitProcess;
class IGPSProvider;
class IMapDownloader;
class ISpeech;

template<int Index, class Search, class First, class... Types>
struct get_internal
{
    typedef typename get_internal<Index + 1, Search, Types...>::type type;
        static constexpr int index = Index;
};

template<int Index, class Search, class... Types>
struct get_internal<Index, Search, Search, Types...>
{
    typedef get_internal type;
    static constexpr int index = Index;
};

template<class T, class... Types>
T get(std::tuple<Types...> tuple)
{
    return std::get<get_internal<0,T,Types...>::type::index>(tuple);
}

namespace DI {
typedef std::tuple<std::shared_ptr<INavitIPC>,
                    std::shared_ptr<INavitProcess>,
                    std::shared_ptr<IGPSProvider>,
                    std::shared_ptr<IMapDownloader>,
                    std::shared_ptr<ISpeech>
        >Injector;

} // DI
}
#endif // CONTEXT_H
