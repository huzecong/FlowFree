import QtQuick 2.0
import Material 0.1
import QtMultimedia 5.0
import com.Kanari.GameLogic 1.0
import "GameBoardToggle.js" as Toggle

Item {
	id: root

	property int n: 5
	property int m: 5
	property double gridLength: Math.min(width / m, height / n)
	property string levelName
    
    property bool finished: false
	
	onLevelNameChanged: logic.loadLevel(levelName)
	
    signal pressed()
	signal gameFinished()
    signal gameNeedFill()
	
	function restart() {
		logic.restart()
	}
    
    SoundEffect {
        id: needFillSound
        source: "qrc:/sound/Blow.wav"
    }
	
    SoundEffect {
        id: lastConnectionSound
        source: "qrc:/sound/Hero.wav"
    }
    
    SoundEffect {
        id: connectedSound
        source: "qrc:/sound/Submarine.wav"
    }
    
    SoundEffect {
        id: disconnectedSound
        source: "qrc:/sound/Pop.wav"
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
        onChangeGridColor: Toggle.changeGridColor(x, y, color)
		onShowCircle: Toggle.showCircle(x, y, color)
		onHideCircle: Toggle.hideCircle(x, y)
		onShowLine: Toggle.showLine(x1, y1, vertical, color)
		onHideLine: Toggle.hideLine(x1, y1, vertical)
        
        onPlayConnectedSound: connectedSound.play()
        onPlayDisconnectedSound: disconnectedSound.play()
		
        onGameFinished: {
            lastConnectionSound.play()
            root.gameFinished()
            root.finished = true
        }
        onGameNeedFill: {
            needFillSound.play()
            root.gameNeedFill()
        }
	}
	
	Rectangle {
		id: board
		
		anchors.centerIn: parent
		width: m * gridLength
		height: n * gridLength
		color: Theme.backgroundColor
		
		MouseArea {
			id: mouseArea
			
			anchors.fill: parent
			
			z: 10000
			
            FollowMouseCircle {
                id: followCircle
            }
            
            function pressEvent(mouse) {
                if (!root.finished) {
                    var y = Math.floor(mouse.x / gridLength)
                    var x = Math.floor(mouse.y / gridLength)
                    var color = logic.colorAt(x, y)
                    console.log(color)
                    followCircle.color = color
                    followCircle.state = color != "none" ? "shown" : "hidden"
                    logic.startPath(x, y);
                }
            }

			onPressed: {
                root.pressed()
				pressEvent(mouse)
			}
			
			onPositionChanged: {
				var y = Math.floor(mouse.x / gridLength)
				var x = Math.floor(mouse.y / gridLength)
//				console.log("moved ", x, y);
				logic.movePath(x, y);
			}
			
			onReleased: {
				var y = Math.floor(mouse.x / gridLength)
				var x = Math.floor(mouse.y / gridLength)
//				console.log("released ", x, y)
				logic.endPath(x, y);
                followCircle.state = "hidden"
			}
		}
	}
}
