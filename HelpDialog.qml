import QtQuick 2.5
import Material 0.1
import QtQuick.Layouts 1.1

Dialog {
    id: dialog
    width: Units.dp(400)
    title: qsTr("Instructions")
    text: qsTr("Drag to connect matching colors with pipe, creating a flow.\n
Pair all colors, and cover the entire board with pipe to solve each puzzle.\n
But watch out, pipes will break if they cross or overlap!\n
Too easy? Try selecting a larger board.\n
Too hard? Try using the auto-solve function. But note that the last level is too complex for the solver, so you're on yourself on that one.\n");
    negativeButtonText: "OK"
    positiveButton.visible: false
}

