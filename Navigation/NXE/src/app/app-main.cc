#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtCore/QUuid>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtGui/QScreen>

#include <memory>
#include <vector>
#include <numeric>

#include "navititem.h"
#include "navitquickproxy.h"
#include "nxe_extension.h"
#include "nxe_instance.h"
#include "settings.h"
#include "settingtags.h"
#include "alog.h"
#include "navitsubcompositor.h"
#include "nxe_version.h"

void qtLogOutput(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    static std::map<std::string, std::string> cats;
    if (cats.empty()) {
        cats["default"] = "Qt";
        cats["qml"] = "QtQuick";
    }
    const std::string cat{ cats[ctx.category] };
    switch (type) {
    case QtDebugMsg:
        spdlog::get(cat)->warn() << ctx.file << "@" << ctx.line << " " << msg.toLatin1().data();
        break;
    case QtWarningMsg:
        spdlog::get(cat)->warn() << ctx.file << "@" << ctx.line << " " << msg.toLatin1().data();
        break;
    case QtCriticalMsg:
        spdlog::get(cat)->critical() << ctx.file << "@" << ctx.line << " " << msg.toLatin1().data();
    case QtFatalMsg:
        spdlog::get(cat)->emerg() << ctx.file << "@" << ctx.line << " " << msg.toLatin1().data();
    }
}

void createLoggers()
{
    NXE::Settings s;
    // Create app logger

    const std::string path = s.get<SettingsTags::LogPath>();
    std::shared_ptr<spdlog::sinks::sink> out{ new spdlog::sinks::stdout_sink_mt() };
    std::shared_ptr<spdlog::sinks::sink> rot{ new spdlog::sinks::simple_file_sink_mt(path + "/nxe-app.log", true) };
    spdlog::create("app", { out, rot });
    spdlog::create("Qt", { out, rot });
    spdlog::create("QtQuick", { out, rot });

    // Create nxe logger
    NXE::NXExtension::createLogger();

    qInstallMessageHandler(qtLogOutput);
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("nxe-app");
    QCoreApplication::setApplicationVersion("1.0");
    NXE::Settings s;

    int ret{ 0 };

    // remove all navit instances
    system("killall navit");
    system("rm -rf ~/.navit/destination.txt");
    system("cp /usr/share/navit/maps/borders.bin ~/.NavIt/maps/borders.bin");

    // Parse app settings
    QCommandLineParser parser;
    parser.addOptions({
        { { "d", "debug" },
         QCoreApplication::translate("main", "Enable verbose output") },
        { { "e", "external-navit" },
         QCoreApplication::translate("main", "Don't run navit, user has to run it manually") },
        { { "n", "navit-path" },
         QCoreApplication::translate("main", "Use non standard navit path"),
         QCoreApplication::translate("main", "path") },
    });

    createLoggers();
    spdlog::set_pattern("[%H:%M:%S.%e] [%n] [%t] [%l] %v");

    aInfo() << "NXE version is= " << gNXEVersion << " config path="<< s.configPath();

    qDebug() << " args=" << app.arguments();
    parser.parse(app.arguments());
#if defined(NXE_OS_LINUX)
    const QString navitPath = parser.value("navit-path");
    const QString rmDest = QString("rm -rf %1/destination.txt").arg(navitPath);
    system(rmDest.toLatin1().data());
    s.set<SettingsTags::Navit::Path>(parser.isSet("navit-path") ? navitPath.toStdString() : "/usr/bin/");
    s.save();
    s.set<SettingsTags::Navit::ExternalNavit>(parser.isSet("external-navit"));
    s.save();
#endif

    if (parser.isSet("debug")) {
        spdlog::set_level(spdlog::level::trace);
    }
    // if not set use nxe.conf settings

    // Create subcompositor
    try {
        const QString waylandSocketName = QByteArray{ "navit-" } + QUuid::createUuid().toByteArray();
        NavitSubCompositor view{ waylandSocketName };
        aInfo() << "Starting nxe-app with wayland socket name= " << view.socketName();
        NavitQuickProxy proxy{ view.socketName(), view.rootContext() };
        view.rootContext()->setContextProperty("navitProxy", &proxy);
        view.rootContext()->setContextProperty("navitMapsProxy", proxy.navitMapsProxy());
        view.rootContext()->setContextProperty("navigationProxy", proxy.navitNavigationProxy());
        view.rootContext()->setContextProperty("compositor", &view);
        view.setSource(QUrl("qrc:///qml/CompositorMainView.qml"));
#if defined(NXE_OS_LINUX)
        view.showMaximized();
#elif defined(NXE_OS_TIZEN)
        QScreen* s = app.primaryScreen();
        view.setGeometry(0,0, s->size().width(), s->size().height() - 200);
//        view.showMaximized();
        view.show();
#endif
        // Initialize all
        QObject::connect(&view, SIGNAL(windowAdded(QVariant)), view.rootObject(), SLOT(windowAdded(QVariant)));
        QObject::connect(&view, SIGNAL(windowResized(QVariant)), view.rootObject(), SLOT(windowResized(QVariant)));
        QObject::connect(&view, &NavitSubCompositor::resized, [&proxy](const QRect& rect) {
            proxy.resize(rect);
        });

        QObject::connect(&proxy, &NavitQuickProxy::quitSignal, &app, &QGuiApplication::quit);
        QObject::connect(&view, &NavitSubCompositor::windowDestroyed, [](QVariant) {
            aInfo() << "Window destroyed!!!!";
        });

        ret = app.exec();

    }
    catch (const std::exception& ex) {
        aFatal() << "An error ocurred while running nxe-app. Error code= " << ex.what();
        ret = -1;
    }

    return ret;
}
