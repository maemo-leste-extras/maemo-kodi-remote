import QtQuick 2.0
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import "lib"

Rectangle {
  id: root

  signal onExample(int number)

  property string backgroundColor: "red"
  property string borderColor: "#fdd663"
  property string areaColor: "#4d431d"

  property string panelBgColor: "#2c3436"
  property string panelTextColor: "#dbe3e6"

  visible: true
  color: backgroundColor

  FontAwesome {
    id: awesome
    resource: "qrc:///assets/fonts/fontawesome-webfont.ttf"
  }

  FontLoader {
      id: fontRobotoBold
      source: "qrc:/assets/fonts/Roboto-Bold.ttf"
  }

  FontLoader {
      id: fontRobotoRegular
      source: "qrc:/assets/fonts/Roboto-Regular.ttf"
  }

  ListModel {
    id: keyMdl
    ListElement {
        k: "arrow keys"
        v: "move"
        icon: ""
    }
    ListElement {
        k: "hjkl"
        v: "move"
        icon: ""
    }
    ListElement {
        k: "0"
        v: "mute/unmute"
        icon: ""
    }
    ListElement {
        k: "1-9"
        v: "volume levels"
        icon: ""
    }
    ListElement {
        k: "backspace"
        v: "return"
        icon: ""
    }
    ListElement {
        k: "enter"
        v: "select"
        icon: ""
    }
    ListElement {
        k: "space"
        v: "insert text"
        icon: ""
    }
    ListElement {
        k: "i"
        v: "info"
        icon: ""
    }
  }

  Component {
    id: keyDelegate
      RowLayout {
        id: del
        property int delHeight: 24
        Layout.preferredHeight: del.delHeight
        Layout.fillWidth: true
        spacing: 0

        Rectangle {
          color: "green"
          Layout.preferredHeight: del.delHeight
          Layout.preferredWidth: 20
          visible: icon != ""
        }

        RowLayout {
          spacing: 8
          Layout.preferredHeight: del.delHeight
          Layout.fillWidth: true
          
          Text {
            text: k
            font.bold: true
            font.family: fontRobotoBold.name
            font.pointSize: 16
            color: root.panelTextColor
          }
          
          Text {
            text: "-"
            font.family: fontRobotoRegular.name
            font.pointSize: 14
            color: root.panelTextColor
          }
          
          Text {
            text: v
            font.family: fontRobotoRegular.name
            font.pointSize: 14
            color: root.panelTextColor
          }
                  
          Item {
            Layout.fillWidth: true
            Layout.preferredHeight: del.delHeight
          } 
        }
      }
  }

  Rectangle {
    //bg
    anchors.fill: parent
    gradient: Gradient {
      GradientStop { position: 0.0; color: "#09435e" }
      GradientStop { position: 1.0; color: "#030e12" }
    }
  }

    RowLayout {
        anchors.margins: 32
        anchors.fill: parent
        spacing: 16
        
        Rectangle {
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 500
            Layout.maximumWidth: 640

            ColumnLayout {
              anchors.fill: parent
              spacing: 0

                ColumnLayout {
                  visible: mdl.statusInt != 3
                  Layout.fillWidth: true
                  Layout.minimumHeight: 372
                  Layout.minimumWidth: 500
                  Layout.maximumWidth: 640
                  spacing: 20

                  Item {
                    Layout.preferredWidth: 600
                    Layout.fillHeight: true
                  }

                  Item {
                    Layout.preferredWidth: 600
                    Layout.preferredHeight: 32

                    Text {
                      color: "white"
                      font.pointSize: 26
                      text: "Nothing is playing"
                      anchors.centerIn: parent
                    }
                  }

                  Item {
                    Layout.preferredWidth: 600
                    Layout.preferredHeight: 32

                    Text {
                      color: root.panelTextColor
                      font.pointSize: 18
                      text: {
                        console.log(mdl.statusInt);  // this is needed for QML to detect statusInt changes :/
                        switch(mdl.statusInt) {
                          case 0: {
                            return "No connection";
                          }
                          case 1: {
                            return "Authentication failed!";
                          }
                          case 2: {
                            return "Connected to Kodi";
                          }
                          default: {
                            return "?";
                            break;
                          }
                        }
                      }
                      anchors.centerIn: parent
                    }
                  }

                  Item {
                    Layout.preferredWidth: 600
                    Layout.fillHeight: true
                  }
                }


                Rectangle {
                    id: panel
                    visible: mdl.statusInt == 3
                    color: root.panelBgColor
                    Layout.fillWidth: true
                    Layout.minimumHeight: 372
                    Layout.minimumWidth: 500
                    Layout.maximumWidth: 640
                    radius: 12

                    ColumnLayout {
                        anchors.fill: parent
                        
                        Rectangle {
                            color: "transparent"
                            Layout.minimumHeight: 192
                            Layout.fillWidth: true
                            
                            RowLayout {
                                spacing: 0
                                anchors.fill: parent
                                Image {
                                    // image
                                    visible: np.thumbnail !== ""
                                    source: np.thumbnail
                                    Layout.preferredWidth: 148
                                    Layout.preferredHeight: 192
                                }
                                
                                Rectangle {
                                    // top content
                                    color: "transparent"
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    
                                    ColumnLayout {
                                        anchors.fill: parent
                                        spacing: 6
                                        
                                        Rectangle {
                                            color: "transparent"
                                            Layout.topMargin: 16
                                            Layout.preferredHeight: textTitle.implicitHeight
                                            Layout.fillWidth: true
                                            
                                            Text {
                                                id: textTitle
                                                text: np.displayName
                                                font.pointSize: 18
                                                font.bold: true
                                                font.family: fontRobotoBold.name
                                                color: "white"
                                                wrapMode: Text.WordWrap
                                                anchors.fill: parent
                                                anchors.leftMargin: 32
                                            }

                                        }
                                        
                                        Rectangle {
                                            color: "transparent"
                                            Layout.topMargin: 4
                                            Layout.preferredHeight: 24
                                            Layout.fillWidth: true
                                            
                                            Text {
                                                text: {
                                                    let ntype = np.npTypeInt;
                                                    if(ntype == 0) {
                                                        return "Movie";
                                                    } else if(np == 1) {
                                                        return "Episode";
                                                    } else if(np == 2){
                                                        return "Audio";
                                                    } else {
                                                        return "Unknown";
                                                    }
                                                }
                                                font.pointSize: 16
                                                font.family: fontRobotoRegular.name
                                                color: root.panelTextColor
                                                anchors.fill: parent
                                                anchors.leftMargin: 32
                                            }
                                        }
                                        
                                        Rectangle {
                                            // play buttons etc.
                                            color: "transparent"
                                            Layout.fillHeight: true
                                            Layout.fillWidth: true

                                            RowLayout {
                                              anchors.fill: parent
                                              anchors.margins: 8
                                              spacing: 6

                                              Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                              }
                                              
                                              Rectangle {
                                                Layout.minimumWidth: 84
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: parent.height
                                                color: "transparent"
                                                visible: false

                                                Text {
                                                  text: awesome.icons.fa_backward
                                                  font.family: awesome.family
                                                  renderType: Text.NativeRendering
                                                  font.pointSize: 18
                                                  color: "white"
                                                  anchors.centerIn: parent
                                                }
                                              }
                                              
                                              Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                              }

                                              Rectangle {
                                                Layout.minimumWidth: 84
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: parent.height
                                                color: "transparent"
                                                
                                                Text {
                                                  text: {
                                                    console.log(player.paused);
                                                    if(player.paused) {
                                                      return awesome.icons.fa_play;
                                                    } else {
                                                      return awesome.icons.fa_pause;
                                                    }
                                                  }
                                                  font.family: awesome.family
                                                  renderType: Text.NativeRendering
                                                  font.pointSize: 18
                                                  color: "white"
                                                  anchors.centerIn: parent
                                                }

                                                MouseArea {
                                                  anchors.fill: parent
                                                  onClicked: {
                                                    mdl.playPause();
                                                  }
                                                }
                                              }
                                              
                                              Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                              }

                                              Rectangle {
                                                Layout.minimumWidth: 84
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: parent.height
                                                color: "transparent"
                                                
                                                Text {
                                                  text: awesome.icons.fa_stop
                                                  font.family: awesome.family
                                                  renderType: Text.NativeRendering
                                                  font.pointSize: 18
                                                  color: "white"
                                                  anchors.centerIn: parent
                                                }

                                                MouseArea {
                                                  anchors.fill: parent
                                                  onClicked: {
                                                    mdl.playerStop();
                                                  }
                                                }
                                              }

                                              Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                              }
                                              
                                              Rectangle {
                                                Layout.minimumWidth: 84
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: parent.height
                                                color: "transparent"
                                                visible: false
                                                
                                                Text {
                                                  text: awesome.icons.fa_forward
                                                  font.family: awesome.family
                                                  renderType: Text.NativeRendering
                                                  font.pointSize: 18
                                                  color: "white"
                                                  anchors.centerIn: parent
                                                }
                                              }
                                              
                                              Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                              }
                                            }

                                        }
                                    }
                                }
                            }
                        }
                        
                        // seek bar
                        Item {
                          Layout.fillWidth: true
                          Layout.preferredHeight: 74
                          
                          RowLayout {
                            anchors.fill: parent
                            spacing: 16
                            
                            // seeker: current pos
                            Item {
                              Layout.preferredWidth: 118
                              Layout.fillHeight: true
                              
                              Text {
                                color: root.panelTextColor
                                font.pointSize: 14
                                text: player.time_str
                                anchors.centerIn: parent
                              }
                            }
                            
                            Rectangle {
                              color: "transparent"
                              Layout.fillWidth: true
                              Layout.fillHeight: true

                              ProgressBar {
                                id: controlSeek
                                anchors.verticalCenter: parent.verticalCenter
                                width: parent.width
                                value: player.percentage / 100

                                background: Rectangle {
                                    implicitWidth: 200
                                    implicitHeight: 6
                                    color: root.panelTextColor
                                    radius: 3
                                }

                                contentItem: Item {
                                    implicitWidth: 200
                                    implicitHeight: 4

                                    Rectangle {
                                        width: controlSeek.visualPosition * parent.width
                                        height: parent.height
                                        radius: 2
                                        color: "#058ac8"
                                    }
                                }
                              }
                            }
                            
                            // seeker: total
                            Item {
                              Layout.preferredWidth: 118
                              Layout.fillHeight: true
                              
                              Text {
                                color: root.panelTextColor
                                font.pointSize: 14
                                text: np.duration_str
                                anchors.centerIn: parent
                              }
                            }
                          }
                        }

                        // lower btns
                        Item {
                          Layout.fillWidth: true
                          Layout.preferredHeight: 80
                          
                          RowLayout {
                            id: lowerButtonLayout
                            anchors.fill: parent
                            property int lowerBtnHeight: 80
                            spacing: 20
                            
                            Item {
                              Layout.preferredWidth: lowerButtonLayout.lowerBtnHeight
                              Layout.preferredHeight: lowerButtonLayout.lowerBtnHeight
                              
                              Text {
                                text: {
                                  if(mdl.volume_muted) {
                                    return awesome.icons.fa_volume_off;
                                  } else {
                                    return awesome.icons.fa_volume_up;
                                  }
                                }
                                font.family: awesome.family
                                font.pointSize: 20
                                renderType: Text.NativeRendering
                                color: root.panelTextColor
                                anchors.centerIn: parent
                              }
                            }

                            Item {
                              Layout.preferredWidth: 192
                              Layout.preferredHeight: lowerButtonLayout.lowerBtnHeight

                              ProgressBar {
                                id: controlVolume
                                anchors.verticalCenter: parent.verticalCenter
                                width: parent.width
                                value: {
                                  if(mdl.volume_muted) {
                                    return 0.0;
                                  } else {
                                    return mdl.volume / 100;
                                  }
                                }

                                background: Rectangle {
                                    implicitWidth: 200
                                    implicitHeight: 6
                                    color: root.panelTextColor
                                    radius: 3
                                }

                                contentItem: Item {
                                    implicitWidth: 200
                                    implicitHeight: 4

                                    Rectangle {
                                        width: controlVolume.visualPosition * parent.width
                                        height: parent.height
                                        radius: 2
                                        color: "#058ac8"
                                    }
                                }
                              }
                            }

                            Item {
                              Layout.fillWidth: true
                              Layout.fillHeight: true
                            }

                            Item {
                              Layout.preferredWidth: 100
                              Layout.preferredHeight: lowerButtonLayout.lowerBtnHeight
                              
                              Text {
                                text: awesome.icons.fa_file_audio_o 
                                font.family: awesome.family
                                font.pointSize: 20
                                renderType: Text.NativeRendering
                                color: root.panelTextColor
                                anchors.centerIn: parent
                              }

                              MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                  audioStreamsDialog.title = "Audio stream";
                                  audioStreamsDialog.setItems(player.audioStreams);
                                  audioStreamsDialog.show();
                                }
                              }
                            }

                            Item {
                              Layout.preferredWidth: 100
                              Layout.preferredHeight: lowerButtonLayout.lowerBtnHeight  
                              
                              Text {
                                text: awesome.icons.fa_comment
                                font.family: awesome.family
                                font.pointSize: 20
                                renderType: Text.NativeRendering
                                color: root.panelTextColor
                                anchors.centerIn: parent
                              }

                              MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                  subtitlesDialog.title = "Subtitles";
                                  subtitlesDialog.setItems(player.subtitles);
                                  subtitlesDialog.show();
                                }
                              }
                            }
                          }
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                }
                
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }
        }
        
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Rectangle {
            // FAQ
            color: "transparent"
            Layout.fillHeight: true
            Layout.minimumWidth: 200

            ColumnLayout {
              anchors.fill: parent
              spacing: 0

              Repeater {
                  model: keyMdl
                  delegate: keyDelegate
              }
                
              Rectangle {
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
              }
            }

        }
    }

  MouseArea {
    //anchors.fill: parent
    onClicked: {
      inputDialog.title = "Keyboard";
      inputDialog.description = "Send some text to Kodi";
      inputDialog.show();
    }
  }

  DialogList {
      id: subtitlesDialog
      anchors.fill: parent
      z: 100000
  }

  DialogList {
      id: audioStreamsDialog
      anchors.fill: parent
      z: 100000
  }

  DialogInput {
      id: inputDialog
      anchors.fill: parent
      z: 100000
  }

  DialogMessage {
    id: messageDialog
    anchors.fill: parent
    z: 100000
    btnSubmit: "OK"
    text: ""

    Connections {
      target: messageDialog
      function onSubmit() {
        console.log("cool");
      }
    }

    Connections {
      target: audioStreamsDialog
      function onSubmit(idx) {
        mdl.setAudioStream(idx);
      }
    }

    Connections {
      target: subtitlesDialog
      function onSubmit(idx) {
        mdl.setSubtitle(idx);
      }
    }

    Connections {
      target: inputDialog
      function onSubmit(msg) {
        window.inputBoxActive = false;
        mdl.sendText(msg);
      }

      function onCancel() {
        window.inputBoxActive = false;
      }
    }

    Connections {
      target: window
      function onEnterPressed() {
        inputDialog.doSubmit();
      }
    }

    Connections {
      target: mdl
      function onShowKeyboard() {
        if(mdl.statusInt >=2) {
          inputDialog.title = "Keyboard";
          inputDialog.description = "Send some text to Kodi";
          inputDialog.show();
          window.inputBoxActive = true;
        }
      }
    }
  }
}
