import QtQuick 2.2
import Material 0.1
import Material.ListItems 0.1 as ListItem
import Material.Extras 0.1
import QtQuick.Controls 1.2 as QuickControls
import QtQuick.Layouts 1.2

ApplicationWindow {
	id: main

	title: qsTr("FlowFree", "app title")

	theme {
		primaryColor: Palette.colors["blue"]["400"]
		primaryDarkColor: Palette.colors["blue"]["900"]
		accentColor: Palette.colors["teal"]["500"]
		tabHighlightColor: "white"
	}

	property int easyLevelsCnt: 5
	property int mediumLevelsCnt: 5
	property int hardLevelsCnt: 5
	property var easyLevels: createLevels("easy", easyLevelsCnt)
	property var mediumLevels: createLevels("medium", mediumLevelsCnt)
	property var hardLevels: createLevels("hard", hardLevelsCnt)
	function createLevels(prefix, cnt) {
		var ret = [];
		var i = 0;
		for (i = 1; i <= cnt; ++i) {
			var str = prefix + "-" + String(i);
			console.log(str);
			ret.push(str);
		}
		return ret;
	}

	property var languages: [ "zh_CN", "zh_TW", "en_UK", "ja_JP" ]
	property var languagesName: createLanguageName(languages)
	function createLanguageName(languages) {
		var ret = [];
		var i = 0;
		for (i = 0; i < languages.length; ++i) {
			var locale = Qt.locale(languages[i])
			console.log(locale.nativeLanguageName);
			ret.push(locale.nativeLanguageName)
		}
		return ret;
	}

	property var sections: [ easyLevels, mediumLevels, hardLevels ]

	property var sectionTitles: [ qsTr("Easy"), qsTr("Medium"), qsTr("Hard") ]

	property string selectedLevel: easyLevels[0]

	initialPage: Page {
		id: page

		title: qsTr("FlowFree", "page title")

		tabs: []

		actionBar.maxActionCount: 3

		actions: [
			Action {
				iconName: "navigation/chevron_left"
				name: qsTr("Previous Level")
				enabled: false
			},

			Action {
				iconName: "navigation/chevron_right"
				name: qsTr("Next Level")
				enabled: true
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

		NavigationDrawer {
			id: navDrawer
			width: Units.gu(3)

			enabled: page.width < Units.dp(700)

			Flickable {
				anchors.fill: parent
				contentHeight: Math.max(navBarContent.implicitHeight, height)

				Column {
					id: navBarContent
					anchors.fill: parent

					Repeater {
						model: sections
						delegate: Column {
							width: parent.width

							ListItem.Subheader {
								text: sectionTitles[index]
							}
							Repeater {
								model: modelData
								delegate: ListItem.Standard {
									text: index + 1
									selected: main.selectedLevel == modelData
									onClicked: main.selectedLevel = modelData
								}
							}
						}
					}
				}
			}
		}
		
		Sidebar {
			id: sidebar
			width: Units.gu(3)
			expanded: !navDrawer.enabled

			Flickable {
				anchors.fill: parent
				contentHeight: Math.max(sidebarContent.implicitHeight, height)

				Column {
					id: sidebarContent
					anchors.fill: parent

					Repeater {
						model: sections
						delegate: Column {
							width: parent.width

							ListItem.Subheader {
								text: sectionTitles[index]
							}
							Repeater {
								model: modelData
								delegate: ListItem.Standard {
									text: index + 1
									selected: main.selectedLevel == modelData
									onClicked: main.selectedLevel = modelData
								}
							}
						}
					}
				}
			}
		}
		
		Rectangle {
			id: gameboard
			anchors {
				left: sidebar.right
				right: parent.right
				top: parent.top
				bottom: parent.bottom
			}
		}
		
/*
		TabView {
			id: tabView
			anchors.fill: parent
			currentIndex: page.selectedTab
			model: sections

			delegate: Item {
				width: tabView.width
				height: tabView.height
				clip: true

				property string selectedComponent: modelData[0]

				
				Flickable {
					id: flickable
					anchors {
						left: sidebar.right
						right: parent.right
						top: parent.top
						bottom: parent.bottom
					}
					clip: true
					contentHeight: Math.max(example.implicitHeight + 40, height)
					Loader {
						id: example
						anchors.fill: parent
						asynchronous: true
						visible: status == Loader.Ready
						// selectedComponent will always be valid, as it defaults to the first component
						source: {
							if (navDrawer.enabled) {
								return Qt.resolvedUrl("%1Demo.qml").arg(demo.selectedComponent.replace(" ", ""))
							} else {
								return Qt.resolvedUrl("%1Demo.qml").arg(selectedComponent.replace(" ", ""))
							}
						}
					}

					ProgressCircle {
						anchors.centerIn: parent
						visible: example.status == Loader.Loading
					}
				}
				Scrollbar {
					flickableItem: flickable
				}
			}
		}
		*/
	}

	ActionButton {
		anchors {
			right: parent.right
			bottom: snackbar.top
			margins: Units.dp(32)
		}

		iconName: "navigation/refresh"
	}

	Snackbar {
		id: snackbar
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
