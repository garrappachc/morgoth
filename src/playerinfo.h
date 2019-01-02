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

#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "morgoth_export.h"
#include "steamid.h"
#include <QMetaType>
#include <QSharedDataPointer>

namespace morgoth {

class PlayerInfoData;

class MORGOTH_EXPORT PlayerInfo {
public:
    explicit PlayerInfo(const QString& name = QString());
    PlayerInfo(const PlayerInfo& other);
    virtual ~PlayerInfo();

    PlayerInfo& operator=(const PlayerInfo& other);

    QString name() const;
    void setName(const QString& name);

    SteamId steamId() const;
    void setSteamId(const SteamId& steamId);

    bool operator==(const PlayerInfo& other) const;

private:
    QSharedDataPointer<PlayerInfoData> d;

};

} // namespace morgoth

Q_DECLARE_METATYPE(morgoth::PlayerInfo);

#endif // PLAYERINFO_H
