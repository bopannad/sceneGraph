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
    Q_PROPERTY(int totalObjects READ totalObjects NOTIFY objectCountChanged)
    Q_PROPERTY(int totalObjectsAlt READ totalObjectsAlt NOTIFY objectCountChanged)
    Q_PROPERTY(bool internalObjectCountEnabled READ internalObjectCountEnabled WRITE setInternalObjectCountEnabled NOTIFY internalObjectCountEnabledChanged)
    Q_PROPERTY(bool viewObjectCountEnabled READ viewObjectCountEnabled WRITE setViewObjectCountEnabled NOTIFY viewObjectCountEnabledChanged)

public:
    explicit MemoryUsage(QObject *parent = nullptr);

    int totalAllocated() const { return m_totalAllocated; }
    int totalFree() const { return m_totalFree; }
    int totalObjects() const { return m_totalObjects; }
    int totalObjectsAlt() const { return m_totalObjectsAlt; }
    bool internalObjectCountEnabled() const { return m_internalObjectCountEnabled; }
    void setInternalObjectCountEnabled(bool enabled);
    bool viewObjectCountEnabled() const { return m_viewObjectCountEnabled; }
    void setViewObjectCountEnabled(bool enabled);

    static QObject* qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

public slots:
    void updateMemoryStats();
    void noOfQObjectsInternal();  // Renamed from noOfQObjectsInCoreApplication
    void noOfQObjectsInView();  // Renamed from countAllQObjectsUsingFindChildren

signals:
    void memoryStatsUpdated();
    void objectCountChanged();
    void internalObjectCountEnabledChanged();
    void viewObjectCountEnabledChanged();

private:
    int m_totalAllocated;
    int m_totalFree;
    int m_totalObjects;
    int m_totalObjectsAlt;
    bool m_internalObjectCountEnabled = true;
    bool m_viewObjectCountEnabled = true;
};

#endif // MEMORYUSAGE_H