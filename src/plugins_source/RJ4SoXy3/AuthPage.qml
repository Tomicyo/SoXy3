import QtQuick 1.1

Rectangle {
    id:prnt
    width: 552
    height: 281
    color: "transparent"
    radius: 5

    property string xnetcard: "none"

    signal echo(string msg);
    signal endcert;
    signal begincert(string user, string pass, string netcard, string resource)
    signal rememsetting;

    function toggle() {
        if(mousearea1.mouseX > 0 && mousearea1.mouseX < 60 && (toggleswitch.state != "offline")){
            toggleswitch.state = "offline"
            echo("离线")
            endcert()
        }
        if(mousearea1.mouseX > 60 && mousearea1.mouseX < 119 && (toggleswitch.state != "free")){
            toggleswitch.state = "free"
            echo(qsTr("免费资源"))
            begincert(id_input.text, pass_input.text, xnetcard, "Free")
        }

        if(mousearea1.mouseX > 119 && (toggleswitch.state != "charged") ){
            toggleswitch.state = "charged"
            echo(qsTr("已切换到收费资源"))
            begincert(id_input.text, pass_input.text, xnetcard, "Charged")
        }
    }

    Item {
        x: 115
        y: 256
        id: toggleswitch
        width: background.width; height: background.height
        state: "offline"
        signal qmlSignal(string msg)

        Image {
            id: background
            source: "img/staterback.png"
            MouseArea {
                id: mousearea1;
                anchors.fill: parent;
                acceptedButtons: Qt.LeftButton;
                onClicked: toggle()
            }
        }

        Image {
            id: knob
            x: 0
            y: 2
            anchors.verticalCenter: parent.verticalCenter
            source: "img/staterhandler.png"
            MouseArea{
                id: mouseAreaKonb;
                anchors.fill: parent;
            }
        }

        states: [
            State {
                name: "offline"
                PropertyChanges {
                          target: knob
                          x: 0
                          y: 0
                          anchors.verticalCenterOffset: 0
                }

            },

            State {
                name: "free"

                PropertyChanges {
                    target: knob
                    x: 60
                    y: 0
                    anchors.verticalCenterOffset: 0
                }
            },
            State {
                name: "charged"

                PropertyChanges {
                    target: knob
                    x: 119
                    y: 0
                    anchors.verticalCenterOffset: 0
                }
            }
        ]
    //![5]
        transitions: Transition {
            NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad; duration: 200 }
        }
    //![7]
    }

    TextInput {
        id: id_input
        objectName: "ID_Input"
        x: 115
        y: 127
        width: back.width
        height: back.height
        color: "#ffffff"
        text: qsTr("输入你的学号！！")
        inputMask: ""
        font.family: "幼圆"
        echoMode: TextInput.Password
        opacity: 1
        horizontalAlignment: TextInput.AlignHCenter
        font.pixelSize: 24
        focus: true
        Image{
            id: back
            z: -1
            source: "img/input_field.png"
        }
    }

    TextInput {
        id: pass_input
        objectName: "PassWord"
        x: 115
        y: 172
        width: back2.width
        height: back2.height
        color: "#ffffff"
        text: qsTr("text")
        echoMode: TextInput.Password
        horizontalAlignment: TextInput.AlignHCenter
        font.pixelSize: 30
        focus: true
        Image{
            id: back2
            z: -1
            source: "img/input_field.png"
        }
    }

    Image {
        id: idinputimage
        x: 321
        y: 124
        width: source.width
        height: source.height
        source: "img/LabelId.png"
    }

    Image {
        id: passwdimage
        x: 322
        y: 170
        width: source.width
        height: source.height
        source: "img/LabelPass.png"
    }

    function rem_check(){
        if( remempass.state == "checked"){
            remempass.state = "unchecked"
        }
        else{
            remempass.state = "checked"
            rememsetting()
        }
    }

    Image {
        objectName: "remempass"
        id: remempass
        x: 282
        y: 213
        width: source.width
        height: source.height
        source: "img/remem_pass_bg.png"

        state: "unchecked"

        Image{
            id:remem_marker
            source: "img/remem_pass_marker.png"
        }


        MouseArea{
            id: click
            x: 0
            y: 0
            width: remempass.width
            height: remempass.height
            onClicked: rem_check()
        }

        states: [
            State {
                name: "checked"
                PropertyChanges {
                    target: remem_marker
                    visible: true
                }

            },

            State {
                name: "unchecked"

                PropertyChanges {
                    target: remem_marker
                    visible: false
                }
            }
        ]
        transitions: [
           Transition {
               PropertyAnimation { target: remem_marker
                                   properties: "visible"; duration: 50 }
           }
        ]
    }

    function runasserv_check(){
        if( run_as_serv.state == "checked")
            run_as_serv.state = "unchecked"
        else
            run_as_serv.state = "checked"
    }

    Image {
        id: run_as_serv
        x: 299
        y: 256
        width: source.width
        height: source.height
        source: "img/run_back.png"

        state: "checked"

        Image{
            id:run_marker
            source: "img/run_check.png"
        }


        MouseArea{
            id: checkrun
            x: 0
            y: 0
            width: run_as_serv.width
            height: run_as_serv.height
            onClicked: runasserv_check()
        }

        states: [
            State {
                name: "checked"
                PropertyChanges {
                    target: run_marker
                    visible: true
                }

            },

            State {
                name: "unchecked"

                PropertyChanges {
                    target: run_marker
                    visible: false
                }
            }
        ]
        transitions: [
           Transition {
               PropertyAnimation { target: run_marker
                                   properties: "visible"; duration: 50 }
           }
        ]
    }

    Image {
        id: logo
        source: "img/NPULOGO.png"
        x: 115
        y: 13
        width: 343
        height: 107
    }

}
