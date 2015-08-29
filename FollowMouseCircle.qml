import QtQuick 2.0
import Material 0.1

// Should be placed directly under a MouseArea
Item {
	id: root
	
	property alias radius: followRect.radius
	property string color: "blue"
	
	states: [
		State {
			name: "hidden"
			PropertyChanges { target: followRect; scale: 0.0 }
			StateChangeScript {
				script: breatheAnimation.stop();
			}
		},
		State {
			name: "shown"
			PropertyChanges { target: followRect; scale: 1.0 }
			StateChangeScript {
				script: breatheAnimation.start();
			}
		}
	]
	state: "hidden"
	transitions: [
		Transition {
			from: "hidden"; to: "shown"
			NumberAnimation { properties: "scale"; easing.type: Easing.OutExpo; duration: 200 }
		},
		Transition {
			from: "shown"; to: "hidden"
			NumberAnimation { properties: "scale"; easing.type: Easing.InExpo; duration: 200 }
		}
	]
	
	Rectangle {
		id: followRect
		radius: Units.dp(50)
		width: radius * 2
		height: radius * 2
		color: Palette.colors[root.color]["300"]
		opacity: 0.4
		
		property real targetX: root.parent.mouseX
		property real targetY: root.parent.mouseY
		
		x: targetX - radius * scale
		y: targetY - radius * scale
		
		property real scale: 0.0
		
		transformOrigin: Item.Center
		transform: Scale {
			xScale: followRect.scale
			yScale: followRect.scale
		}
		
		SequentialAnimation on scale {
			id: breatheAnimation
			
			running: false
			
			PauseAnimation {
				duration: 200
			}
			SequentialAnimation {
				loops: Animation.Infinite
				PropertyAnimation {
					duration: 2000
					to: 1.5
					easing.type: Easing.InOutQuad
				}
				PropertyAnimation {
					duration: 2000
					to: 1.00
					easing.type: Easing.InOutQuad
				}
			}
		}
	}
}

