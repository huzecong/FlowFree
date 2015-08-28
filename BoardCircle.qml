import QtQuick 2.5
import Material 0.1

Item {
	id: root
//	property double length: Units.dp(100)
	property double length: parent.parent.gridLength
	property string color: "red"
	property int posX: 0
	property int posY: 0
	x: posX * length
	y: posY * length
	
	states: [
		State {
			name: "hidden"
			PropertyChanges { target: view; scale: 0.0 }
            PropertyChanges { target: view; visible: false }
		},
		State {
			name: "shown"
			PropertyChanges { target: view; scale: 1.0 }
            PropertyChanges { target: view; visible: true }
		}
	]
	state: "hidden"
	
	width: length
	height: length

	View {
		id: view
		anchors.centerIn: parent
		elevation: 2
		width: (length / 4 * 3) * scale
		height: width
		radius: width / 2
        antialiasing: true
		
		property double scale: 0.0
		
		Behavior on scale {
			SpringAnimation {
				spring: 1.2
				damping: 0.12
				epsilon: 0.01
			}
		}

		Rectangle {
			id: rect
			anchors.fill: parent
			radius: width / 2
            antialiasing: true
			color: Palette.colors[root.color]["500"]
		}
	}
}

