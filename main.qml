import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import Custom 1.0

Window {
    visible: true
    width: 1280
    height: 720
    color: "black"
    title: qsTr("Menu Gallery View")

    // Add helper function at Window level
    function isBackKey(key) {
        return key === Qt.Key_Back ||
               key === Qt.Key_Escape ||
               key === Qt.Key_Backspace
    }

    // Add focus scope to manage focus properly
    FocusScope {
        id: rootFocusScope
        anchors.fill: parent
        focus: true  // Give focus to the scope

        // Key handler for root focus scope
        Keys.onPressed: {
            console.log("Root focus scope received key:", event.key,
                       "Widget metrics focus:", widgetMetrics.activeFocus,
                       "App metrics focus:", appMetrics.activeFocus,
                       "Gallery has focus:", viewLoader.item ? viewLoader.item.activeFocus : false)
            if (event.key === Qt.Key_Space) {
            console.log("Space key pressed in root focus scope")
            // Add any additional logic for space key here
            event.accepted = true
            } else if (isBackKey(event.key)) {
            if (viewLoader.sourceComponent) {
                viewLoader.sourceComponent = null
                event.accepted = true
                console.log("Gallery unloaded due to back key in root focus scope")
            }
            } else if (event.key === Qt.Key_S) {
                appMetrics.showMetrics = !appMetrics.showMetrics
                event.accepted = true
            }
        }



        // Loader for dynamic loading
        Loader {
            id: viewLoader
            anchors.fill: parent
            focus: true  // Loader should have focus within the scope
            onLoaded: {
                console.log("Loader onLoaded called")
                if (item) {
                    focusTimer.start()
                }
            }

            Keys.onPressed: {
                console.log("Loader received key:", event.key)
                if (event.key === Qt.Key_Left) {
                    console.log("Left key pressed in loader")
                    // Add any additional logic for left key here
                    event.accepted = true
                } else if (event.key === Qt.Key_Right) {
                    console.log("Right key pressed in loader")
                    // Add any additional logic for right key here
                    event.accepted = true
                } else if (isBackKey(event.key)) {
                    if (viewLoader.sourceComponent) {
                        viewLoader.sourceComponent = null
                        event.accepted = true
                        console.log("Gallery unloaded due to back key in loader")
                    }
                }
            }
            onStatusChanged: {
                console.log("Loader status:", status)
                if (status === Loader.Null) {
                    rootFocusScope.focus = true
                    console.log("Loader unloaded, focus returned to root scope")
                }
            }

            // Add Timer to force focus after loading
            Timer {
                id: focusTimer
                interval: 100  // Short delay to ensure component is ready
                repeat: false
                onTriggered: {
                    console.log("Focus timer triggered")
                    if (viewLoader.item) {
                        viewLoader.item.forceActiveFocus()
                        console.log("Force focus given to gallery")
                    }
                }
            }
        }

        // Launch button
        Button {
            id: launchButton
            anchors.centerIn: parent
            text: "Launch Gallery"
            visible: !viewLoader.sourceComponent
            onClicked: {
            viewLoader.sourceComponent = galleryComponent
            }
        }


    }

    // Window level key handling
    Keys.onPressed: {
        console.log("Window received key:", event.key, "Active focus item:", Window.activeFocusItem)
        if (isBackKey(event.key)) {
            if (viewLoader.sourceComponent) {
                viewLoader.sourceComponent = null
                event.accepted = true
                console.log("Gallery unloaded due to back key")
            }
        }
    }

    // Component definition for the gallery view
    Component {
        id: galleryComponent

        CustomImageListView {
            anchors.fill: parent
            jsonSource: "qrc:/data/embeddedHubMenu.json"
            focus: true
            clip: true
            startPositionX: 50


            // Only handle specific keys, let others propagate
            Keys.onPressed: {
                console.log("Gallery received key:", event.key)
                if (event.key === Qt.Key_M) {
                    widgetMetrics.enabled = !widgetMetrics.enabled
                    event.accepted = true
                } else if (event.key === Qt.Key_N) {
                    enableNodeMetrics = !enableNodeMetrics
                    event.accepted = true
                } else if (event.key === Qt.Key_T) {
                    enableTextureMetrics = !enableTextureMetrics
                    event.accepted = true
                } else if (event.key === Qt.Key_R) {
                    enableTextureMemoryMetrics = !enableTextureMemoryMetrics
                    event.accepted = true
                }
            }

            onLinkActivated: function(action, url) {
                console.log("Link activated - Action:", action, "URL:", url)
                switch(action) {
                    case "OK":
                        console.log("Processing OK action with URL:", url)
                        // Add your content playback logic here
                        break

                    case "info":
                        console.log("Processing info action with URL:", url)
                        // Add your info display logic here
                        break

                    default:
                        console.log("Unknown action type:", action)
                        break
                }
            }

            onAssetFocused: function(assetData) {
                console.log("Asset focused changed")
                //console.log("Asset focused, complete data:", JSON.stringify(assetData, null, 2))
                // Now you have access to all JSON fields exactly as they were in the source
            }

            onMoodImageSelected: function(url) {
                console.log("Mood image URL:", url)
                // Handle the mood image URL here
            }

            // Add focus handling
            onFocusChanged: {
                if (focus) {
                    console.log("CustomImageListView gained focus")
                } else {
                    console.log("CustomImageListView lost focus")
                }
            }


            Component.onCompleted: {
                console.log("Gallery component completed")
                forceActiveFocus()
            }
        }
    }

    // Widget metrics overlay - keep this dependent on loader
    WidgetMetricsOverlay {
        id: widgetMetrics
        imageListView: viewLoader.item
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
            rightMargin: 30
        }
        z: 1000
        enabled: viewLoader.item ? true : false
    }

    // App metrics overlay - remove loader dependency
    AppMetricsOverlay {
        id: appMetrics
        anchors {
            right: parent.right
            top: widgetMetrics.bottom
            margins: 20
            rightMargin: 30
            topMargin: 10
        }
        z: 1000
        enabled: true  // Always enabled
        visible: enabled  // Always visible when enabled
        showMetrics: false  // Toggle with S key
    }

    // Test objects loader
    Loader {
        id: testLoader
        active: viewLoader.sourceComponent ? true : false
        sourceComponent: Column {
            id: testColumn
            anchors.left: parent.left
            anchors.top: parent.top
            spacing: 10
            Repeater {
                model: 10
                delegate: Row {
                    spacing: 10
                    Repeater {
                        model: 50
                        delegate: Rectangle {
                            width: 50
                            height: 50
                            color: "red"
                        }
                    }
                }
            }
        }
    }
    
}
