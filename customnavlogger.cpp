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

CustomNavLogger::CustomNavLogger(QObject* parent) : QObject(parent),
    m_eventCount(0),
    m_paramCount(0),
    m_activeScenario(SCENARIO_NONE),
    m_enabled(true),
    m_nodeCount(0),
    m_textureCount(0),
    m_renderTimeMs(0),
    m_threadSafetyEnabled(false) // Initialize thread safety flag
{
    // Initialize arrays to prevent undefined behavior
    memset(m_events, 0, sizeof(m_events));
    memset(m_params, 0, sizeof(m_params));
}

CustomNavLogger::~CustomNavLogger()
{
    // Output any remaining logs
    if (m_activeScenario != SCENARIO_NONE) {
        flushLogs();
    }
}

bool CustomNavLogger::isScenarioActive() const
{
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    return m_activeScenario != SCENARIO_NONE;
}

void CustomNavLogger::beginScenario(NavScenarioType type)
{
    if (!m_enabled) return;
    
    qDebug() << "Attempting to begin scenario:" << type;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    // End previous scenario if active
    if (m_activeScenario != SCENARIO_NONE) {
        qDebug() << "Ending previous scenario before starting new one";
        
        // Need to release lock before recursive call
        NavScenarioType oldType = m_activeScenario;
        m_activeScenario = SCENARIO_NONE;
        
        if (m_threadSafetyEnabled) {
            locker.unlock();
            endScenario();
            locker.relock();
        } else {
            endScenario();
        }
        
        m_activeScenario = oldType;
    }
    
    // Start new scenario
    m_activeScenario = type;
    m_scenarioTimer.start();
    m_scenarioStartTime = QDateTime::currentMSecsSinceEpoch();
    m_eventCount = 0;
    m_paramCount = 0;
    
    // Log the start event
    NavEvent& evt = m_events[0];
    evt.type = NAV_SCENARIO_START;
    evt.scenario = type;
    evt.index = -1;
    evt.timestamp = 0;
    m_eventCount = 1;
    
    qDebug() << "Scenario started successfully";
}

void CustomNavLogger::endScenario()
{
    if (!m_enabled) return;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    if (m_activeScenario == SCENARIO_NONE) return;
    
    // Log the end event while we have the lock
    if (m_eventCount < MAX_EVENTS) {
        int eventIndex = m_eventCount % MAX_EVENTS;
        m_events[eventIndex].type = NAV_CALC_COMPLETE;
        m_events[eventIndex].scenario = m_activeScenario;
        m_events[eventIndex].index = -1;
        m_events[eventIndex].timestamp = m_scenarioTimer.elapsed();
        m_eventCount++;
    }
    
    // Flush logs while still under the same lock
    flushLogsNoLock();
    
    // Reset scenario state
    m_activeScenario = SCENARIO_NONE;
    m_eventCount = 0;
    m_paramCount = 0;
}

void CustomNavLogger::logEvent(NavEventType type, qint16 index)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    if (m_eventCount >= MAX_EVENTS) {
        qWarning() << "Event buffer full, dropping event:" << type;
        return;
    }
    
    quint32 timestamp = m_scenarioTimer.elapsed();
    
    // Use fixed-size circular buffer (no allocations)
    int eventIndex = m_eventCount % MAX_EVENTS;
    
    // Record the event with timestamp
    m_events[eventIndex].type = type;
    m_events[eventIndex].scenario = m_activeScenario;
    m_events[eventIndex].index = index;
    m_events[eventIndex].timestamp = timestamp;
    
    m_eventCount++;
}

void CustomNavLogger::logEventWithParamInt(NavEventType type, qint16 index, const char* paramName, qint32 value)
{
    if (!m_enabled) return;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    quint32 timestamp = m_scenarioTimer.elapsed();
    
    if (m_paramCount < MAX_PARAMS) {
        NavParam& param = m_params[m_paramCount++];
        param.type = qHash(paramName) & 0xFF;  // Store hash of param name
        param.value = value;
        param.timestamp = timestamp;
    }
    
    logEvent(type, index);
}

void CustomNavLogger::logEventWithParamFloat(NavEventType type, qint16 index, const char* paramName, float value) 
{
    if (!m_enabled) return;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    quint32 timestamp = m_scenarioTimer.elapsed();
    
    if (m_paramCount < MAX_PARAMS) {
        NavParam& param = m_params[m_paramCount++];
        param.type = qHash(paramName) & 0xFF;  // Store hash of param name
        param.value = 0;
        param.floatValue = value;
        param.timestamp = timestamp;
    }
    
    logEvent(type, index);
}

void CustomNavLogger::logPosition(const char* checkpoint, qreal x, qreal y, qint16 index)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    quint32 timestamp = m_scenarioTimer.elapsed();
    
    // Log position event first
    logEvent(NAV_CALC_POS, index);
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    // Log X coordinate first
    if (m_paramCount < MAX_PARAMS) {
        NavParam& param = m_params[m_paramCount++];
        param.type = 1; // X coordinate marker
        param.floatValue = static_cast<float>(x);
        param.timestamp = timestamp;
    }
    
    // Log Y coordinate second (if non-zero)
    if (y != 0.0 && m_paramCount < MAX_PARAMS) {
        NavParam& param = m_params[m_paramCount++];
        param.type = 2; // Y coordinate marker
        param.floatValue = static_cast<float>(y);
        param.timestamp = timestamp;
    }
}

void CustomNavLogger::logBoundsCheck(const char* boundType, qreal value, qreal min, qreal max, qreal result)
{
    if (!m_enabled || m_activeScenario == SCENARIO_NONE) return;
    
    // Log bounds check event
    logEvent(NAV_BOUNDS_CHECK);
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    quint32 timestamp = m_scenarioTimer.elapsed();
    
    // Add value param
    int paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = qChecksum(boundType, qstrlen(boundType)) % 255;
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(value);
    m_params[paramIndex].timestamp = timestamp;
    m_paramCount++;
    
    // Add min param
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 1; // min marker
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(min);
    m_params[paramIndex].timestamp = timestamp;
    m_paramCount++;
    
    // Add max param
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 2; // max marker
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(max);
    m_params[paramIndex].timestamp = timestamp;
    m_paramCount++;
    
    // Add result param
    paramIndex = m_paramCount % MAX_PARAMS;
    m_params[paramIndex].type = 3; // result marker
    m_params[paramIndex].value = 0;
    m_params[paramIndex].floatValue = static_cast<float>(result);
    m_params[paramIndex].timestamp = timestamp;
    m_paramCount++;
}

void CustomNavLogger::logMetrics(int nodeCount, int textureCount, qint64 renderTimeMs)
{
    if (!m_enabled) return;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    
    m_nodeCount = nodeCount;
    m_textureCount = textureCount;
    m_renderTimeMs = renderTimeMs;
}

void CustomNavLogger::flushLogs()
{
    if (!m_enabled) return;
    
    QMutexLocker locker(m_threadSafetyEnabled ? &m_logMutex : nullptr);
    flushLogsNoLock();
}

void CustomNavLogger::flushLogsNoLock()
{
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
    qDebug() << "NAV/" << scenarioName << "scenario started";
    qDebug().nospace() << "Duration:" << totalDuration << "ms, Events:" << m_eventCount 
               << ", Nodes:" << m_nodeCount 
               << ", Textures:" << m_textureCount;
    
    // Output detailed event timeline
    qDebug() << "  Timeline:";
    
    // Create lookup for params with proper timestamp matching
    QMap<quint32, QStringList> eventParams;
    for (int i = 0; i < m_paramCount; i++) {
        NavParam &param = m_params[i];
        QString value;
        
        // Format values according to type
        if (param.type == 1) { // X coordinate
            value = QString::number(param.floatValue, 'f', 2);
        }
        else if (param.type == 2) { // Y coordinate
            value = QString::number(param.floatValue, 'f', 2);
        }
        else {
            // For other parameters
            if (param.value != 0) {
                value = QString::number(param.value);
            } else {
                value = QString::number(param.floatValue, 'f', 2);
            }
        }
        
        // Only add non-empty values
        if (!value.isEmpty()) {
            eventParams[param.timestamp].append(value);
        }
    }
    
    // Limit output to prevent console flooding
    int outputLimit = qMin(m_eventCount, MAX_EVENTS);
    int start = (m_eventCount > MAX_EVENTS) ? (m_eventCount - MAX_EVENTS) : 0;
    
    // Output events with properly formatted parameters
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
        QString paramsStr;
        if (eventParams.contains(evt.timestamp)) {
            paramsStr = " [" + eventParams[evt.timestamp].join(", ") + "]";
        }
        
        qDebug().nospace() << "    " << evt.timestamp << "ms: \"" << eventName 
                          << "\" idx:\"" << indexStr << "\"" 
                          << (paramsStr.isEmpty() ? "" : paramsStr);
    }
}