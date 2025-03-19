#include "memoryusage.h"
#include <malloc.h>
#include <QDebug>
#include <QCoreApplication>
#include <QStack>
#include <QQuickWindow>
#include <QQuickView>
#include <QGuiApplication>
#include <QQuickItem>

MemoryUsage::MemoryUsage(QObject *parent)
    : QObject(parent), m_totalAllocated(0), m_totalFree(0), m_totalObjects(0), m_totalObjectsAlt(0),
      m_internalObjectCountEnabled(true), m_viewObjectCountEnabled(true)  // Initialize both to true
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
    
    // Only call counting methods if enabled
    if (m_internalObjectCountEnabled) {
        noOfQObjectsInternal();
    }
    if (m_viewObjectCountEnabled) {
        noOfQObjectsInView();
    }
}

void MemoryUsage::noOfQObjectsInternal()  // Renamed method
{
    // Method implementation remains the same
    int count = 0;
    QStack<QObject*> stack;
    stack.push(QCoreApplication::instance());

    while (!stack.isEmpty()) {
        QObject* current = stack.pop();
        ++count;

        const auto children = current->children();
        for (QObject* child : children) {
            stack.push(child);
        }
    }

    if (m_totalObjects != count) {
        m_totalObjects = count;
        emit objectCountChanged();
    }
}

void MemoryUsage::noOfQObjectsInView()  // Renamed method
{
    QList<QObject*> allObjects;
    QWindowList windows = QGuiApplication::topLevelWindows();
    for (QWindow* window : windows) {
        allObjects.append(window); // Add the window itself
        
        if (QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window)) {
            // Add the quick window
            allObjects.append(quickWindow);
            
            if (QQuickItem* rootItem = quickWindow->contentItem()) {
                // Add the root item
                allObjects.append(rootItem);
                
                // Add all child items recursively
                QList<QQuickItem*> items = rootItem->childItems();
                for (QQuickItem* item : items) {
                    allObjects.append(item);
                    // Get all QObject children of each item
                    allObjects.append(item->findChildren<QObject*>());
                    
                    // Recursively get all child items
                    QList<QQuickItem*> childItems = item->childItems();
                    while (!childItems.isEmpty()) {
                        QQuickItem* childItem = childItems.takeFirst();
                        allObjects.append(childItem);
                        allObjects.append(childItem->findChildren<QObject*>());
                        childItems.append(childItem->childItems());
                    }
                }
            }
        }
    }

    int count = allObjects.size() + 1; // +1 to include the QCoreApplication instance itself
    
    if (m_totalObjectsAlt != count) {
        m_totalObjectsAlt = count;
        emit objectCountChanged();
    }
}

void MemoryUsage::setInternalObjectCountEnabled(bool enabled)
{
    if (m_internalObjectCountEnabled != enabled) {
        m_internalObjectCountEnabled = enabled;
        if (enabled) {
            noOfQObjectsInternal();  // Update count immediately when enabled
        }
        emit internalObjectCountEnabledChanged();
    }
}

void MemoryUsage::setViewObjectCountEnabled(bool enabled)
{
    if (m_viewObjectCountEnabled != enabled) {
        m_viewObjectCountEnabled = enabled;
        if (enabled) {
            noOfQObjectsInView();  // Update count immediately when enabled
        }
        emit viewObjectCountEnabledChanged();
    }
}

QObject* MemoryUsage::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    static MemoryUsage instance;
    return &instance;
}