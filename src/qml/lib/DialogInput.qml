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
    property string btnCancel: "Cancel"
    property string btnSubmit: "Submit"
    property string borderColor: "#0e85a6"

    property bool _active: dialogContainer.state === "on"
    property int _opacityTransitionDuration: 400

    signal submit(string msg);
    signal cancel();

    function show() {
        console.log(`DialogInput(${root.title})`);
        root.visible = true;
        dialogContainer.state = "on";
        bgStaticHidden.state = "on";
        input.text = "";
        input.forceActiveFocus();
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

    function doSubmit() {
      root.submit(input.text);
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
            root.cancel();
        }
    }

    Item {
        anchors.fill: parent

        Rectangle {
            z: 100000
            id: dialogContainer
            anchors.centerIn: parent
            height: dialogLayout.implicitHeight + 32

            width: {
                let _w = root.width - 200;
                if(_w >= 420) return 420;
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

                Item {
                    Layout.fillWidth: true
                    Layout.leftMargin: 24
                    Layout.rightMargin: 24
                    Layout.preferredHeight: descriptionText.implicitHeight

                    Text {
                        id: descriptionText
                        text: root.description
                        font.pointSize: 14
                        font.family: fontRobotoRegular.name
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                        wrapMode: Text.WordWrap
                    }

                    MouseArea { anchors.fill: parent }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 68

                    Rectangle {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        anchors.topMargin: 12
                        anchors.bottomMargin: 12
                        border.color: "#c7c7c7"
                        border.width: 1
                        radius: 10

                        color: "white"

                        TextInput {
                            id: input
                            text: ""
                            font.pointSize: 18
                            font.family: fontRobotoRegular.name
                            cursorVisible: false
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: 16
                            cursorDelegate: Rectangle {
                                id: cursor
                                visible: true
                                color: "#426bf2"
                                width: 2

                                Timer {
                                    interval: 500
                                    running: root._active
                                    repeat: true
                                    onTriggered: {
                                        cursor.visible = !cursor.visible;
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

                    Rectangle {  // border
                        Layout.preferredHeight: btnRow._height
                        Layout.preferredWidth: 1
                        color: root.borderColor
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: btnRow._height

                        Text {
                            text: root.btnSubmit
                            font.pointSize: 16
                            font.family: fontRobotoRegular.name
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: "white"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    console.log(`DialogInput(${root.title}).submit()`);
                                    root.doSubmit();
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

