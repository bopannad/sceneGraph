import QtQuick 2.5
import Custom 1.0

Rectangle {
    id: root
    color: Qt.rgba(0, 0, 0, 0.6)
    radius: 4
    border.color: "#404040"
    border.width: 1

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
            visible: false
            text: "Scene Graph Nodes: 0"
        }
        
        Text {
            id: textureCountText
            color: "#ffffff"
            font.pixelSize: 12
            visible: false
            text: "Active Textures: 0"
        }

        Text {
            id: textureMemoryText
            color: "#ffffff"
            font.pixelSize: 12
            visible: false
            text: "Texture Memory: 0 MB"
        }
        
        Text {
            id: controlText
            color: "#808080"
            font.pixelSize: 10
            text: "Widget Controls:\n" +
                  "'N' - Toggle Node Count\n" +
                  "'T' - Toggle Texture Count\n" +
                  "'M' - Toggle Memory Usage"
        }
    }
    
    Timer {
        interval: updateInterval
        running: root.visible && imageListView !== null
        repeat: true
        onTriggered: {
            if (imageListView) {
                nodeCountText.visible = imageListView.enableNodeMetrics
                if (imageListView.enableNodeMetrics) {
                    nodeCountText.text = "Scene Graph Nodes: " + imageListView.nodeCount
                }
                
                textureCountText.visible = imageListView.enableTextureMetrics
                if (imageListView.enableTextureMetrics) {
                    textureCountText.text = "Active Textures: " + imageListView.textureCount
                }

                textureMemoryText.visible = imageListView.enableTextureMemoryMetrics
                if (imageListView.enableTextureMemoryMetrics) {
                    var memoryMB = (imageListView.textureMemoryUsage / (1024 * 1024)).toFixed(2)
                    textureMemoryText.text = "Texture Memory: " + memoryMB + " MB"
                }
            }
        }
    }
}
