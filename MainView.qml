import QtQuick 2.5
import Material 0.1
import Material.ListItems 0.1 as ListItem
import Material.Extras 0.1
import QtQuick.Controls 1.2 as QuickControls
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2 as QuickDialogs

ApplicationWindow {
	id: main
	
	onClosing: {
		close.accepted = false
		quitConfirmDialog.show()
	}
	
	Dialog {
		id: quitConfirmDialog
		title: qsTr("Confirm quit", "quit dialog")
		text: qsTr("Are you sure you want to quit?")
		hasActions: true
		onAccepted: Qt.quit()
	}
	
	function delay(delayTime, cb) {
		var delayTimer = Qt.createQmlObject('import QtQuick 2.0; Timer { }', main);
		delayTimer.interval = delayTime;
		delayTimer.repeat = false;
		delayTimer.triggered.connect(cb);
		delayTimer.start();
	}
	
	minimumWidth: Units.dp(400)
	minimumHeight: Units.dp(400)

	title: qsTr("FlowFree", "app title")

	theme {
		primaryColor: Palette.colors["blue"]["400"]
		primaryDarkColor: Palette.colors["blue"]["900"]
		accentColor: Palette.colors["teal"]["500"]
		tabHighlightColor: "white"
	}

	property int difficultyCnt: 4
	property var levelsCnt: [ 5, 5, 5, 5 ]
	property var levelsPrefix: [ "easy", "medium", "hard", "expert" ]
/*
	property var languages: [ "zh_CN", "zh_TW", "en_UK", "ja_JP" ]
	property var languagesName: createLanguageName(languages)
	function createLanguageName(languages) {
		var ret = [];
		for (var i = 0; i < languages.length; ++i) {
			var locale = Qt.locale(languages[i])
			console.log(locale.nativeLanguageName);
			ret.push(locale.nativeLanguageName)
		}
		return ret;
	}
*/
	property var sections: createSections()
	function createSections() {
		var ret = [];
		for (var i = 0; i < difficultyCnt; ++i) {
			var levels = createLevels(levelsPrefix[i], levelsCnt[i])
			ret.push(levels)
		}
		return ret;
	}
	function createLevels(prefix, cnt) {
		var ret = [];
		for (var i = 1; i <= cnt; ++i) {
			var str = prefix + "-" + String(i);
//			console.log(str);
			ret.push(str);
		}
		return ret;
	}
	property var sectionTitles: [ qsTr("Easy (5x5 and 6x6)"), qsTr("Medium (7x7 and 8x8)"),
		qsTr("Hard (9x9 and 10x10)"), qsTr("Expert (11x11 and above)") ]
	property int levelDifficulty: 0
	property int levelNumber: 0
	property string selectedLevel//: sections[levelDifficulty][levelNumber]
		// better set manually
	
	onSelectedLevelChanged: {
		loader.sourceComponent = undefined
		loader.sourceComponent = gameboardComponent
		loader.active = true
	}
	
	Component.onCompleted: setLevel()
	
	function setLevel() {
		selectedLevel = ":/levels/" + sections[levelDifficulty][levelNumber] + ".txt"
	}
	
	function isFirstLevel() {
		return levelDifficulty == 0 && levelNumber == 0;
	}
	
	function isLastLevel() {
		return levelDifficulty + 1 == difficultyCnt
				&& levelNumber + 1 == levelsCnt[levelDifficulty];
	}
	
	function nextLevel() {
		if (isLastLevel()) {
			levelDifficulty = 0;
			levelNumber = 0;
			setLevel();
			return ;
		}

		if (levelNumber + 1 == levelsCnt[levelDifficulty]) {
			++levelDifficulty;
			levelNumber = 0;
		} else {
			++levelNumber;
		}
		setLevel()
	}
	
	function previousLevel() {
		if (isFirstLevel()) return ;
		if (levelNumber == 0) {
			--levelDifficulty;
			levelNumber = levelsCnt[levelDifficulty] - 1;
		} else {
			--levelNumber;
		}
		setLevel()
	}
	
	Dialog {
		id: solvingDialog
		title: qsTr("Solving...")
//		text: qsTr("This may take a while")
		negativeButtonText: qsTr("Abort")
		positiveButton.visible: false
		
		hasActions: false
		dismissOnTap: false
		
		property alias customText: label.text
		
		RowLayout {
			spacing: Units.dp(20)
			ProgressCircle {
				indeterminate: true
				width: Units.dp(50)
				height: Units.dp(50)
				dashThickness: Units.dp(5)
			}
			Label {
				id: label
				text: qsTr("This may take a while...")
				style: "dialog"
				Layout.alignment: Qt.AlignVCenter
			}
		}

		onRejected: {
			loader.item.snackbar.duration = 2000
			loader.item.snackbar.open("Auto-solve process canceled")
			loader.item.abortSolve()
		}
	}
	
	Dialog {
		id: showSolutionDialog
		title: qsTr("Confirm", "show solution dialog")
		text: qsTr("Are you sure you want to show the solution?")
		hasActions: true
		onAccepted: {
			solvingDialog.show()
			delay(200, loader.item.solve)
		}
	}
	
	Dialog {
		id: cannotSolveDialog
		title: qsTr("Level too complex")
		text: qsTr("This level is too complex to be solved. Sorry :(")
		positiveButton.visible: false
		negativeButtonText: "OK"
	}
	
	function trySolve() {
		if (loader.item.canSolve()) {
			showSolutionDialog.show()
		} else {
			cannotSolveDialog.show()
		}
	}
	
	HelpDialog {
		id: helpDialog
	}
	
	QuickDialogs.FileDialog {
		id: fileDialog
		title: "Select custom level data file"
		onAccepted: {
			console.log(fileDialog.fileUrl)
			main.selectedLevel = fileDialog.fileUrl
			main.levelDifficulty = -1
		}
	}
	
	initialPage: Page {
		id: page

		title: qsTr("FlowFree", "page title")

		tabs: []

		actionBar.maxActionCount: 3

		actions: [
			Action {
				iconName: "navigation/chevron_left"
				name: qsTr("Previous Level")
				enabled: levelDifficulty >= 0 && !isFirstLevel()
				onTriggered: previousLevel()
			},

			Action {
				iconName: "navigation/chevron_right"
				name: qsTr("Next Level")
				enabled: levelDifficulty >= 0 && !isLastLevel()
				onTriggered: nextLevel()
			},
			
			Action {
				iconName: "file/folder_open"
				name: qsTr("Load Custom Level")
				enabled: true
				onTriggered: fileDialog.open()
			},

			Action {
				iconName: "action/assignment"
				name: qsTr("Auto Solve")
				enabled: true
				onTriggered: trySolve()
			},
			
			Action {
				iconName: "action/help"
				name: qsTr("Instructions")
				enabled: true
				onTriggered: helpDialog.show()
			},

/*
			Action {
				iconName: "action/settings"
				name: qsTr("Settings")
				hoverAnimation: true
				onTriggered: settings.show()
			}*/
			
			Action {
				iconName: "action/exit_to_app"
				name: qsTr("Quit")
				enabled: true
				onTriggered: quitConfirmDialog.show()
			}
		]

		backAction: navDrawer.action

		property int levelColumns: Math.min(5, Math.max(3, Math.floor(page.width / 200)))
		
		NavigationDrawer {
			id: navDrawer
			width: Units.gu(page.levelColumns)

			enabled: page.width < Units.dp(750)

			Flickable {
				id: navBarFlickable
				anchors.fill: parent
				contentHeight: Math.max(navBarContent.implicitHeight, height)

				Column {
					id: navBarContent
					anchors.fill: parent

					Repeater {
						model: sections
						delegate: ColumnLayout {
							width: parent.width
							
							ListItem.Subheader {
								id: navBarHeader
								text: sectionTitles[index]
							}
							
							Grid {
								id: navBarGrid
								
								Layout.alignment: Qt.AlignHCenter
								anchors.top: navBarHeader.bottom
								width: parent.width - Units.dp(10)
								columns: page.levelColumns
								spacing: Units.dp(10)
								property int curDifficulty: index
								
								Repeater {
									model: modelData
									delegate: ClickableSquare {
										text: index + 1
										onClicked: {
											main.levelDifficulty = navBarGrid.curDifficulty
											main.levelNumber = index
											main.setLevel()
										}
										selected: main.levelDifficulty == navBarGrid.curDifficulty
											&& main.levelNumber == index
									}
								}
							}
						}
					}
				}
			}
			Scrollbar {
				flickableItem: navBarFlickable
			}
		}
		
		Sidebar {
			id: sidebar
			width: Units.gu(page.levelColumns)
			expanded: !navDrawer.enabled
			
			Column {
				width: parent.width

				Repeater {
					id: repeater
					model: sections
					
					delegate: ColumnLayout {
						width: parent.width
						
						ListItem.Subheader {
							id: sidebarHeader
							text: sectionTitles[index]
						}
						
						Grid {
							id: sidebarGrid
							
							Layout.alignment: Qt.AlignHCenter
							anchors.top: sidebarHeader.bottom
							width: parent.width - Units.dp(10)
							columns: page.levelColumns
							spacing: Units.dp(10)
							property int curDifficulty: index
							
							Repeater {
								model: modelData
								delegate: ClickableSquare {
									text: index + 1
									onClicked: {
										main.levelDifficulty = sidebarGrid.curDifficulty
										main.levelNumber = index
										loader.active = false
										delay(levelDifficulty >= 2 ? 100 : 10, main.setLevel)
									}
									selected: main.levelDifficulty == sidebarGrid.curDifficulty
										&& main.levelNumber == index
								}
							}
						}
					}
				}
			}
		}
		
		Loader {
			id: loader
			
			anchors {
				left: sidebar.right
				right: parent.right
				top: parent.top
				bottom: parent.bottom
				margins: Units.dp(30)
			}
			asynchronous: true
			active: false
			visible: status == Loader.Ready
			
			sourceComponent: undefined
		}
		
		Item {
			anchors {
				left: sidebar.right
				right: parent.right
				top: parent.top
				bottom: parent.bottom
				margins: Units.dp(30)
			}
			ProgressCircle {
				id: progressCircle
				
				anchors.centerIn: parent
				visible: loader.status != Loader.Ready
				width: Math.min(parent.width, parent.height) * 0.15
				height: width
				dashThickness: width / 10
				
				indeterminate: false
				minimumValue: 0
				maximumValue: 100
				
				SequentialAnimation on value {
					running: progressCircle.visible
//					loops: NumberAnimation.Infinite
	
					NumberAnimation {
						duration: levelDifficulty >= 2 ? 150 : 50
						from: progressCircle.minimumValue
						to: progressCircle.maximumValue
					}
				}
				
				Label {
					font.pixelSize: progressCircle.width * 0.22
					anchors.centerIn: parent
					text: Math.round(progressCircle.value) + "%"
				}
			}
		}
		
		Component {
			id: gameboardComponent
			GameBoard {
				id: gameboard
				anchors {
					left: sidebar.right
					right: page.right
					top: page.top
					bottom: page.bottom
					margins: Units.dp(30)
				}
				levelName: main.selectedLevel
				property alias snackbar: snackbar
				
				onGameFinished: {
					snackbar.buttonText = levelDifficulty < 0 ? "" : (isLastLevel() ? qsTr("Start Again") : qsTr("Next Level"))
                    snackbar.open(levelDifficulty < 0 ? qsTr("Congratz! You solved this level!")
                                : (isLastLevel()
								  ? qsTr("Congratz! You've solved every puzzle!\nStart again from first level?")
								  : qsTr("Congratz! You solved this level!\nProceed to next level?")))
					snackbar.duration = NumberAnimation.Infinite
				}
				onGameNeedFill: {
					snackbar.buttonText = ""
					snackbar.duration = 4000
					snackbar.open(qsTr("Almost there...\nYou still need to fill the whole board :("))
				}
				onPressed: snackbar.opened = false
				onSolveFinished: {
					solvingDialog.close()
					snackbar.buttonText = ""
					snackbar.duration = 4000
					snackbar.open(qsTr("Solution found in %1 ms").arg(time))
				}
				
				onLoadFailed: {
					loadFailedDialog.text = message
					loadFailedDialog.show()
				}
				
				Snackbar {
					id: snackbar
					
					onClicked: nextLevel()
				}
				
				ActionButton {
					id: actionButton
					
					anchors {
						right: parent.right
						bottom: snackbar.top
						margins: Units.dp(20)
					}
					iconName: "navigation/refresh"
					
					onClicked: gameboard.restart()
					
					isMiniSize: navDrawer.enabled
				}
			}
		}
	}
	
	Dialog {
		id: loadFailedDialog
		title: qsTr("Load failed")
		negativeButtonText: "OK"
		positiveButton.visible: false
	}
/* 
	Dialog {
		id: settings
		title: qsTr("Settings", "settings dialog")
		positiveButtonText: qsTr("Done", "settings dialog")
		
		GridLayout {
			rowSpacing: Units.dp(5)
			columnSpacing: Units.dp(5)
			columns: 2
			
			// Theme
			Label {
				text: qsTr("Theme: ", "settings dialog")
				style: "dialog"
				Layout.alignment: Qt.AlignRight
			}
			RowLayout {
				id: themeRadioButtonGroup
				QuickControls.ExclusiveGroup { id: themeOptionGroup }
				Layout.alignment: Qt.AlignLeft
				RadioButton {
					checked: true
					text: qsTr("Light", "theme")
					canToggle: true
					exclusiveGroup: themeOptionGroup
					onClicked: main.theme.light = true
				}
				RadioButton {
					checked: false
					text: qsTr("Dark", "theme")
					canToggle: true
					exclusiveGroup: themeOptionGroup
					onClicked: main.theme.light = false
				}
			}
			
			// Language
			Label {
				text: qsTr("Language: ")
				style: "dialog"
				Layout.alignment: Qt.AlignRight
			}
			MenuField {
				Layout.alignment: Qt.AlignLeft
				//Layout.preferredWidth: themeRadioButtonGroup.width
				//Layout.preferredWidth: 

				model: languagesName
				
				onItemSelected: {
					// change language
				}
			}
		}
	}
*/
/*
	Dialog {
		id: colorPicker
		title: "Pick color"

		positiveButtonText: "Done"

		MenuField {
			id: selection
			model: ["Primary color", "Accent color", "Background color"]
			width: Units.dp(160)
		}

		Grid {
			columns: 7
			spacing: Units.dp(8)

			Repeater {
				model: [
					"red", "pink", "purple", "deepPurple", "indigo",
					"blue", "lightBlue", "cyan", "teal", "green",
					"lightGreen", "lime", "yellow", "amber", "orange",
					"deepOrange", "grey", "blueGrey", "brown", "black",
					"white"
				]

				Rectangle {
					width: Units.dp(30)
					height: Units.dp(30)
					radius: Units.dp(2)
					color: Palette.colors[modelData]["500"]
					border.width: modelData === "white" ? Units.dp(2) : 0
					border.color: Theme.alpha("#000", 0.26)

					Ink {
						anchors.fill: parent

						onPressed: {
							switch(selection.selectedIndex) {
								case 0:
									theme.primaryColor = parent.color
									break;
								case 1:
									theme.accentColor = parent.color
									break;
								case 2:
									theme.backgroundColor = parent.color
									break;
							}
						}
					}
				}
			}
		}

		onRejected: {
			// TODO set default colors again but we currently don't know what that is
		}
	}
	*/
}
