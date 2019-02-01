// This file is part of morgoth.

// morgoth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "servercoordinator.h"
#include "morgothdaemon.h"
#include "serverconfiguration.h"
#include "servercoordinatoradaptor.h"
#include <QtCore>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

namespace morgoth {

class ServerCoordinatorPrivate {
public:
    explicit ServerCoordinatorPrivate(Server* server) : server(server) {}

    Server* server;
    ServerCoordinator::State state = ServerCoordinator::State::Offline;
    TmuxSessionWrapper tmux;
    QString logFileName;
};

ServerCoordinator::ServerCoordinator(Server* server) :
    QObject(server),
    d(new ServerCoordinatorPrivate(server))
{
    Q_ASSERT(server->isValid());

    connect(qApp, &QCoreApplication::aboutToQuit, this, &ServerCoordinator::stopSync);

    new ServerCoordinatorAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject(server->coordinatorPath().path(), this);
}

ServerCoordinator::~ServerCoordinator()
{
    // stopSync() cannot be used here, as the parent Server instance
    // is already destroyed and we relay on its existence
}

void ServerCoordinator::sendCommand(const QString& command)
{
    if (state() == Running)
        d->tmux.sendKeys(command);
}

const Server* ServerCoordinator::server() const
{
    return d->server;
}

ServerCoordinator::State ServerCoordinator::state() const
{
    return d->state;
}

void ServerCoordinator::setState(ServerCoordinator::State state)
{
    d->state = state;
    emit stateChanged(d->state);
}

bool ServerCoordinator::start()
{
    Q_ASSERT(server()->configuration());

    if (!server()->isValid()) {
        qWarning("%s is not installed properly", qPrintable(server()->name()));
        return false;
    }

    if (state() != Offline) {
        qWarning("%s is already running", qPrintable(server()->name()));
        return false;
    }

    qInfo("%s: starting...", qPrintable(server()->name()));
    setState(Starting);

    QString user = server()->configuration()->value("org.morgoth.Server.user");
    if (user.isEmpty() && morgothd)
        user = morgothd->config().value("user").toString();

    d->tmux.setUser(user);

    if (!d->tmux.create()) {
        qWarning("%s: could not create a tmux session", qPrintable(server()->name()));
        setState(Crashed);
        return false;
    }

    QString logDirectory = server()->configuration()->value("org.morgoth.Server.logDirectory");
    if (QDir::isRelativePath(logDirectory))
        logDirectory.prepend(server()->path().toLocalFile());

    d->logFileName = QString("%1/gameserver.log").arg(logDirectory);
    passwd* pwd = ::getpwnam(user.toLocal8Bit().constData());
    if (!pwd) {
        char* error = ::strerror(errno);
        qWarning("Error retrieving uid and gid of user %s (%s)", qPrintable(user), error);
        setState(Crashed);
        return false;
    } else {
        ::chown(d->logFileName.toLocal8Bit().constData(), pwd->pw_uid, pwd->pw_gid);
    }

    if (!d->tmux.redirectOutput(d->logFileName)) {
        qWarning("%s: could not redirect output to %s", qPrintable(server()->name()), qPrintable(d->logFileName));
        d->tmux.kill();
        setState(Crashed);
        return false;
    }

    QString arguments = server()->configuration()->value("org.morgoth.Server.launchArguments");
    QString cmd = QString("%1/srcds_run %2").arg(server()->path().toLocalFile(), arguments);
    if (!d->tmux.sendKeys(cmd)) {
        qWarning("%s: could not start the server", qPrintable(server()->name()));
        d->tmux.kill();
        setState(Crashed);
        return false;
    }

    return true;
}

void ServerCoordinator::stop()
{
    if (state() == Running || state() == Starting) {
        qDebug("%s: stopping...", qPrintable(server()->name()));
        setState(ShuttingDown);
        d->tmux.sendKeys("quit");
    }
}

void ServerCoordinator::handleServerStarted()
{
    Q_ASSERT(state() == Starting);
    setState(Running);
    qInfo("%s: started", qPrintable(server()->name()));
}

void ServerCoordinator::handleServerStopped()
{
    Q_ASSERT(state() == ShuttingDown);
    setState(Offline);
    qInfo("%s: stopped", qPrintable(server()->name()));
}

void ServerCoordinator::stopSync()
{
    // send stop keys and wait for the server to actually shut down
    stop();
    // TODO
}

} // namespace Morgoth

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerCoordinator::State& state)
{
    QString strStatus = QMetaEnum::fromType<morgoth::ServerCoordinator::State>().valueToKey(state);

    argument.beginStructure();
    argument << strStatus;
    argument.endStructure();

    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerCoordinator::State& state)
{
    QString strStatus;

    argument.beginStructure();
    argument >> strStatus;
    argument.endStructure();

    int value = QMetaEnum::fromType<morgoth::ServerCoordinator::State>().keyToValue(strStatus.toLocal8Bit().constData());
    state = static_cast<morgoth::ServerCoordinator::State>(value);
    return argument;
}

static void registerMetaType()
{
    qDBusRegisterMetaType<morgoth::ServerCoordinator::State>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetaType)
