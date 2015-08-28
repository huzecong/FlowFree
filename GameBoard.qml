import QtQuick 2.0
import Material 0.1
import com.Kanari.GameLogic 1.0
import "GameBoardToggle.js" as Toggle

Item {
	id: root

	property int n: 5
	property int m: 5
	property double gridLength: Math.min(width / m, height / n)
	property string levelName
	
	onLevelNameChanged: logic.loadLevel(levelName)
	
	signal gameFinished()
	
	function restart() {
		logic.restart()
	}
	
	GameLogic {
		id: logic
		
		onLoadFinished: {
			n = height
			m = width
			Toggle.createBoard(n, m, gridLength)
			logic.displayCircles()
		}
		onHideAll: Toggle.hideAll()
		onRipple: Toggle.ripple(x, y)
		onShowCircle: Toggle.showCircle(x, y, color)
		onHideCircle: Toggle.hideCircle(x, y)
		onShowLine: Toggle.showLine(x1, y1, vertical, color)
		onHideLine: Toggle.hideLine(x1, y1, vertical)
		
		onGameFinished: root.gameFinished()
	}
	
	Rectangle {
		id: board
		
		anchors.centerIn: parent
		width: m * gridLength
		height: n * gridLength
		color: Palette.colors["grey"]["500"]
		
		MouseArea {
			id: mouseArea
			
			anchors.fill: parent
			
			z: 10000
			
            onClicked: {
                var y = Math.floor(mouse.x / gridLength)
				var x = Math.floor(mouse.y / gridLength)
                console.log("clicked ", x, y)
            }
            
			onPressed: {
				var y = Math.floor(mouse.x / gridLength)
				var x = Math.floor(mouse.y / gridLength)
				console.log("pressed ", x, y)
				logic.startPath(x, y);
			}
			
			onPositionChanged: {
				var y = Math.floor(mouse.x / gridLength)
				var x = Math.floor(mouse.y / gridLength)
				console.log("moved ", x, y);
				logic.movePath(x, y);
			}
			
			onReleased: {
				var y = Math.floor(mouse.x / gridLength)
				var x = Math.floor(mouse.y / gridLength)
				console.log("released ", x, y)
				logic.endPath(x, y);
			}
		}
	}
}
