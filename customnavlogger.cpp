#include "customnavlogger.h"
#include <QDateTime>
#include <QThread>

// Define the static constants
const int CustomNavLogger::MAX_EVENTS = 100;
const int CustomNavLogger::MAX_PARAMS = 200;

CustomNavLogger& CustomNavLogger::instance()
{
    static CustomNavLogger instance;
    return instance;
}

CustomNavLogger::CustomNavLogger(QObject* parent) : QObject(parent)
{
    // Pre-allocate our fixed buffers to avoid any runtime allocations
}

CustomNavLogger::~CustomNavLogger()
{
    // Output any remaining logs
    if (m_activeScenario != SCENARIO_NONE) {
        flushLogs();
    }
}

void CustomNavLogger::beginScenario(NavScenarioType type)
{
    if (!m_enabled) return;
    
    QMutexLocker locker(&m_logMutex);
    
    // End previous scenario if active
    if (m_activeScenario != SCENARIO_NONE) {
        NavScenarioType oldType = m_activeScenario;
        m_activeScenario = SCENARIO_NONE;  // Prevent recursion
        endScenario();
        m_activeScenario = oldType;  // Restore for proper cleanup
    }
    
    // Start new scenario
    m_activeScenario = type;
    m_scenarioTimer.start();
    m_scenarioStartTime = QDateTime::currentMSecsSinceEpoch();
    m_eventCount = 0;
    m_paramCount = 0;
    
    // Log the start event while still under lock
    int eventIndex = m_eventCount % MAX_EVENTS;
    m_events[eventIndex].type = NAV_SCENARIO_START;
    m_events[eventIndex].scenario = type;
    m_events[eventIndex].index = -1;
    m_events[eventIndex].timestamp = 0;
    m_eventCount++;
}

void CustomNavLogger::endScenario()
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    QMutexLocker locker(&m_logMutex);
    
    // Log the end event
    logEvent(NAV_CALC_COMPLETE);
    
    // Output debug data
    flushLogs();
    
    // Reset scenario
    m_activeScenario = SCENARIO_NONE;
}

void CustomNavLogger::logEvent(NavEventType type, qint16 index)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    QMutexLocker locker(&m_logMutex);
    
    if (m_eventCount >= MAX_EVENTS) {
        qWarning() << "Event buffer full, dropping event:" << type;
        return;
    }
    
    // Use fixed-size circular buffer (no allocations)
    int eventIndex = m_eventCount % MAX_EVENTS;
    
    // Record the event
    m_events[eventIndex].type = type;
    m_events[eventIndex].scenario = m_activeScenario;
    m_events[eventIndex].index = index;
    m_events[eventIndex].timestamp = m_scenarioTimer.elapsed();
    
    m_eventCount++;
}

void CustomNavLogger::logEventWithParam(NavEventType type, qint16 index, const char* paramName, qint32 value)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    QMutexLocker locker(&m_logMutex);
    
    // First log the event while under the same lock
    if (m_eventCount < MAX_EVENTS) {
        int eventIndex = m_eventCount % MAX_EVENTS;
        m_events[eventIndex].type = type;
        m_events[eventIndex].scenario = m_activeScenario;
        m_events[eventIndex].index = index;
        m_events[eventIndex].timestamp = m_scenarioTimer.elapsed();
        m_eventCount++;
    }
    
    // Then add parameter if there's space
    if (m_paramCount >= MAX_PARAMS) {
        qWarning() << "Parameter buffer full, dropping param:" << paramName;
        return;
    }
    
    int paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = qChecksum(paramName, qstrlen(paramName)) % 255;
    m_params[paramIndex].value = value;
    m_params[paramIndex].floatValue = 0.0f;
    
    m_paramCount++;
}

void CustomNavLogger::logEventWithParam(NavEventType type, qint16 index, const char* paramName, float value)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    QMutexLocker locker(&m_logMutex);
    
    // Log event under same lock
    if (m_eventCount < MAX_EVENTS) {
        int eventIndex = m_eventCount % MAX_EVENTS;
        m_events[eventIndex].type = type;
        m_events[eventIndex].scenario = m_activeScenario;
        m_events[eventIndex].index = index;
        m_events[eventIndex].timestamp = m_scenarioTimer.elapsed();
        m_eventCount++;
    }
    
    // Add parameter if there's space
    if (m_paramCount >= MAX_PARAMS) {
        qWarning() << "Parameter buffer full, dropping param:" << paramName;
        return;
    }
    
    int paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = qChecksum(paramName, qstrlen(paramName)) % 255;
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = value;
    
    m_paramCount++;
}

void CustomNavLogger::logPosition(const char* checkpoint, qreal x, qreal y, qint16 index)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    // Log generic position event
    logEvent(NAV_CALC_POS, index);
    
    QMutexLocker locker(&m_logMutex);
    
    // Add X coordinate
    int paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = qChecksum(checkpoint, qstrlen(checkpoint)) % 255;
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(x);
    m_paramCount++;
    
    // Add Y coordinate
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 0; // marker for Y component of previous param
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(y);
    m_paramCount++;
}

void CustomNavLogger::logBoundsCheck(const char* boundType, qreal value, qreal min, qreal max, qreal result)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    // Log bounds check event
    logEvent(NAV_BOUNDS_CHECK);
    
    QMutexLocker locker(&m_logMutex);
    
    // Add value param
    int paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = qChecksum(boundType, qstrlen(boundType)) % 255;
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(value);
    m_paramCount++;
    
    // Add min param
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 1; // min marker
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(min);
    m_paramCount++;
    
    // Add max param
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 2; // max marker
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(max);
    m_paramCount++;
    
    // Add result param
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 3; // result marker
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(result);
    m_paramCount++;
}

void CustomNavLogger::logMetrics(int nodeCount, int textureCount, qint64 renderTimeMs)
{
    if (!m_enabled) return;
    
    QMutexLocker locker(&m_logMutex);
    
    m_nodeCount = nodeCount;
    m_textureCount = textureCount;
    m_renderTimeMs = renderTimeMs;
}

void CustomNavLogger::flushLogs()
{
    if (!m_enabled) return;
    
    QMutexLocker locker(&m_logMutex);
    
    if (m_eventCount == 0) return;
    
    // Generate scenario name
    QString scenarioName;
    switch (m_activeScenario) {
        case SCENARIO_NAV_RIGHT: scenarioName = "RIGHT"; break;
        case SCENARIO_NAV_LEFT: scenarioName = "LEFT"; break;
        case SCENARIO_NAV_UP: scenarioName = "UP"; break;
        case SCENARIO_NAV_DOWN: scenarioName = "DOWN"; break;
        case SCENARIO_ITEM_SELECTED: scenarioName = "SELECT"; break;
        default: scenarioName = "UNKNOWN"; break;
    }
    
    // Calculate total scenario duration
    quint32 totalDuration = m_scenarioTimer.elapsed();
    
    // Output a compact summary
    qDebug().nospace() << "\n[NAV/" << scenarioName << "] (" 
               << totalDuration << "ms) Events:" << m_eventCount 
               << " Nodes:" << m_nodeCount 
               << " Textures:" << m_textureCount;
    
    // Output detailed event timeline
    qDebug() << "  Timeline:";
    
    // Limit output to prevent console flooding
    int outputLimit = qMin(m_eventCount, MAX_EVENTS);
    int start = (m_eventCount > MAX_EVENTS) ? (m_eventCount - MAX_EVENTS) : 0;
    
    for (int i = start; i < m_eventCount && (i - start) < outputLimit; i++) {
        int idx = i % MAX_EVENTS;
        NavEvent &evt = m_events[idx];
        
        QString eventName;
        switch (evt.type) {
            case NAV_SCENARIO_START: eventName = "START"; break;
            case NAV_RIGHT_START: eventName = "RIGHT_START"; break;
            case NAV_RIGHT_END: eventName = "RIGHT_END"; break;
            case NAV_LEFT_START: eventName = "LEFT_START"; break;
            case NAV_LEFT_END: eventName = "LEFT_END"; break;
            case NAV_UP_START: eventName = "UP_START"; break;
            case NAV_UP_END: eventName = "UP_END"; break;
            case NAV_DOWN_START: eventName = "DOWN_START"; break;
            case NAV_DOWN_END: eventName = "DOWN_END"; break;
            case NAV_ANIM_START: eventName = "ANIM_START"; break;
            case NAV_ANIM_COMPLETE: eventName = "ANIM_COMPLETE"; break;
            case NAV_ANIM_ABORTED: eventName = "ANIM_ABORTED"; break;
            case NAV_INDEX_CHANGED: eventName = "INDEX_CHANGED"; break;
            case NAV_BOUNDS_CHECK: eventName = "BOUNDS_CHECK"; break;
            case NAV_CALC_POS: eventName = "CALC_POS"; break;
            case NAV_CALC_COMPLETE: eventName = "COMPLETE"; break;
            default: eventName = QString::number(evt.type); break;
        }
        
        QString indexStr = (evt.index >= 0) ? QString::number(evt.index) : "-";
        qDebug().nospace() << "    " << evt.timestamp << "ms: " << eventName 
                 << " idx:" << indexStr;
    }
}