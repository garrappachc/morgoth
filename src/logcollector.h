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

#ifndef LOGCOLLECTOR_H
#define LOGCOLLECTOR_H

#include <QtCore/QByteArray>
#include <QtCore/QMutex>
#include <QtCore/QObject>

namespace morgoth {

/**
 * \brief The LogCollector class is responsible for saving all servers' logs.
 */
class LogCollector : public QObject {
    Q_OBJECT

    /**
     * \brief The directory where all the logs are saved in.
     */
    Q_PROPERTY(QString directory READ directory WRITE setDirectory)

public:
    explicit LogCollector(const QString& directory, QObject *parent = nullptr);

    virtual ~LogCollector();

    /**
     * \brief Appends a single line of log.
     * \note This method is thread-safe.
     */
    void log(const QString& line);

    void setDirectory(const QString& directory);
    const QString& directory() const { return m_directory; }

    /**
     * \brief Size (in bytes) of logs stored in memory before they get saved
     *  to a file.
     */
    static constexpr int MaxLogSize() { return 64 * 1024; }

public slots:
    /**
     * \brief Saves the current data to a file and clears it.
     * \note This method is thread-safe.
     */
    void save();

private:
    bool isLogDirWritable();
    QString logFileName();

    QMutex m_mutex;
    QString m_directory;
    QByteArray m_data;
    bool m_active = false;

};

} // namespace morgoth

#endif // LOGCOLLECTOR_H
