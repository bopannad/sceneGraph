#ifndef MEMORYUSAGE_H
#define MEMORYUSAGE_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#ifdef _WIN32
#include <malloc.h>
#endif

class MemoryUsage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int totalAllocated READ totalAllocated NOTIFY memoryStatsUpdated)
    Q_PROPERTY(int totalFree READ totalFree NOTIFY memoryStatsUpdated)

public:
    explicit MemoryUsage(QObject *parent = nullptr);

    int totalAllocated() const { return m_totalAllocated; }
    int totalFree() const { return m_totalFree; }

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

public slots:
    void updateMemoryStats();

signals:
    void memoryStatsUpdated();

private:
    int m_totalAllocated;
    int m_totalFree;
};

#endif // MEMORYUSAGE_H