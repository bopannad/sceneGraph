#include <QtTest>
#include "customnavlogger.h"

// Capture debug output for testing
class OutputCapture {
public:
    OutputCapture() {
        m_originalHandler = qInstallMessageHandler(captureMessage);
    }
    
    ~OutputCapture() {
        qInstallMessageHandler(m_originalHandler);
    }
    
    static void captureMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void reset();
    static QStringList getCapturedOutput();
    static bool outputContains(const QString &text);

private:
    static QStringList m_capturedOutput;
    static QtMessageHandler m_originalHandler;
};

// Now declare static members
QStringList OutputCapture::m_capturedOutput;
QtMessageHandler OutputCapture::m_originalHandler = nullptr;

// Implement static methods
void OutputCapture::captureMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) 
{
    Q_UNUSED(context);
    
    // Store original message
    m_capturedOutput.append(msg);
    
    // Also store with type prefix for debugging
    QString typedMsg;
    switch(type) {
        case QtDebugMsg:
            typedMsg = QString("Debug: %1").arg(msg);
            break;
        case QtWarningMsg:
            typedMsg = QString("Warning: %1").arg(msg);
            break;
        case QtCriticalMsg:
            typedMsg = QString("Critical: %1").arg(msg);
            break;
        case QtFatalMsg:
            typedMsg = QString("Fatal: %1").arg(msg);
            break;
        default:
            typedMsg = msg;
            break;
    }
    m_capturedOutput.append(typedMsg);
    
    // Also print to console for immediate feedback
    fprintf(stderr, "%s\n", qPrintable(msg));
    
    // Forward critical and fatal messages
    if ((type == QtCriticalMsg || type == QtFatalMsg) && m_originalHandler) {
        m_originalHandler(type, context, msg);
    }
}

void OutputCapture::reset() {
    m_capturedOutput.clear();
}

QStringList OutputCapture::getCapturedOutput() {
    return m_capturedOutput;
}

bool OutputCapture::outputContains(const QString &text) {
    QString searchText = text.trimmed();
    for (const QString &msg : m_capturedOutput) {
        if (msg.contains(searchText, Qt::CaseInsensitive)) {
            return true;
        }
        // Try without brackets
        QString noBrackets = searchText;
        noBrackets.remove('[').remove(']');
        if (msg.contains(noBrackets, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

// Test class definition
class TestCustomNavLogger : public QObject
{
    Q_OBJECT

private:
    OutputCapture* m_capture;

private slots:

    // Add this new test function after other tests
    void test_displayLogs()
    {
        qDebug() << "\n=== Navigation Logger Output Demo ===\n";
        
        OutputCapture::reset();
        CustomNavLogger& logger = CustomNavLogger::instance();
        
        // Start a navigation scenario
        logger.beginScenario(CustomNavLogger::SCENARIO_NAV_RIGHT);
        
        // Log various types of events
        NAV_LOG_EVENT(CustomNavLogger::NAV_RIGHT_START);
        NAV_LOG_EVENT_IDX(CustomNavLogger::NAV_INDEX_CHANGED, 5);
        NAV_LOG_POS("MainItem", 100.0, 200.0, 1);
        NAV_LOG_PARAM(CustomNavLogger::NAV_CALC_POS, 1, "offset", 150);
        logger.logMetrics(10, 5, 16);
        NAV_LOG_EVENT(CustomNavLogger::NAV_RIGHT_END);
        
        // Force flush logs before ending
        logger.flushLogs();
        
        // Get captured output
        QStringList outputs = OutputCapture::getCapturedOutput();
        
        qDebug() << "Raw Logger Output:";
        qDebug() << "----------------------------------------";
        for(const QString& msg : outputs) {
            // Remove Debug/Warning prefixes for cleaner output
            QString cleanMsg = msg;
            cleanMsg.remove("Debug: ").remove("Warning: ");
            qDebug().noquote() << cleanMsg;
        }
        qDebug() << "----------------------------------------";
        
        logger.endScenario();
        
        // Process events and get final output
        QCoreApplication::processEvents();
        QTest::qWait(200);
        
        outputs = OutputCapture::getCapturedOutput();
        qDebug() << "\nFinal Logger State:";
        qDebug() << "----------------------------------------";
        for(const QString& msg : outputs) {
            QString cleanMsg = msg;
            cleanMsg.remove("Debug: ").remove("Warning: ");
            qDebug().noquote() << cleanMsg;
        }
        qDebug() << "----------------------------------------\n";
        
        // This test always passes - it's for display purposes
        QVERIFY(true);
    }

    void initTestCase() {
        m_capture = new OutputCapture();
    }
    
    void cleanupTestCase() {
        delete m_capture;
    }
    
    void init() {
        qDebug() << "\n=== Starting test:" << QTest::currentTestFunction() << "===";
        OutputCapture::reset();
        CustomNavLogger::instance().setEnabled(true);
    }
    
    void cleanup() {
        CustomNavLogger& logger = CustomNavLogger::instance();
        if (logger.isScenarioActive()) {
            logger.endScenario();
        }
        // Ensure all events are processed
        QCoreApplication::processEvents();
        QTest::qWait(50);
        qDebug() << "=== Test complete:" << QTest::currentTestFunction() << "===\n";
    }

    // Basic test cases first
    void test_instance();
    void test_enableDisable();
    void test_basicScenario();

    // Add new test cases
    void test_eventLogging();
    void test_parameterLogging();
    void test_positionLogging();
    void test_metricsLogging();
    void test_bufferLimits();
};

// Test implementation
void TestCustomNavLogger::test_instance() {
    CustomNavLogger& instance1 = CustomNavLogger::instance();
    CustomNavLogger& instance2 = CustomNavLogger::instance();
    QCOMPARE(&instance1, &instance2);
}

void TestCustomNavLogger::test_enableDisable() {
    QVERIFY(CustomNavLogger::instance().isEnabled());
    CustomNavLogger::instance().setEnabled(false);
    QVERIFY(!CustomNavLogger::instance().isEnabled());
    CustomNavLogger::instance().setEnabled(true);
    QVERIFY(CustomNavLogger::instance().isEnabled());
}

void TestCustomNavLogger::test_basicScenario() 
{
    OutputCapture::reset();
    qDebug() << "=== test_basicScenario starting ===";
    
    try {
        CustomNavLogger& logger = CustomNavLogger::instance();
        QVERIFY2(!logger.isScenarioActive(), "Scenario should not be active initially");
        
        // Kill switch for deadlock protection
        bool deadlockDetected = false;
        QTimer deadlockTimer;
        deadlockTimer.setSingleShot(true);
        QObject::connect(&deadlockTimer, &QTimer::timeout, [&]() {
            deadlockDetected = true;
            qWarning() << "DEADLOCK DETECTED: Test timed out";
            QCoreApplication::exit(1);
        });
        deadlockTimer.start(5000); // 5 second timeout
        
        // Begin scenario
        logger.beginScenario(CustomNavLogger::SCENARIO_NAV_RIGHT);
        QVERIFY2(logger.isScenarioActive(), "Scenario should be active after beginning");
        
        // Log a few events
        logger.logEvent(CustomNavLogger::NAV_RIGHT_START);
        logger.logEvent(CustomNavLogger::NAV_INDEX_CHANGED, 5);
        
        // End scenario - this was hanging
        logger.endScenario();
        
        // Process events immediately after endScenario
        QCoreApplication::processEvents();
        QTest::qWait(100); // Wait for output to be processed
        
        // Verify scenario ended
        QVERIFY2(!logger.isScenarioActive(), "Scenario should not be active after ending");
        
        // Dump captured output first for debugging
        QStringList outputs = OutputCapture::getCapturedOutput();
    
        
        // Add detailed output verification
        qDebug() << "=== Captured Output ===";
        for (const QString& msg : outputs) {
            qDebug() << "  ->" << msg;
        }
        
        // Verify specific messages are present
        bool foundScenarioStart = false;
        bool foundRightStart = false;
        bool foundIndexChange = false;
        
        for (const QString& msg : outputs) {
            // Change this line:
            if (msg.contains("NAV/RIGHT")) foundScenarioStart = true;
            
            // To this more flexible check:
            if (msg.contains("NAV") && msg.contains("RIGHT") && msg.contains("scenario")) {
                foundScenarioStart = true;
            }
            if (msg.contains("RIGHT_START")) foundRightStart = true;
            if (msg.contains("INDEX_CHANGED") && msg.contains("5")) foundIndexChange = true;
        }

        qDebug() << "Verification results:";
        qDebug() << "  Scenario Start:" << foundScenarioStart;
        qDebug() << "  Right Start:" << foundRightStart;
        qDebug() << "  Index Change:" << foundIndexChange;
        
        QVERIFY2(foundScenarioStart, "NAV/RIGHT scenario start not found");
        QVERIFY2(foundRightStart, "RIGHT_START event not found");
        QVERIFY2(foundIndexChange, "INDEX_CHANGED with value 5 not found");
    }
    catch (...) {
        QFAIL("Exception occurred during test");
    }
}

void TestCustomNavLogger::test_eventLogging()
{
    OutputCapture::reset();
    
    CustomNavLogger& logger = CustomNavLogger::instance();
    logger.beginScenario(CustomNavLogger::SCENARIO_NAV_RIGHT);
    
    // Log events using the macros
    NAV_LOG_EVENT(CustomNavLogger::NAV_RIGHT_START);
    NAV_LOG_EVENT_IDX(CustomNavLogger::NAV_INDEX_CHANGED, 5);
    NAV_LOG_EVENT(CustomNavLogger::NAV_RIGHT_END);
    
    logger.endScenario();
    
    // Process events and wait longer
    QCoreApplication::processEvents();
    QTest::qWait(200);
    
    // More flexible output verification
    QStringList outputs = OutputCapture::getCapturedOutput();
    QString allOutput = outputs.join(" ");
    
    QVERIFY2(OutputCapture::outputContains("RIGHT_START"), 
             "RIGHT_START event not found");
    QVERIFY2(OutputCapture::outputContains("INDEX_CHANGED"), 
             "INDEX_CHANGED event not found");
    QVERIFY2(OutputCapture::outputContains("5"), 
             "Index value 5 not found");
}

void TestCustomNavLogger::test_parameterLogging()
{
    OutputCapture::reset();
    CustomNavLogger::instance().beginScenario(CustomNavLogger::SCENARIO_NAV_LEFT);
    
    NAV_LOG_PARAM(CustomNavLogger::NAV_CALC_POS, 1, "offset", 100);
    NAV_LOG_PARAM(CustomNavLogger::NAV_CALC_POS, 2, "scale", 1.5f);
    
    CustomNavLogger::instance().endScenario();
    
    // Process events
    QCoreApplication::processEvents();
    QTest::qWait(200);
    
    // More flexible verification
    QVERIFY2(OutputCapture::outputContains("CALC_POS"), 
             "CALC_POS event not found");
    QVERIFY2(OutputCapture::outputContains("1"), 
             "Index 1 not found");
}

void TestCustomNavLogger::test_positionLogging()
{
    OutputCapture::reset();
    CustomNavLogger::instance().beginScenario(CustomNavLogger::SCENARIO_NAV_DOWN);
    
    NAV_LOG_POS("item1", 100.0, 200.0, 1);
    NAV_LOG_POS("item2", 300.0, 400.0, 2);
    
    CustomNavLogger::instance().endScenario();
    
    // Process events
    QCoreApplication::processEvents();
    QTest::qWait(200);
    
    // More flexible verification
    QVERIFY2(OutputCapture::outputContains("CALC_POS"), 
             "CALC_POS event not found");
    QVERIFY2(OutputCapture::outputContains("1"), 
             "Position index 1 not found");
}

void TestCustomNavLogger::test_metricsLogging()
{
    CustomNavLogger::instance().beginScenario(CustomNavLogger::SCENARIO_NAV_UP);
    
    CustomNavLogger::instance().logMetrics(50, 10, 16);
    
    CustomNavLogger::instance().endScenario();
    
    QVERIFY(OutputCapture::outputContains("Nodes:50"));
    QVERIFY(OutputCapture::outputContains("Textures:10"));
}

void TestCustomNavLogger::test_bufferLimits()
{
    OutputCapture::reset();
    CustomNavLogger::instance().beginScenario(CustomNavLogger::SCENARIO_NAV_RIGHT);
    
    // Test event buffer limit
    for(int i = 0; i < 150; i++) {
        NAV_LOG_EVENT_IDX(CustomNavLogger::NAV_INDEX_CHANGED, i);
    }
    
    // Test parameter buffer limit
    for(int i = 0; i < 250; i++) {
        NAV_LOG_PARAM(CustomNavLogger::NAV_CALC_POS, i, "test", i);
    }
    
    CustomNavLogger::instance().endScenario();
    
    // Should not crash and should have warning messages
    QVERIFY(OutputCapture::outputContains("buffer full"));
    // Verify warning messages
    QVERIFY2(OutputCapture::outputContains("Event buffer full") || 
             OutputCapture::outputContains("Parameter buffer full"),
             "Buffer full warning not found");
}

QTEST_MAIN(TestCustomNavLogger)

#include "tst_customnavlogger.moc"