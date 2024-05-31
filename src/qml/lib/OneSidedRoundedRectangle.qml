import QtQuick 2.0
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Item {
    id: root
    property int contentHeight: 0
    property int contentWidth: 0
    property string contentColor: "green"
    property double contentOpacity: 1.0

    Rectangle {
        id: mask

        color: "transparent"
        height: output.height
        visible: false
        width: output.width

        Rectangle {
            id: corner_mask

            property real size: parent.width * 0.1

            color: "red"
            height: size * 2
            radius: size * 2
            visible: false
            width: size * 2
        }
        Rectangle {
            id: corner_rect

            height: corner_mask.height
            visible: false
            width: corner_mask.width
        }
        ShaderEffectSource {
            id: shader_effect_mask

            sourceItem: corner_mask
            sourceRect: Qt.rect(0, 0, corner_mask.width / 2, corner_mask.height / 2)
        }
        OpacityMask {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            height: corner_mask.height / 4
            invert: true
            maskSource: shader_effect_mask
            rotation: 270
            source: corner_rect
            width: corner_mask.width / 4
        }
        OpacityMask {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            height: corner_mask.height / 4
            invert: true
            maskSource: shader_effect_mask
            rotation: 180
            source: corner_rect
            width: corner_mask.width / 4
        }
    }

    // the maskee
    Rectangle {
        id: output

        color: root.contentColor
        height: root.contentHeight
        visible: false
        width: root.contentWidth
    }

    OpacityMask {
        height: output.height
        invert: true
        maskSource: mask
        source: output
        width: output.width
    }
}