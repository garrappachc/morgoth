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

#ifndef SERVERSTARTEDEVENT_H
#define SERVERSTARTEDEVENT_H

#include "eventhandler.h"

namespace morgoth {

class ServerStartedEvent : public EventHandler {
    Q_OBJECT

public:
    explicit ServerStartedEvent(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    const QString& game() const { return m_game; }

    static auto constexpr Name = "server.start";

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override;

private:
    QString m_game;

};

} // namespace Morgoth

#endif // SERVERSTARTEDEVENT_H
