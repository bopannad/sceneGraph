#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QVector>
#include <QDebug>
#include <QMutex>
#include <QtGlobal>

// Numeric codes for navigation events to avoid string allocations
enum NavEventType {
    NAV_SCENARIO_START = 1,
    NAV_RIGHT_START,
    NAV_RIGHT_END,
    NAV_LEFT_START,
    NAV_LEFT_END,
    NAV_UP_START,
    NAV_UP_END,
    NAV_DOWN_START,
    NAV_DOWN_END,
    NAV_ANIM_START,
    NAV_ANIM_COMPLETE,
    NAV_ANIM_ABORTED,
    NAV_INDEX_CHANGED,
    NAV_BOUNDS_CHECK,
    NAV_CALC_POS,
    NAV_CALC_COMPLETE
};

// Numeric codes for scenario types
enum NavScenarioType {
    SCENARIO_NONE = 0,
    SCENARIO_NAV_RIGHT,
    SCENARIO_NAV_LEFT,
    SCENARIO_NAV_UP,
    SCENARIO_NAV_DOWN,
    SCENARIO_ITEM_SELECTED
};

// Lightweight event structure (8 bytes)
struct NavEvent {
    quint8 type;          // Event type code
    quint8 scenario;      // Scenario type
    qint16 index;         // Current item index (-1 if N/A)
    quint32 timestamp;    // Milliseconds since scenario start
};

// Lightweight numeric parameter (12 bytes)
struct NavParam {
    quint8 type;          // Parameter type
    quint32 value;        // Integer value
    float floatValue;     // Float value (when needed)
};

class CustomNavLogger : public QObject
{
    Q_OBJECT
public:
    static CustomNavLogger& instance();
    
    // Start/end scenario tracking
    void beginScenario(NavScenarioType type);
    void endScenario();
    bool isScenarioActive() const { return m_activeScenario != SCENARIO_NONE; }
    
    // Log navigation events with minimal overhead
    void logEvent(NavEventType type, qint16 index = -1);
    void logEventWithParam(NavEventType type, qint16 index, const char* paramName, qint32 value);
    void logEventWithParam(NavEventType type, qint16 index, const char* paramName, float value);
    
    // Log position calculations
    void logPosition(const char* checkpoint, qreal x, qreal y, qint16 index = -1);
    
    // Log bounds checks
    void logBoundsCheck(const char* boundType, qreal value, qreal min, qreal max, qreal result);
    
    // Log metrics snapshot
    void logMetrics(int nodeCount, int textureCount, qint64 renderTimeMs);
    
    // Enable/disable logging
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    
    // Output all collected logs (called at scenario end)
    void flushLogs();
    
private:
    CustomNavLogger(QObject* parent = nullptr);
    ~CustomNavLogger();
    
    // Prevent copying
    Q_DISABLE_COPY(CustomNavLogger)
    
    // Scenario state
    NavScenarioType m_activeScenario = SCENARIO_NONE;
    QElapsedTimer m_scenarioTimer;
    quint64 m_scenarioStartTime = 0;
    
    // Fixed-size circular buffers (no dynamic allocation during logging)
    static const int MAX_EVENTS = 128;
    static const int MAX_PARAMS = 128;
    NavEvent m_events[MAX_EVENTS];
    NavParam m_params[MAX_PARAMS];
    int m_eventCount = 0;
    int m_paramCount = 0;
    
    // Thread safety for logging
    QMutex m_logMutex;
    
    // Metrics data
    int m_nodeCount = 0;
    int m_textureCount = 0;
    qint64 m_renderTimeMs = 0;
    
    bool m_enabled = true;
    
    // Helper for generating parameter strings in flushLogs
    QString getParamString(int paramIndex);
};

// Convenience macro for concise logging (no-op when logging disabled)
#define NAV_LOG_EVENT(type) \
    if (CustomNavLogger::instance().isEnabled()) { \
        CustomNavLogger::instance().logEvent(type); \
    }

#define NAV_LOG_EVENT_IDX(type, idx) \
    if (CustomNavLogger::instance().isEnabled()) { \
        CustomNavLogger::instance().logEvent(type, idx); \
    }

#define NAV_LOG_PARAM(type, idx, name, value) \
    if (CustomNavLogger::instance().isEnabled()) { \
        CustomNavLogger::instance().logEventWithParam(type, idx, name, value); \
    }

#define NAV_LOG_POS(checkpoint, x, y, idx) \
    if (CustomNavLogger::instance().isEnabled()) { \
        CustomNavLogger::instance().logPosition(checkpoint, x, y, idx); \
    }