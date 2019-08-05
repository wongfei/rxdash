import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Extras 1.4
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import rxdash 1.0

Window {
    id: wnd
    title: qsTr("rxdash")
    width: 720
    height: 480
    visible: true
    color: "#000000"

    property real rpmLimit: 7500
    property real rpmRedZone: 7000
    property real rpmStep: 1000
    property int rpmFontSize: 25
    property int rpmHeight: 40

    property int valWidthL: 100
    property int valWidthR: 80
    property int valFontSize: 28
    property int dbgFontSize: 25
    property color valOK: "#08b308"
    property color valERR: "#ef0f0f"

    DashDriver {
        id: dash
    }

    FontLoader {
        id: gaugeFont
        source: "qrc:/whitrabt.ttf"
    }

    Rectangle {
        id: screen
        color: "#00000000"
        visible: true
        anchors.rightMargin: 30
        anchors.leftMargin: 30
        anchors.bottomMargin: 30
        anchors.topMargin: 30
        anchors.fill: parent
        enabled: false
        smooth: false
        border.width: 0
        border.color: "#2c8a07"

        Timer {
            id: updateTimer
            interval: dash.getRenderRate()
            repeat: true
            running: true
            triggeredOnStart: true
            onTriggered: function () {

                var connected = dash.isConnected()
                var RPM = dash.getVar(Pfc.Adv_RPM)
                //var RPM = randomRange(0, rpmLimit)

                // http://www.racingbeat.com/mazda/performance/rotary-tech-tips/oil-tips.html
                // oil idle 20..40 psi
                // oil >3k 64-78 psi
                // max 205°F
                var maxIAT = 70
                var maxOT = 95
                var maxWT = 95
                var minOP = 35
                var maxOP = 100
                var minFP = 35
                var maxFP = 45

                // Aux inputs
                // 1 fuel pressure
                // 2 oil pressure
                // 3 oil temp
                // 4 O2 yellow 0V=7.35 AFR and 5V=22.39 AFR
                // 5 O2 brown 1.1V=14 AFR and 0.1V=15 AFR
                // 6 +5v
                // 7 ground
                var auxFP = dash.getVar(Pfc.Aux1)
                var auxOP = dash.getVar(Pfc.Aux2)
                var auxOT = dash.getVar(Pfc.Aux3)
                var auxAFW = dash.getVar(Pfc.Aux4)

                var FP = connected ? (auxFP * 25 - 12.5) : 0 // sensor 0-100 psi ; y = 25*x - 12.5
                var OP = connected ? (auxOP * 50 - 25) : 0 // sensor 0-200 psi ; y = 50*x - 25
                var OT = connected ? (auxOT * auxOT * 0.7234098 - auxOT * 15.86543 + 108.8124) : 0
                var AFW = connected ? (auxAFW * 3.008 + 7.35) : 0 // 0V=7.35 AFR and 5V=22.39 AFR

                var WT = dash.getVar(Pfc.Adv_WtrT)
                var IAT = dash.getVar(Pfc.Adv_AirT)

                error.active = !dash.isConnected() || (IAT > maxIAT)
                        || (OT > maxOT) || (WT > maxWT) || (OP < minOP)
                        || (OP > maxOP) || (FP < minFP) || (FP > maxFP)

                gaugeRPM.value = RPM
                valRPM.text = RPM.toFixed().toString()
                valSpeed.text = dash.getVar(Pfc.Adv_Speed).toFixed().toString()

                // left
                setGauge(valBAT, 12.4, 14.7, dash.getVar(Pfc.Adv_BatV))
                setGauge(valIAT, 0, maxIAT, IAT)
                setGauge(valOT, 0, maxOT, OT)
                setGauge(valWT, 0, maxWT, WT)
                setGauge(valOP, minOP, maxOP, OP)
                setGauge(valFP, minFP, maxFP, FP)

                // right
                setGauge(valKN, 0, 60, dash.getVar(Pfc.Adv_Knock))
                setGauge(valAFW, 10, 15, AFW)
                setGauge(valTPSV, 0, 5, dash.getVar(Pfc.Adv_TPSV)) // 0..5V
                setGauge(valPIMV, 0, 5, dash.getVar(Pfc.Adv_PIMV)) // 0..5V

                // middle
                valBusRate.text = dash.getBusUpdateRate().toString()
                valBusErrors.text = dash.getBusErrorCount().toString()

                // bottom
                valSW.text = "SW " + dash.getSwitchesStr()
                valGV.text =
                    "G " + valueToString(dash.getVar(Pfc.Aux7)) +
                    " V " + valueToString(dash.getVar(Pfc.Aux6))
            }

            function setGauge(gauge, min, max, value) {
                gauge.text = valueToString(value)
                if (value > min && value < max)
                    gauge.color = valOK
                else
                    gauge.color = valERR
            }

            function valueToString(val) {
                return val.toFixed(1).toString()
            }

            function randomRange(min, max) {
                return (min + Math.random() * (max - min))
            }
        }

        Rectangle {
            id: rectRPM
            color: "#00000000"
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.bottom: parent.top
            anchors.bottomMargin: -100
            anchors.top: parent.top
            anchors.topMargin: 10
            border.width: 0
            smooth: true
            border.color: "#9b23d7"

            Gauge {
                id: gaugeRPM
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 5
                smooth: true
                enabled: false
                orientation: Qt.Horizontal
                minimumValue: 0
                maximumValue: 8000
                value: 7500
                tickmarkStepSize: rpmStep
                minorTickmarkCount: 1
                font.family: gaugeFont.name
                font.pixelSize: rpmFontSize

                style: GaugeStyle {
                    foreground: null
                    valueBar: Rectangle {
                        implicitWidth: rpmHeight
                        //color: Qt.rgba((valRPM.value / rpmRedZone), (1 - (valRPM.value / rpmRedZone)), 0, 1)
                        gradient: Gradient {
                            GradientStop {
                                position: 1.0
                                color: "green"
                            }
                            GradientStop {
                                position: 0.0
                                color: Qt.rgba(
                                           (gaugeRPM.value / rpmRedZone),
                                           (1 - (gaugeRPM.value / rpmRedZone)),
                                           0, 1)
                            }
                        }
                    }
                    tickmark: Item {
                        implicitWidth: 8
                        implicitHeight: 4
                        Rectangle {
                            x: control.tickmarkAlignment === Qt.AlignLeft
                               || control.tickmarkAlignment
                               === Qt.AlignTop ? parent.implicitWidth : -(rpmHeight - 1)
                            width: (rpmHeight - 1)
                            height: parent.height
                            color: "#ffffff"
                        }
                    }
                    minorTickmark: Item {
                        implicitWidth: 8
                        implicitHeight: 2
                        Rectangle {
                            x: control.tickmarkAlignment === Qt.AlignLeft
                               || control.tickmarkAlignment
                               === Qt.AlignTop ? parent.implicitWidth : -(rpmHeight / 2)
                            width: (rpmHeight / 2)
                            height: parent.height
                            color: "#ffffff"
                        }
                    }
                }

                formatValue: function (value) {
                    return (value * 0.001)
                }

                Behavior on value {
                    NumberAnimation {
                        duration: 500
                    }
                }
            }
        }

        Text {
            id: valRPM
            x: 273
            width: 150
            color: "#dab80e"
            text: qsTr("7999")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: rectRPM.bottom
            anchors.topMargin: 10
            horizontalAlignment: Text.AlignHCenter
            smooth: true
            textFormat: Text.PlainText
            style: Text.Normal
            font.family: gaugeFont.name
            antialiasing: false
            styleColor: "#40f80c"
            enabled: false
            transformOrigin: Item.Center
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 60
        }

        Text {
            id: valSpeed
            width: 110
            color: "#119cd3"
            text: qsTr("200")
            anchors.topMargin: 20
            font.family: gaugeFont.name
            style: Text.Normal
            anchors.top: valRPM.bottom
            anchors.horizontalCenter: rectRPM.horizontalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            transformOrigin: Item.Center
            textFormat: Text.PlainText
            enabled: false
            smooth: true
            antialiasing: false
            font.pixelSize: 60
            styleColor: "#40f80c"
        }

        Text {
            id: txKMH
            y: 190
            color: "#119cd3"
            text: qsTr("kmh")
            anchors.verticalCenter: valSpeed.verticalCenter
            anchors.left: valSpeed.right
            anchors.leftMargin: 10
            horizontalAlignment: Text.AlignHCenter
            smooth: true
            textFormat: Text.PlainText
            style: Text.Normal
            font.family: gaugeFont.name
            antialiasing: false
            styleColor: "#40f80c"
            enabled: false
            transformOrigin: Item.Center
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 30
        }

        StatusIndicator {
            id: error
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.horizontalCenter: parent.horizontalCenter
            smooth: true
            enabled: false
            active: true
        }

        Grid {
            id: gridL
            transformOrigin: Item.Center
            enabled: false
            smooth: true
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40
            anchors.left: parent.left
            anchors.leftMargin: 20
            spacing: 14
            rows: 6
            columns: 2

            //horizontalItemAlignment: Grid.AlignHCenter
            //horizontalItemAlignment: Grid.AlignHRight
            Text {
                id: txBAT
                color: "#9b23d7"
                text: qsTr("BAT")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                enabled: false
                smooth: true
                antialiasing: false
                font.family: gaugeFont.name
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
            }

            Text {
                id: valBAT
                width: valWidthL
                color: "#ef0f0f"
                text: qsTr("14.5")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText

                enabled: false
                smooth: true
                antialiasing: false

                font.pixelSize: valFontSize
                font.family: gaugeFont.name
            }

            Text {
                id: txIAT
                color: "#9b23d7"
                text: qsTr("IAT")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                smooth: true
                enabled: false
                antialiasing: false
                horizontalAlignment: Text.AlignHCenter
                textFormat: Text.PlainText
            }

            Text {
                id: valIAT
                color: "#08b308"
                text: qsTr("28.0")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                smooth: true
                enabled: false
                antialiasing: false
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText
                width: valWidthL
            }

            Text {
                id: txOT
                color: "#9b23d7"
                text: qsTr("OT")
                horizontalAlignment: Text.AlignHCenter
                enabled: false
                smooth: true
                antialiasing: false
                textFormat: Text.PlainText
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
            }

            Text {
                id: valOT
                color: "#08b308"
                text: qsTr("100.0")
                enabled: false
                smooth: true
                antialiasing: false
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
                verticalAlignment: Text.AlignVCenter
                font.family: gaugeFont.name
                width: valWidthL
            }

            Text {
                id: txWT
                color: "#9b23d7"
                text: qsTr("WT")
                verticalAlignment: Text.AlignVCenter
                enabled: false
                smooth: true
                antialiasing: false
                textFormat: Text.PlainText
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
            }

            Text {
                id: valWT
                color: "#08b308"
                text: qsTr("99.0")
                verticalAlignment: Text.AlignVCenter
                enabled: false
                smooth: true
                antialiasing: false
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                width: valWidthL
            }

            Text {
                id: txOP
                color: "#9b23d7"
                text: qsTr("OP")
                horizontalAlignment: Text.AlignHCenter
                enabled: false
                smooth: true
                antialiasing: false
                verticalAlignment: Text.AlignVCenter
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
            }

            Text {
                id: valOP
                color: "#08b308"
                text: qsTr("60.0")
                enabled: false
                smooth: true
                antialiasing: false
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
                verticalAlignment: Text.AlignVCenter
                font.family: gaugeFont.name
                width: valWidthL
            }

            Text {
                id: txFP
                color: "#9b23d7"
                text: qsTr("FP")
                horizontalAlignment: Text.AlignHCenter
                enabled: false
                smooth: true
                antialiasing: false
                verticalAlignment: Text.AlignVCenter
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
            }

            Text {
                id: valFP
                color: "#08b308"
                text: qsTr("40.0")
                enabled: false
                smooth: true
                antialiasing: false
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText
                font.pixelSize: valFontSize
                verticalAlignment: Text.AlignVCenter
                font.family: gaugeFont.name
                width: valWidthL
            }
        }

        Grid {
            id: gridR
            spacing: 14
            rows: 4
            columns: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 40
            anchors.right: parent.right
            anchors.rightMargin: 20

            //horizontalItemAlignment: Grid.AlignHCenter
            Text {
                id: txKN
                color: "#9b23d7"
                text: qsTr("KN")
                horizontalAlignment: Text.AlignLeft
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
            }

            Text {
                id: valKN
                color: "#08b308"
                text: qsTr("0.0")
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                horizontalAlignment: Text.AlignRight
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
                width: valWidthR
            }

            Text {
                id: txAFW
                color: "#9b23d7"
                text: qsTr("AFW")
                horizontalAlignment: Text.AlignLeft
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
            }

            Text {
                id: valAFW
                color: "#08b308"
                text: qsTr("12.0")
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                horizontalAlignment: Text.AlignRight
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
                width: valWidthR
            }

            Text {
                id: txTPSV
                color: "#9b23d7"
                text: qsTr("TPSV")
                horizontalAlignment: Text.AlignLeft
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
            }

            Text {
                id: valTPSV
                color: "#08b308"
                text: qsTr("4.9")
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                horizontalAlignment: Text.AlignRight
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
                width: valWidthR
            }
            Text {
                id: txPIMV
                color: "#9b23d7"
                text: qsTr("PIMV")
                horizontalAlignment: Text.AlignLeft
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
            }

            Text {
                id: valPIMV
                color: "#08b308"
                text: qsTr("1.0")
                textFormat: Text.PlainText
                antialiasing: false
                enabled: false
                horizontalAlignment: Text.AlignRight
                font.pixelSize: valFontSize
                font.family: gaugeFont.name
                verticalAlignment: Text.AlignVCenter
                smooth: true
                width: valWidthR
            }
        }

        Grid {
            id: gridDbg
            rows: 3
            spacing: 14
            columns: 2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60
            smooth: true

            Text {
                id: txUpdateRate
                color: "#9b23d7"
                text: qsTr("U")
                horizontalAlignment: Text.AlignLeft
                antialiasing: false
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: dbgFontSize
                textFormat: Text.PlainText
                font.family: gaugeFont.name
            }

            Text {
                id: valBusRate
                width: 40
                color: "#08b308"
                text: qsTr("50")
                horizontalAlignment: Text.AlignRight
                antialiasing: false
                verticalAlignment: Text.AlignVCenter
                textFormat: Text.PlainText
                font.pixelSize: dbgFontSize
                font.family: gaugeFont.name
            }

            Text {
                id: txNumErrors
                color: "#9b23d7"
                text: qsTr("E")
                horizontalAlignment: Text.AlignLeft
                antialiasing: false
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: dbgFontSize
                textFormat: Text.PlainText
                font.family: gaugeFont.name
            }

            Text {
                id: valBusErrors
                width: 40
                color: "#ef0f0f"
                text: qsTr("0")
                antialiasing: false
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                font.pixelSize: 25
                textFormat: Text.PlainText
                font.family: gaugeFont.name
            }
        }

        Text {
            id: valSW
            color: "#4d524d"
            text: qsTr("SW 0101010101010101")
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            font.family: gaugeFont.name
            antialiasing: false
            textFormat: Text.PlainText
            font.pixelSize: 20
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
        }

        Text {
            id: valGV
            color: "#4d524d"
            text: qsTr("G 65535.0 V 65535.0")
            anchors.right: parent.right
            anchors.rightMargin: 20
            font.family: gaugeFont.name
            verticalAlignment: Text.AlignVCenter
            anchors.bottom: parent.bottom
            font.pixelSize: 20
            horizontalAlignment: Text.AlignRight
            textFormat: Text.PlainText
            anchors.bottomMargin: 0
            antialiasing: false
        }
    }
}
