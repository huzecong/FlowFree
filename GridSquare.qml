import QtQuick 2.0
import Material 0.1

Item {
	id: root
	property alias length: view.width
	property string color: "grey"
	
	signal pressed(MouseEvent mouse)
	signal positionChanged(MouseEvent mouse)
	signal clicked(MouseEvent mouse)

	View {
		id: view
		elevation: 1
		width: 100
		height: width
		radius: width / 20

		Rectangle {
			id: rect
			anchors.fill: parent
			radius: width / 20
			opacity: 0.8
			color: Palette.colors[parent.color]["500"]
	//		border.width: modelData === "white" ? Units.dp(2) : 0
	//		border.color: Theme.alpha("#000", 0.26)
			border.width: width / 50
			border.color: Palette.colors[parent.color]["700"]

			Ink {
				id: ink
				anchors.fill: parent				
//				drag.target: view
				onPressed: root.pressed(mouse)
				onPositionChanged: root.positionChanged(mouse)
				onClicked: root.clicked(mouse)
			}
		}
	}
}

