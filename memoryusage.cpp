#include "memoryusage.h"
#include <malloc.h>
#include <QDebug>

MemoryUsage::MemoryUsage(QObject *parent)
    : QObject(parent), m_totalAllocated(0), m_totalFree(0)
{
}

void MemoryUsage::updateMemoryStats()
{
#if defined(__GLIBC__)
    struct mallinfo mi = mallinfo();
    m_totalAllocated = mi.uordblks;
    m_totalFree = mi.fordblks;
#else
    // Windows implementation
    _HEAPINFO hinfo;
    int heapstatus;
    hinfo._pentry = NULL;
    m_totalAllocated = 0;
    m_totalFree = 0;
    
    while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
    {
        if(hinfo._useflag == _USEDENTRY)
            m_totalAllocated += hinfo._size;
        else
            m_totalFree += hinfo._size;
    }
#endif
    emit memoryStatsUpdated();
}

QObject* MemoryUsage::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    static MemoryUsage instance;
    return &instance;
}