import QtQuick 2.0

Item {
    id: awesome

    property alias icons: variables
    property alias loaded: loader.loaded
    property alias resource: loader.resource

    readonly property string family: "FontAwesome"

    FontAwesomeLoader {
        id: loader
    }

    FontAwesomeVariables {
        id: variables
    }
}
