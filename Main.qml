import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import GstInspect

pragma ComponentBehavior : Bound

ApplicationWindow {

    Material.theme: Material.Dark
    Material.accent: Material.Lime

    id: root
    width: 1200
    height: 800
    visible: true
    title: "GStreamer Element Browser"

    property alias browser: elementBrowser

    GstElementBrowser {
        id: elementBrowser
        Component.onCompleted: refreshElements()
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // Left panel - Element selection
        Rectangle {
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 250
            Material.theme: Material.Dark
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // Header
                Text {
                    text: "GStreamer Elements"
                    font.bold: true
                    font.pointSize: 14
                }

                // Search/Filter
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: "Search elements..."
                    onTextChanged: elementBrowser.filterElements(text)
                }

                // Refresh button
                Button {
                    Layout.fillWidth: true
                    text: elementBrowser.isLoading ? "Loading..." : "Refresh Elements"
                    enabled: !elementBrowser.isLoading
                    onClicked: elementBrowser.refreshElements()
                }

                // Element list
                ListView {
                    id: elementList
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    model: elementBrowser.elementNames
                    currentIndex: -1

                    delegate: ItemDelegate {
                        id: delegate
                        width: elementList.width
                        height: 40
                        required property string modelData
                        required property int index
                        Rectangle {
                            anchors.fill: parent
                            color: parent.hovered ? "#e3f2fd" : (delegate.modelData.selectedElement === modelData ? "#bbdefb" : "transparent")
                            border.color: delegate.modelData.selectedElement
                                          === delegate.modelData ? "#2196f3" : "transparent"

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                text: delegate.modelData
                                font.family: "monospace"
                            }
                        }

                        onClicked: {
                            modelData.selectedElement = delegate.modelData
                            modelData.currentIndex = delegate.index
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }

                // Status
                Text {
                    Layout.fillWidth: true
                    text: `${elementBrowser.elementNames.length} elements available`
                    color: "#666"
                    font.pointSize: 9
                }
            }
        }

        // Right panel - Element details
        Rectangle {
            SplitView.fillWidth: true

            ScrollView {
                anchors.fill: parent
                anchors.margins: 10
                contentWidth: availableWidth

                ColumnLayout {
                    width: parent.width
                    spacing: 20

                    // Element info header
                    Rectangle {
                        Layout.fillWidth: true
                        height: elementInfoColumn.height + 20
                        radius: 4

                        ColumnLayout {
                            id: elementInfoColumn
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 10
                            spacing: 5

                            Text {
                                text: elementBrowser.selectedElement
                                      || "Select an element"
                                font.bold: true
                                font.pointSize: 16
                                color: elementBrowser.selectedElement ? "#000" : "#666"
                            }

                            Text {
                                text: elementBrowser.elementDescription
                                font.pointSize: 10
                                color: "#666"
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                                visible: text.length > 0
                            }

                            RowLayout {
                                visible: elementBrowser.elementClassification.length > 0

                                Text {
                                    text: "Classification:"
                                    font.bold: true
                                    font.pointSize: 9
                                }

                                Text {
                                    text: elementBrowser.elementClassification
                                    font.pointSize: 9
                                    color: "#007acc"
                                }
                            }

                            RowLayout {
                                visible: elementBrowser.elementAuthor.length > 0

                                Text {
                                    text: "Author:"
                                    font.bold: true
                                    font.pointSize: 9
                                }

                                Text {
                                    text: elementBrowser.elementAuthor
                                    font.pointSize: 9
                                    color: "#666"
                                }
                            }
                        }
                    }

                    // Tabs for Properties and Pads
                    TabBar {
                        id: tabBar
                        Layout.fillWidth: true

                        TabButton {
                            text: `Properties (${elementBrowser.propertyModel.rowCount(
                                      )})`
                        }
                        TabButton {
                            text: `Pad Templates (${elementBrowser.padModel.rowCount(
                                      )})`
                        }
                    }

                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: tabBar.currentIndex

                        // Properties tab
                        Item {
                            ListView {
                                id: propertiesTabListView
                                anchors.fill: parent
                                model: elementBrowser.propertyModel
                                spacing: 1

                                delegate: Rectangle {
                                    id: propertiesTab
                                    required property int index
                                    required property string modelData
                                    width: parent.width
                                    height: propColumn.height + 20
                                    color: propertiesTab.index % 2 == 0 ? "#fafafa" : "white"
                                    border.color: "#eee"

                                    ColumnLayout {
                                        id: propColumn
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.margins: 10
                                        spacing: 5

                                        RowLayout {
                                            Text {
                                                text: propertiesTab.modelData.name
                                                font.bold: true
                                                font.family: "monospace"
                                                color: "#d73a49"
                                            }

                                            Text {
                                                text: `(${propertiesTab.modelData.type})`
                                                font.pointSize: 9
                                                color: "#6f42c1"
                                            }

                                            // Flags
                                            Row {
                                                spacing: 5

                                                Rectangle {
                                                    width: readableText.width + 8
                                                    height: readableText.height + 4
                                                    color: propertiesTab.modelData.readable ? "#28a745" : "#dc3545"
                                                    radius: 2
                                                    visible: propertiesTab.modelData.readable
                                                             || propertiesTab.writable

                                                    Text {
                                                        id: readableText
                                                        anchors.centerIn: parent
                                                        text: "R"
                                                        font.family: "Noto Sans Mono CJK HK"
                                                        color: "white"
                                                        font.pointSize: 8
                                                        font.bold: true
                                                    }
                                                }

                                                Rectangle {
                                                    width: writableText.width + 8
                                                    height: writableText.height + 4
                                                    color: propertiesTab.writable ? "#28a745" : "#dc3545"
                                                    radius: 2
                                                    visible: propertiesTab.modelData.readable
                                                             || propertiesTab.modelData.writable

                                                    Text {
                                                        id: writableText
                                                        anchors.centerIn: parent
                                                        text: "W"
                                                        color: "white"
                                                        font.pointSize: 8
                                                        font.bold: true
                                                    }
                                                }
                                            }
                                        }

                                        Text {
                                            text: propertiesTab.modelData.description
                                            font.pointSize: 9
                                            color: "#666"
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        RowLayout {
                                            visible: propertiesTab.modelData.defaultValue.length > 0

                                            Text {
                                                text: "Default:"
                                                font.pointSize: 8
                                                font.bold: true
                                            }

                                            Text {
                                                text: propertiesTab.modelData.defaultValue
                                                font.pointSize: 8
                                                font.family: "monospace"
                                                color: "#e83e8c"
                                            }
                                        }

                                        RowLayout {
                                            visible: propertiesTab.modelData.range.length > 0

                                            Text {
                                                text: "Range:"
                                                font.family: "Noto Sans Mono CJK KR"
                                                font.pointSize: 8
                                                font.bold: true
                                            }

                                            Text {
                                                text: propertiesTab.modelData.range
                                                font.pointSize: 8
                                                font.family: "monospace"
                                                color: "#fd7e14"
                                            }
                                        }

                                        Flow {
                                            Layout.fillWidth: true
                                            spacing: 5
                                            visible: propertiesTab.modelData.enumValues.length > 0

                                            Text {
                                                text: "Values:"
                                                font.pointSize: 8
                                                font.bold: true
                                            }

                                            Repeater {
                                                model: parent.visible ? propertiesTab.modelData.enumValues : []

                                                Rectangle {
                                                    width: enumText.width + 8
                                                    height: enumText.height + 4
                                                    color: "#e9ecef"
                                                    border.color: "#ced4da"
                                                    radius: 2

                                                    Text {
                                                        id: enumText
                                                        anchors.centerIn: parent
                                                        text: propertiesTab.modelData.modelData
                                                        font.pointSize: 8
                                                        font.family: "monospace"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                ScrollBar.vertical: ScrollBar {}
                            }
                        }

                        // Pad Templates tab
                        Item {
                            ListView {
                                id: padTemplates
                                anchors.fill: parent
                                model: elementBrowser.padModel
                                spacing: 10

                                delegate: Rectangle {
                                    id: padTemplate
                                    required property int index
                                    required property string direction
                                    required property string name
                                    required property string presence
                                    required property string caps
                                    width: padTemplates.width
                                    height: padColumn.height + 20
                                    color: padTemplate.direction === "SRC" ? "#e8f5e8" : "#e8f0ff"
                                    border.color: padTemplate.direction
                                                  === "SRC" ? "#28a745" : "#007bff"
                                    border.width: 2
                                    radius: 4

                                    ColumnLayout {
                                        id: padColumn
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.margins: 10
                                        spacing: 8

                                        RowLayout {
                                            Rectangle {
                                                width: directionText.width + 12
                                                height: directionText.height + 6
                                                color: padTemplate.direction
                                                       === "SRC" ? "#28a745" : "#007bff"
                                                radius: 3

                                                Text {
                                                    id: directionText
                                                    anchors.centerIn: parent
                                                    text: padTemplate.direction
                                                    color: "white"
                                                    font.bold: true
                                                    font.pointSize: 9
                                                }
                                            }

                                            Text {
                                                text: padTemplate.name
                                                font.bold: true
                                                font.family: "monospace"
                                                font.pointSize: 12
                                            }

                                            Text {
                                                text: `(${padTemplate.presence})`
                                                font.pointSize: 9
                                                color: "#666"
                                            }
                                        }

                                        ScrollView {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: Math.min(
                                                                        capsText.contentHeight
                                                                        + 10,
                                                                        200)
                                            contentWidth: availableWidth

                                            Rectangle {
                                                width: parent.width
                                                height: capsText.contentHeight + 10
                                                color: "#f8f9fa"
                                                border.color: "#dee2e6"
                                                radius: 2

                                                Text {
                                                    id: capsText
                                                    anchors.fill: parent
                                                    anchors.margins: 5
                                                    text: padTemplate.caps
                                                          || "No capabilities information"
                                                    font.family: "monospace"
                                                    font.pointSize: 8
                                                    wrapMode: Text.WordWrap
                                                    color: padTemplate.caps ? "#000" : "#999"
                                                }
                                            }
                                        }
                                    }
                                }

                                ScrollBar.vertical: ScrollBar {}
                            }
                        }
                    }
                }
            }
        }
    }
}
