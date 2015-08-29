import QtQuick 2.5
import Material 0.1

Item {
	id: root
//	property double length: Units.dp(100)
	property double length: gridLength
	property string color: "grey"
	property int posX: 0
	property int posY: 0
	x: posX * length
	y: posY * length
	
	function ripple() {
		var x = ink.width / 2;
		var y = ink.height / 2;
		ink.createTapCircle(x, y);
		ink.currentCircle.removeCircle();
	}
	
	states: [
		State {
			name: "hidden"
			PropertyChanges { target: view; scale: 0.0 }
		},
		State {
			name: "shown"
			PropertyChanges { target: view; scale: 1.0 }
		}
	]
	state: "hidden"

	View {
		id: view
		elevation: 1
		x: length * (1.0 - scale) / 2
		y: x
		width: length * scale
		height: width
		radius: length / 20
		
		property double scale: 1.0
		
		Behavior on scale {
			SpringAnimation {
				spring: 2.0
				damping: 0.25
				epsilon: 0.01
			}
		}

		Ink {
			id: ink
			anchors.fill: parent
			
			Rectangle {
				id: rect
				anchors.fill: parent
				radius: length / 20
				opacity: 0.3
				color: Palette.colors[root.color]["500"]
		//		border.width: modelData === "white" ? Units.dp(2) : 0
		//		border.color: Theme.alpha("#000", 0.26)
				border.width: length / 50
				border.color: Palette.colors[root.color]["700"]
				
				Behavior on color {
					ColorAnimation { duration: 100 }
				}
			}
		}
	}
}

