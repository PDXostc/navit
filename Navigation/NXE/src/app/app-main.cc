#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtCore/QUuid>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

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


void qtLogOutput(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    static std::map<std::string, std::string> cats;
    if (cats.empty()) {
        cats["default"] = "Qt";
        cats["qml"] = "QtQuick";
    }
    const std::string cat { cats[ctx.category] };
    switch(type) {
    case QtDebugMsg:
        spdlog::get(cat)->debug() << ctx.file << "@" << ctx.line << " " << msg.toLatin1().data();
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
    spdlog::create("app", {out, rot});
    spdlog::create("Qt", {out, rot});
    spdlog::create("QtQuick", {out, rot});

    // Create nxe logger
    NXE::NXExtension::createLogger();

    qInstallMessageHandler(qtLogOutput);
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("nxe-app");
    QCoreApplication::setApplicationVersion("1.0");
    NXE::Settings s;

    int ret {0};

    // Parse app settings
    QCommandLineParser parser;
    parser.addOptions({
            {{"d", "debug"},
                QCoreApplication::translate("main", "Enable verbose output")},
            {{"n", "navit-path"},
                QCoreApplication::translate("main", "Use non standard navit path"),
                QCoreApplication::translate("main", "path")},
        });

    createLoggers();

    aInfo() << "NXE version is= " << gNXEVersion;

    qDebug() << " args=" << app.arguments();
    parser.parse(app.arguments());
    if (parser.isSet("navit-path")) {
        QString value = parser.value("navit-path");
        aTrace() << "Non default navit path= " << value.toLatin1().data();
        s.set<SettingsTags::Navit::Path>(value.toLatin1().data());
        s.save();
    }

    if(parser.isSet("debug")) {
        spdlog::set_level(spdlog::level::trace);
    }
    // if not set use nxe.conf settings

    // Create subcompositor
    try {
        const QString waylandSocketName = QByteArray{"navit-"} + QUuid::createUuid().toByteArray();
        NavitSubCompositor view {waylandSocketName};
        aInfo() << "Starting nxe-app with wayland socket name= " << view.socketName();
        NavitQuickProxy proxy {view.socketName(), view.rootContext()};
        view.rootContext()->setContextProperty("navitProxy", &proxy);
        view.rootContext()->setContextProperty("navitMapsProxy", proxy.navitMapsProxy());
        view.rootContext()->setContextProperty("compositor", &view);
        view.setSource(QUrl("qrc:///qml/CompositorMainView.qml"));
#if defined(NXE_OS_LINUX)
        view.showMaximized();
#elif defined(NXE_OS_TIZEN)
        view.showFullScreen();
#endif
        // Initialize all
        QObject::connect(&view, SIGNAL(windowAdded(QVariant)), view.rootObject(), SLOT(windowAdded(QVariant)));
        QObject::connect(&view, SIGNAL(windowResized(QVariant)), view.rootObject(), SLOT(windowResized(QVariant)));

        QObject::connect(&proxy, &NavitQuickProxy::quitSignal, &app, &QGuiApplication::quit);
        ret = app.exec();

    } catch(const std::exception& ex) {
        aFatal() << "An error ocurred while running nxe-app. Error code= " << ex.what();
        ret =-1;
    }

    return ret;
}
