import QtQuick 2.5
import Material 0.1

Item {
	id: root
//    property double length: Units.dp(100)
	property double length: gridLength
	property string color: "red"
	property bool vertical: true    // false for horizontal
	property int posX: 0
	property int posY: 0
	x: posX * length
	y: posY * length

	states: [
		State {
			name: "hidden"
//			PropertyChanges { target: view; scale: 1.2 }
			PropertyChanges { target: view; opacity: 0.0 }
		},
		State {
			name: "shown"
//			PropertyChanges { target: view; scale: 1.0 }
			PropertyChanges { target: view; opacity: 1.0 }
		}
	]
	state: "hidden"
	
	width: length * 2
	height: length * 2
	
	View {
		id: view
		elevation: 0
		x: length / 2 - radius - width / scale * (scale - 1.0) / 2
		y: length / 2 - radius - height / scale * (scale - 1.0) / 2
		width: ((vertical ? 0 : length) + radius * 2) * scale
		height: ((vertical ? length : 0) + radius * 2) * scale
		radius: length / 16 * 3
		
		property double scale: 1.0
		Behavior on scale {
			PropertyAnimation { duration: 75 }
		}
		
		opacity: 1.0
		Behavior on opacity {
			PropertyAnimation { duration: 75 }
		}

		Rectangle {
			id: rect
			anchors.fill: parent
			radius: parent.radius
			color: Palette.colors[root.color]["500"]
		}
	}
}

