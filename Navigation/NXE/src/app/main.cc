#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>

#include <memory>

#include "navititem.h"
#include "navitquickproxy.h"
#include "nxe_extension.h"
#include "nxe_instance.h"
#include "fruit/fruit.h"
#include "navitdbus.h"
#include "navitprocessimpl.h"
#include "gpsdprovider.h"
#include "alog.h"

NXE::NXEInstance * g_nxeInstance = nullptr;

struct Context {
    NXE::NXEInstance* instance = nullptr;
    NXE::DI::Injector injector{ []() -> NXE::DI::Components {
        return fruit::createComponent()
                .bind<NXE::INavitIPC, NXE::NavitDBus>()
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>()
                .bind<NXE::IGPSProvider, NXE::GPSDProvider>();
    }() };
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<NavitItem>("Navit", 1,0, "NavitItem");

    // Create logger
    std::shared_ptr<spdlog::sinks::sink> out{ new spdlog::sinks::stdout_sink_mt() };
    spdlog::create("app", {out});

    NXE::NXExtension::createLogger();
    spdlog::set_level(spdlog::level::err);

    Context cxt;
    cxt.instance = new NXE::NXEInstance {cxt.injector};
    cxt.instance->registerMessageCallback( [](const std::string& message) {
        aTrace() << "Received message in app";

    } );
    cxt.instance->Initialize();
    g_nxeInstance = cxt.instance;

    NavitQuickProxy proxy;

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("navitProxy", &proxy);
    view.setSource(QUrl("qrc:///qml/MainView.qml"));
    view.show();

    return app.exec();
}
