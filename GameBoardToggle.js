var rows = 1;
var columns = 1;
var grids = new Array(rows * columns);
var circles = new Array(rows * columns);
var verLines = new Array(rows * columns);
var horLines = new Array(rows * columns);
var components = new Array(3)
var gridLength = 0;

function index(x, y) {
	return x * columns + y
}

function createBoard(n, m, length) {
	// Remove previous blocks
	for (var i = 0; i < rows * columns; ++i) {
		if (grids[i] != null)
			grids[i].destroy();
		if (circles[i] != null)
			circles[i].destroy();
		if (verLines[i] != null)
			verLines[i].destroy();
		if (horLines[i] != null)
			horLines[i].destroy();
	}
	
	rows = n;
	columns = m;
	gridLength = length;
	grids = new Array(rows * columns);
	circles = new Array(rows * columns);
	verLines = new Array(rows * columns);
	horLines = new Array(rows * columns);
	for (var i = 0; i < rows; ++i)
		for (var j = 0; j < columns; ++j) {
			var id = index(i, j);
			grids[id] = createObject(0, "Grid", i, j);
			grids[id].state = "shown";
		}
	for (var i = 0; i < rows; ++i)
		for (var j = 0; j < columns; ++j) {
			var id = index(i, j);
			if (i + 1 < rows) {
				verLines[id] = createObject(2, "Line", i, j);
				verLines[id].vertical = true;
			} else {
				verLines[id] = null;
			}
			if (j + 1 < columns) {
				horLines[id] = createObject(2, "Line", i, j);
				horLines[id].vertical = false;
			} else {
				horLines[id] = null;
			}
		}
	for (var i = 0; i < rows; ++i)
		for (var j = 0; j < columns; ++j) {
			var id = index(i, j);
			circles[id] = createObject(1, "Circle", i, j);
		}
}

function createObject(index, name, x, y) {
	if (components[index] == null)
		components[index] = Qt.createComponent("Board" + name + ".qml");
	
	if (components[index].status == Component.Ready) {
		var obj = components[index].createObject(board);
		if (obj == null) {
			console.log("error creating " + name + ": " + components[index].errorString());
			return null;
		}
//		obj.length = gridLength;
		obj.posY = x;
		obj.posX = y;
		return obj;
	} else {
		console.log("error loading " + name + ": " + components[index].errorString());
		return null;
	}
}

function hideAll() {
	for (var i = 0; i < rows; ++i)
		for (var j = 0; j < columns; ++j) {
			hideLine(i, j, i + 1, j);
			hideLine(i, j, i, j + 1);
			hideCircle(i, j);
			hideGrid(i, j);
		}
}

function hideGrid(x, y) {
	var id = index(x, y);
	if (grids[id] != null) {
		grids[id].state = "hidden"
	}
}

function showCircle(x, y, color) {
	var id = index(x, y)
	if (circles[id] != null) {
		circles[id].color = color
		circles[id].state = "shown"
	}
}

function hideCircle(x, y) {
	var id = index(x, y);
	if (circles[id] != null) {
		circles[id].state = "hidden"
	}
}

function showLine(x1, y1, vertical, color) {
	var id = index(x1, y1);
	if (!vertical) {
		if (horLines[id] != null) {
			horLines[id].color = color;
			horLines[id].state = "shown";
		}
	} else if (verLines[id] != null) {
		verLines[id].color = color;
		verLines[id].state = "shown";
	}
}

function hideLine(x1, y1, vertical) {
	var id = index(x1, y1);
	if (!vertical) {
		if (horLines[id] != null) {
			horLines[id].state = "hidden";
		}
	} else if (verLines[id] != null) {
		verLines[id].state = "hidden";
	}
}

function changeGridColor(x, y, color) {
	var id = index(x, y);
	if (grids[id] != null) {
		grids[id].color = color;
	}
}

function ripple(x, y) {
	var id = index(x, y);
	if (grids[id] != null) {
		grids[id].ripple();
	}
}
