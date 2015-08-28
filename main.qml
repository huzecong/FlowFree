import QtQuick 2.2
import Material 0.1
import Material.ListItems 0.1 as ListItem
import Material.Extras 0.1
import QtQuick.Controls 1.2 as QuickControls
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0


Window {
	width: 0
	height: 0
	visible: true

	FlowFreeMainView {
		visible: true
		GridSquare {
			width: Units.dp(200)
			//color: "red"
		}
	}
}

