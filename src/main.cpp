#include "servermanager.h"
#include "server.h"
#include "servercoordinator.h"
#include <QtCore>
#include <csignal>

using namespace Morgoth;

void handleUnixSignals(std::initializer_list<int> quitSignals)
{
    auto handler = [](int sig) -> void {
        // blocking and not aysnc-signal-safe func are valid
        qInfo("\n--- Caught signal %d; exiting gracefully ---\n", sig);
        QCoreApplication::quit();
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    handleUnixSignals({ SIGQUIT, SIGINT, SIGTERM, SIGHUP });

    ServerManager* sm = new ServerManager(&app);

    auto server = sm->find("one");
    server->coordinator()->start();

    QTimer::singleShot(30 * 1000, [server]() { server->coordinator()->command("changelevel cp_badlands"); });
    QTimer::singleShot(60 * 1000, &app, &QCoreApplication::quit);

    return app.exec();
}
