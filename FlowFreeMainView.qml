import QtQuick 2.5
import Material 0.1
import Material.ListItems 0.1 as ListItem
import Material.Extras 0.1
import QtQuick.Controls 1.2 as QuickControls
import QtQuick.Layouts 1.2

ApplicationWindow {
	id: main
    
    Timer {
        id: __delayTimer
    }
    
    function delay(delayTime, cb) {
        __delayTimer.interval = delayTime;
        __delayTimer.repeat = false;
        __delayTimer.triggered.connect(cb);
        __delayTimer.start();
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
			console.log(str);
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
        selectedLevel = sections[levelDifficulty][levelNumber]
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

/*	Component.onCompleted: {
		selectedLevel: sections[levelDifficulty][levelNumber]
	}
	*/
	initialPage: Page {
		id: page

		title: qsTr("FlowFree", "page title")

		tabs: []

		actionBar.maxActionCount: 3

		actions: [
			Action {
				iconName: "navigation/chevron_left"
				name: qsTr("Previous Level")
				enabled: !isFirstLevel()
                onTriggered: previousLevel()
			},

			Action {
				iconName: "navigation/chevron_right"
				name: qsTr("Next Level")
				enabled: !isLastLevel()
                onTriggered: nextLevel()
			},

			Action {
				iconName: "communication/live_help"
				name: qsTr("Show Solution")
				enabled: true
				onTriggered: showSolutionDialog.show()
			},

			Action {
				iconName: "action/settings"
				name: qsTr("Settings")
				hoverAnimation: true
				onTriggered: settings.show()
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
                                        delay(main.levelDifficulty * 150, main.setLevel)
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
//                    loops: NumberAnimation.Infinite
    
                    NumberAnimation {
                        duration: __delayTimer.interval
                        from: progressCircle.minimumValue
                        to: progressCircle.maximumValue
                    }
                }
                
                Label {
                    font.pixelSize: progressCircle.width * 0.24
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
                
                onGameFinished: {
                    snackbar.buttonText = isLastLevel() ? qsTr("Start Again") : qsTr("Next Level")
                    snackbar.open(isLastLevel()
                                  ? qsTr("Congratulations! You've solved every puzzle in the game!\nStart again from first level?")
                                  : qsTr("Congratulations! You solved this level!\nProceed to next level?"))
                    snackbar.duration = NumberAnimation.Infinite
                }
                onGameNeedFill: {
                    snackbar.buttonText = ""
                    snackbar.duration = 4000
                    snackbar.open(qsTr("Almost there...\nYou still need to fill the whole board :("))
                }
                onPressed: snackbar.opened = false
                
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
		id: showSolutionDialog
		title: qsTr("Confirm", "show solution dialog")
		text: qsTr("Are you sure you want to show the solution?")
		hasActions: true
		onAccepted: {
			console.log("solution shown")
		}
	}
    
    Dialog {
        id: gameFinishedDialog
        title: qsTr("Congratulations!")
        text: qsTr("You solved this level!")
        hasActions: true
        positiveButtonText: qsTr("Next Level")
        negativeButtonText: qsTr("OK")
        onAccepted: {
            console.log("next level")
        }
    }

	Dialog {
		id: settings
		title: qsTr("Settings", "settings dialog")
		positiveButtonText: qsTr("Done", "settings dialog")
		
		GridLayout {
			rowSpacing: Units.dp(5)
			columnSpacing: Units.dp(5)
			columns: 2
			/*
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
			*/
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
