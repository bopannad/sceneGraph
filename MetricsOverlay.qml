import QtQuick 2.5

Rectangle {
    id: root
    color: Qt.rgba(0, 0, 0, 0.6)
    radius: 4
    border.color: "#404040"
    border.width: 1

    // Properties
    property var imageListView: null
    property bool enabled: true
    property int updateInterval: 1000
    width: metrics.width + 20
    height: metrics.height + 20
    
    visible: enabled && imageListView !== null
    
    Column {
        id: metrics
        anchors.centerIn: parent
        spacing: 4
        
        Text {
            id: nodeCountText
            color: "#ffffff"
            font.pixelSize: 12
            visible: false  // Initially hidden, Timer will control visibility
            text: "Scene Graph Nodes: 0"  // Default text, Timer will update
        }
        
        Text {
            id: textureCountText
            color: "#ffffff"
            font.pixelSize: 12
            visible: false  // Initially hidden, Timer will control visibility
            text: "Active Textures: 0"  // Default text, Timer will update
        }

        Text {
            id: textureMemoryText
            color: "#ffffff"
            font.pixelSize: 12
            visible: false  // Initially hidden, Timer will control visibility
            text: "Texture Memory: 0 MB"  // Default text, Timer will update
        }
        
        Text {
            id: controlText
            color: "#808080"
            font.pixelSize: 10
            text: "Press 'N' for nodes\nPress 'T' for textures\nPress 'M' for memory"
        }
    }
    
    Timer {
        interval: updateInterval
        running: root.visible && imageListView !== null
        repeat: true
        onTriggered: {
            if (imageListView) {
                // Update node metrics
                nodeCountText.visible = imageListView.enableNodeMetrics
                if (imageListView.enableNodeMetrics) {
                    nodeCountText.text = "Scene Graph Nodes: " + imageListView.nodeCount
                }
                
                // Update texture metrics
                textureCountText.visible = imageListView.enableTextureMetrics
                if (imageListView.enableTextureMetrics) {
                    textureCountText.text = "Active Textures: " + imageListView.textureCount
                }

                // Update memory metrics using the property instead of the function
                textureMemoryText.visible = imageListView.enableTextureMemoryMetrics
                if (imageListView.enableTextureMemoryMetrics) {
                    var memoryMB = (imageListView.textureMemoryUsage / (1024 * 1024)).toFixed(2)
                    textureMemoryText.text = "Texture Memory: " + memoryMB + " MB"
                }
            }
        }
    }

    // Add key handling for individual metric toggles
    Keys.onPressed: {
        if (event.key === Qt.Key_N && imageListView) {
            imageListView.enableNodeMetrics = !imageListView.enableNodeMetrics
            event.accepted = true
        } else if (event.key === Qt.Key_T && imageListView) {
            imageListView.enableTextureMetrics = !imageListView.enableTextureMetrics
            event.accepted = true
        } else if (event.key === Qt.Key_M && imageListView) {
            imageListView.enableTextureMemoryMetrics = !imageListView.enableTextureMemoryMetrics
            event.accepted = true
        }
    }

    Component.onCompleted: {
        console.log("MetricsOverlay initialized")
    }
}
