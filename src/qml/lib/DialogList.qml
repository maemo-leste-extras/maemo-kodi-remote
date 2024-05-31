import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Item {
    id: root
    visible: false

    property string title: "New modal"
    property string description: "description"
    property var items: []
    property string btnCancel: "Cancel"
    property string btnSubmit: "Submit"
    property string borderColor: "#0e85a6"

    property bool _active: dialogContainer.state === "on"
    property int _opacityTransitionDuration: 400

    signal submit(int idx);
    signal cancel();

    ListModel {
      id: cool
      ListElement {
          name: "arrow keys"
      }
      ListElement {
          name: "arrow keys"
      }
      ListElement {
          name: "arrow keys"
      }
    }

    function setItems(items) {
      cool.clear();
      for (let xx in items) {
        cool.append({name: items[xx]});
      }
    }

    function show() {
        console.log(`DialogInput(${root.title})`);
        root.visible = true;
        dialogContainer.state = "on";
        bgStaticHidden.state = "on";
    }

    function hide() {
        console.log(`DialogInput(${root.title})`);
        dialogContainer.state = "off";
        bgStaticHidden.state = "off";

        ctx.singleShot(root._opacityTransitionDuration, function () {
            if(dialogContainer.state === "off")
                root.visible = false;
        });
    }

    FontLoader {
        id: fontRobotoBold
        source: "qrc:/assets/fonts/Roboto-Bold.ttf"
    }

    FontLoader {
        id: fontRobotoRegular
        source: "qrc:/assets/fonts/Roboto-Regular.ttf"
    }

    Rectangle {
        id: bgStaticHidden
        anchors.fill: parent
        color: "black"
        opacity: 0.0
        state: "off"

        states: [
            State {
                name: "on"

                PropertyChanges {
                    opacity: 0.6
                    target: bgStaticHidden
                }
            },
            State {
                name: "off"

                PropertyChanges {
                    opacity: 0.0
                    target: bgStaticHidden
                }
            }
        ]
        transitions: Transition {
            NumberAnimation {
                duration: root._opacityTransitionDuration
                easing.type: Easing.OutCubic
                properties: "opacity"
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: root._active
        onClicked: {
          console.log("xxcxx")
            root.cancel();
        }
    }

    Item {
        anchors.fill: parent

        Rectangle {
            z: 100000
            id: dialogContainer
            anchors.centerIn: parent
            height: (dialogLayout.implicitHeight) + 32

            width: {
                let _w = root.width - 200;
                if(_w >= 480) return 480;
                return _w;
            }

            gradient: Gradient {
                GradientStop { position: 1.0; color: "#082b37" }
                GradientStop { position: 0.0; color: "#09435e" }
            }

            radius: 16
            border.color: root.borderColor
            border.width: 3

            state: "off"
            states: [
                State {
                    name: "off"
                    PropertyChanges {
                        anchors.verticalCenterOffset: -60
                        target: dialogContainer
                    }
                    PropertyChanges {
                        opacity: 0.0
                        target: dialogContainer
                    }
                },
                State {
                    name: "on"

                    PropertyChanges {
                        anchors.verticalCenterOffset: 0
                        target: dialogContainer
                    }
                    PropertyChanges {
                        opacity: 1.0
                        target: dialogContainer
                    }
                }
            ]
            transitions: Transition {
                NumberAnimation {
                    duration: 400
                    easing.type: Easing.OutCubic
                    properties: "opacity"
                }
                NumberAnimation {
                    duration: 400
                    easing.type: Easing.OutCubic
                    properties: "anchors.verticalCenterOffset"
                }
            }

            ColumnLayout {
                id: dialogLayout
                anchors.topMargin: 18
                anchors.fill: parent
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 26

                    Text {
                        text: root.title
                        font.bold: true
                        font.pointSize: 16
                        font.family: fontRobotoBold.name
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }

                    MouseArea { anchors.fill: parent }
                }

                Rectangle {
                    color: "transparent"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 82 * cool.count
                    visible: true

                    ColumnLayout {
                      anchors.fill: parent
                      spacing: 0

                      Repeater {
                          model: cool
                          delegate: Rectangle {
                            color: "transparent"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80

                            Text {
                              text: name
                              color: "white"
                              font.pointSize: 16
                              font.family: fontRobotoBold.name
                              anchors.fill: parent
                              anchors.leftMargin: 16
                              verticalAlignment: Text.AlignVCenter
                              horizontalAlignment: Text.AlignHCenter

                              MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                  root.submit(index);
                                }
                              }
                            }
                          }
                      }
                    }
                }

                Rectangle {  // border
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: root.borderColor

                    MouseArea { anchors.fill: parent }
                }

                RowLayout {
                    id: btnRow
                    Layout.fillWidth: true
                    Layout.preferredHeight: btnRow._height
                    property int _height: 56
                    spacing: 0

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: btnRow._height

                        Text {
                            text: root.btnCancel
                            font.pointSize: 16
                            font.family: fontRobotoRegular.name
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: "white"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    console.log(`DialogInput(${root.title}).cancel()`);
                                    root.cancel();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        function onCancel() {
            root.hide();
        }

        function onSubmit() {
            root.hide();
        }

        target: root
    }
}

