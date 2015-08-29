import QtQuick 2.5
import Material 0.1

Item {
	id: root
	property double length: Units.dp(45)
	property alias text: label.text
	property bool selected: false
	
	signal clicked()
	
	width: length * (1.0 + borderExtend)
	height: width
	
	property double borderExtend: 0.07
	
	View {
		id: view
		anchors.fill: parent
		elevation: root.selected ? 2 : 1
		radius: length / 15 * scale
		scale: 1.0 + (root.selected ? root.borderExtend : 0.0)
		
		Behavior on scale {
			NumberAnimation { duration: 1250 * root.borderExtend }
		}

		Rectangle {
			id: rect
			anchors.fill: parent
			radius: length / 15 * view.scale
			color: Palette.colors["white"]["500"]
//			border.width: modelData === "white" ? Units.dp(2) : 0
			border.color: Theme.alpha("#000", root.selected ? 0.40 : 0.26)
			border.width: root.selected ? length * root.borderExtend : length * 0.02
			
			Behavior on border.color {
				ColorAnimation { duration: 1250 * root.borderExtend }
			}
			
			Behavior on border.width {
				NumberAnimation { duration: 1250 * root.borderExtend }
			}
			
			Behavior on color {
				ColorAnimation { duration: 200 }
			}
			
			Label {
				id: label
				
				anchors.centerIn: parent
//				width: parent.width
//				wrapMode: Text.Wrap
				text: "ID"
				style: "dialog"
			}
			
			Ink {
				id: ink
				anchors.fill: parent
				
				onPressed: root.clicked()
			}
		}
	}
}

