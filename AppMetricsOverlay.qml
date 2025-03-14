import QtQuick 2.5
import Custom 1.0

Rectangle {
    id: root
    color: Qt.rgba(0, 0, 0, 0.6)
    radius: 4
    border.color: "#404040"
    border.width: 1

    property bool enabled: true
    property int updateInterval: 1000
    property bool showMetrics: false
    width: metrics.width + 20
    height: metrics.height + 20
    
    property var memoryUsage: MemoryUsage
    
    visible: enabled

    Column {
        id: metrics
        anchors.centerIn: parent
        spacing: 4
        
        Text {
            id: systemMemoryHeader
            color: "#80ff80"
            font.pixelSize: 12
            visible: showMetrics
            text: "System Memory Usage:"
        }
        
        Text {
            id: systemAllocatedText
            color: "#ffffff"
            font.pixelSize: 12
            visible: showMetrics
            text: "Allocated: 0 MB"
        }
        
        Text {
            id: systemFreeText
            color: "#ffffff"
            font.pixelSize: 12
            visible: showMetrics
            text: "Free: 0 MB"
        }
        
        Text {
            id: objectCountStackText
            color: "#ffffff"
            font.pixelSize: 12
            visible: showMetrics
            text: "QObjects(Stack): " + (memoryUsage ? memoryUsage.totalObjects : 0)
        }

        Text {
            id: objectCountFindText
            color: "#ffffff"
            font.pixelSize: 12
            visible: showMetrics
            text: "QObjects(Find): " + (memoryUsage ? memoryUsage.totalObjectsAlt : 0)
        }
        
        Text {
            id: controlText
            color: "#808080"
            font.pixelSize: 10
            text: "App Controls:\n" +
                  "'S' - Toggle System Metrics\n" +
                  "'ESC' - Hide Overlay"
        }
    }
    
    Timer {
        interval: updateInterval
        running: root.visible && showMetrics
        repeat: true
        onTriggered: {
            if (memoryUsage) {
                memoryUsage.updateMemoryStats()
                var allocatedMB = (memoryUsage.totalAllocated / (1024 * 1024)).toFixed(2)
                var freeMB = (memoryUsage.totalFree / (1024 * 1024)).toFixed(2)
                systemAllocatedText.text = "Allocated: " + allocatedMB + " MB"
                systemFreeText.text = "Free: " + freeMB + " MB"
                objectCountStackText.text = "QObjects(Stack): " + memoryUsage.totalObjects
                objectCountFindText.text = "QObjects(Find): " + memoryUsage.totalObjectsAlt
            }
        }
    }
}
