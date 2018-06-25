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

#ifndef SERVERSTOPPED_H
#define SERVERSTOPPED_H

#include "gameevent.h"
#include "morgoth_export.h"

namespace morgoth {

class MORGOTH_EXPORT ServerStopped : public GameEvent {
    Q_OBJECT

public:
    explicit ServerStopped(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    static auto constexpr Name = "server.stopped";

};

} // namespace Morgoth

#endif // SERVERSTOPPED_H
