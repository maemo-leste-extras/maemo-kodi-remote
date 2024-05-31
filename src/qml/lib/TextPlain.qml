import QtQuick 2.0
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import "."


Text {
    id: root
    font.family: Style.fontMedium.name
    font.bold: false
    font.pixelSize: 14
    textFormat: Text.PlainText

    Rectangle {
        width: root.contentWidth
        height: root.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: root.focus ? Style.titleBarButtonHoverColor : "transparent"
    }
}
