// ** CODE THAT BELONGS IN map.ts (IF ONLY I COULD GET IT TO IMPORT PROPERLY)

class MapLocation {
    public xloc: number;
    public yloc: number;
    public name: string;
    public note: string;

    constructor(newX: number, newY: number, newName: string, newNote: string) {
        this.xloc = newX;
        this.yloc = newY;
        this.name = newName;
        this.note = newNote;
    }
}

class Map {
    name: string;
    locations: MapLocation[]
    minX: number;
    minY: number;
    maxX: number;
    maxY: number;
    currentIndex: number; // zero-based index to current location

    constructor() {
        this.init();
    }

    init() {
        this.minX = 10000;
        this.minY = 10000;
        this.maxX = -10000;
        this.maxY = -10000;
        this.currentIndex = 0;
        this.locations = [];
    }

    extendBounds(newX: number, newY: number) {
        const gap = 100;
        if (newX < this.minX) this.minX = (newX - gap);
        if (newY < this.minY) this.minY = (newY - gap);
        if (newX > this.maxX) this.maxX = (newX + gap);
        if (newY > this.maxY) this.maxY = (newY + gap);
    }

    addEntry(newX: number, newY: number, newName: string, newNote: string) {
        this.currentIndex = this.locations.push(new MapLocation(newX, newY, newName, newNote)) - 1;
        this.extendBounds(newX, newY);
    }

    removeEntry(i: number) {
        this.locations.splice(i, 1);
    }

    renderMap(context: CanvasRenderingContext2D) {
        context.beginPath();
        context.rect(0, 0, context.canvas.width, context.canvas.height);
        context.fillStyle = '#000000';
        context.fill();
        context.lineWidth = 5;
        context.strokeStyle = '#bbbbbb';
        context.stroke();

        context.font = "12px Arial";
        context.textAlign = "center";
        context.textBaseline = "middle";

        for (var i = 0; i < this.locations.length; i++) {
            var x = ((this.locations[i].xloc - this.minX) * context.canvas.width) / (this.maxX - this.minX);
            var y = ((this.locations[i].yloc - this.minY) * context.canvas.height) / (this.maxY - this.minY);

            if (i == this.currentIndex) {
                context.strokeStyle = '#779900';
                context.fillStyle = "#eeee40";
            } else {
                context.strokeStyle = '#225500';
                context.fillStyle = "#44ee20";
            }

            context.beginPath();
            context.arc(x, y, 5, 0, 2 * Math.PI);
            context.stroke();
            context.beginPath();
            context.arc(x, y, 3, 0, 2 * Math.PI);
            context.fill();

            context.fillStyle = "#eeee40";
            context.fillText(this.locations[i].name, x, y + 20);
        }

        // now draw the compass
        context.strokeStyle = '#bbbbbb';
        context.lineWidth = 2;
        context.beginPath();
        context.moveTo(20, 40);
        context.lineTo(20, 20);
        context.lineTo(16, 24);
        context.moveTo(20, 20);
        context.lineTo(24, 24);
        context.moveTo(20, 40);
        context.lineTo(40, 40);
        context.lineTo(36, 36);
        context.moveTo(40, 40);
        context.lineTo(36, 44);
        context.stroke();
        context.fillStyle = "#dddddd";
        context.fillText("-Z", 20, 10);
        context.fillText("+X", 50, 40);
    }

    load(map: any) {
        if (map) {
            this.name = map.name;
            this.minX = map.minX;
            this.minY = map.minY;
            this.maxX = map.maxX;
            this.maxY = map.maxY;
            this.locations = [];
            for (var i = 0; i < map.locations.length; i++) {
                this.addEntry(map.locations[i].xloc, map.locations[i].yloc,
                    map.locations[i].name, map.locations[i].note);
            }
        }
        else {
            this.init();    // I don't think this can happen, but just in case...
        }
        this.currentIndex = 0;
    }

    shortName() {
        let mapName = this.name;
        if (mapName) {
            let hashName = "map." + mapName.replace(/\s/g, '');  // remove all whitespace
            return hashName;
        } else {
            return "map.unknown"; // I don't think this can happen, but just in case...
        }
    }

    save() {
        let hashName = this.shortName();
        let jsonStr = JSON.stringify(this);
        localStorage.setItem(hashName, jsonStr);
    }

    resize() {
        this.minX = 10000;
        this.minY = 10000;
        this.maxX = -10000;
        this.maxY = -10000;
        for (var i = 0; i < this.locations.length; i++) {
            this.extendBounds(this.locations[i].xloc, this.locations[i].yloc);
        }
    }

    zoomOut() {
        let x = this.locations[this.currentIndex].xloc;
        let y = this.locations[this.currentIndex].yloc;
        let w = (this.maxX - this.minX) * 2;
        let h = (this.maxY - this.minY) * 2;
        if ((w < 20000) && (h < 20000)) {
            this.minX = x - (w * 0.5);
            this.maxX = x + (w * 0.5);
            this.minY = y - (h * 0.5);
            this.maxY = y + (h * 0.5);
        }
    }

    zoomIn() {
        let x = this.locations[this.currentIndex].xloc;
        let y = this.locations[this.currentIndex].yloc;
        let w = (this.maxX - this.minX) * 0.5;
        let h = (this.maxY - this.minY) * 0.5;
        if ((w > 10) && (h > 10)) {
            this.minX = x - (w * 0.5);
            this.maxX = x + (w * 0.5);
            this.minY = y - (h * 0.5);
            this.maxY = y + (h * 0.5);
        }
    }

    setNote(note: string) {
        this.locations[this.currentIndex].note = note;
    }

    getLocForXY(x: number, y: number, w: number, h: number) {
        let theIndex = -1;
        this.locations.forEach(function (loc: MapLocation, i: number) {
            var lx = ((loc.xloc - this.minX) * w) / (this.maxX - this.minX);
            var ly = ((loc.yloc - this.minY) * h) / (this.maxY - this.minY);
            let r = 9; // radius of clickable location
            if ((x > (lx - r)) && (x < (lx + r)) &&
                (y > (ly - r)) && (y < (ly + r))) {
                theIndex = i;
            }
        }, this);
        return theIndex;
    }

    numLocations() {
        return this.locations.length;
    }

    location(i: number) {
        return this.locations[i];
    }
}

// ******************************
// ** CODE THAT BELONGS IN app.ts

declare function saveAs(blob: Blob, name: String, no_auto_bom?: boolean): void;

let theCanvas;
let theMap = new Map();
let theMapNames = [];

function renderMap() {
    var context = theCanvas.getContext('2d');
    theMap.renderMap(context);
}

function onHighlight(x) {
    theMap.currentIndex = x;
    renderMap();
    if (theMap.numLocations() > 0) {
        let name = <HTMLElement>document.getElementById('locationName');
        name.textContent = theMap.locations[theMap.currentIndex].name;
        let coords = <HTMLElement>document.getElementById('coordinates');
        coords.textContent = "Coordinates: (" + theMap.locations[theMap.currentIndex].xloc +
            ", " + theMap.locations[theMap.currentIndex].yloc + ")";
        let note = <HTMLTextAreaElement>document.getElementById('note');
        if (note) {
            if (theMap.locations[theMap.currentIndex].note) {
                note.value = theMap.locations[theMap.currentIndex].note;
            } else {
                note.value = "";
            }
        }
    }
}

function onRemove(x) {
    // row has cell has button, so need parent of parent
    var i = x.parentElement.parentElement.rowIndex;
    theMap.removeEntry(i-1);
    var table = <HTMLTableElement>document.getElementById('locationTable');
    table.deleteRow(i);
    renderMap();
    theMap.save();
}

function addMapEntryToTable(newX: number, newY: number, newName: string, newNote: string) {
    var table = <HTMLTableElement>document.getElementById('locationTable');
    var row = table.insertRow(-1);
    row.setAttribute("onclick", "onHighlight(this.rowIndex)");
    row.insertCell(-1).innerHTML = String(newX);
    row.insertCell(-1).innerHTML = String(newY);
    // use italics to indicate a location has a note
    if (newNote && (newNote.length > 0)) {
        row.insertCell(-1).innerHTML = "<i>" + newName + "</i>";
    } else {
        row.insertCell(-1).innerHTML = newName;
    }
    row.insertCell(-1).innerHTML = "<button onclick=\"onRemove(this)\">Remove</button>";
}

function addMapEntry() {
    var newX = Number(document.getElementById('newX')["value"]);
    var newY = Number(document.getElementById('newY')["value"]);
    var newName = document.getElementById('newName')["value"];
    var newNote = document.getElementById('note')["value"];
    document.getElementById('newX')["value"] = "";
    document.getElementById('newY')["value"] = "";
    document.getElementById('newName')["value"] = "";
    addMapEntryToTable(newX, newY, newName, newNote);
    theMap.addEntry(newX, newY, newName, newNote);
    renderMap();
    theMap.save();
}

function updateMapCombo(selectNewMap: boolean) {
    let mapNames = [];

    for (var i = 0, len = localStorage.length; i < len; i++) {
        var keyName = localStorage.key(i);
        if (keyName.indexOf("map.") >= 0) {
            let jsonObj = JSON.parse(localStorage.getItem(keyName));
            //var iStart = keyName.indexOf(".") + 1;
            mapNames.push(jsonObj.name);
        }
    }

    let mapSelector = <HTMLSelectElement>document.getElementById('mapPicker');
    let mapNameElement = <HTMLInputElement>document.getElementById('mapName');
    if (mapSelector && mapNameElement) {
        // remove all elements from combo box
        while (mapSelector.length > 0) {
            mapSelector.remove(0);
        }
        // add the map names
        mapNames.forEach(function (name: string) {
            if (name && name.length > 0) {
                let mapOption = document.createElement('option');
                mapOption.text = name;
                mapSelector.add(mapOption);
            }
        });

        if (selectNewMap) {
            // Select the current map in the combo box
            mapSelector.selectedIndex = mapNames.indexOf(theMap.name);
        }
    }
}

function resize() {
    theMap.resize();
    renderMap();
    theMap.save();
}

function zoomIn() {
    theMap.zoomIn();
    renderMap();
}

function zoomOut() {
    theMap.zoomOut();
    renderMap();
}

function rebuildLocationTable() {
    let table = <HTMLTableElement>document.getElementById('locationTable');
    while (table.rows.length > 0) {
        table.deleteRow(0);
    }
    for (var i = 0; i < theMap.numLocations(); i++) {
        let loc = theMap.location(i);
        addMapEntryToTable(loc.xloc, loc.yloc, loc.name, loc.note);
    }
    onHighlight(0);
}

function loadMap(saveFirst: boolean) {
    if (saveFirst) {
        // first save the current map before loading something else
        theMap.save();
    }

    let mapSelector = <HTMLSelectElement>document.getElementById('mapPicker');
    let mapOption = <HTMLOptionElement>mapSelector.options[mapSelector.selectedIndex];
    let longName = mapOption.value;
    let hashName = "map." + longName.replace(/\s/g, '');  // remove all whitespace
    let jsonStr = <string>localStorage.getItem(hashName);
    let map = JSON.parse(jsonStr);
    theMap.load(map);

    rebuildLocationTable();

    renderMap();
    onHighlight(theMap.currentIndex);
}

function importMap(map : any) {
    // first save the current map before loading something else
    theMap.save();

    theMap.load(map);
    theMap.save();
    updateMapCombo(true);

    rebuildLocationTable();

    renderMap();
    onHighlight(theMap.currentIndex);
}

function updateNote() {
    let note = <HTMLTextAreaElement>document.getElementById('note');
    theMap.setNote(note.value);
}

function newMap() {
    theMap.init();
    renderMap();
    let mapNameElement = <HTMLInputElement>document.getElementById('mapName');
    let mapName = mapNameElement.value;
    theMap.name = mapName;
    theMap.save();
    updateMapCombo(true);
    mapNameElement.value = "";
}

function onClick(event) {
    var x = event.pageX - theCanvas.offsetLeft;
    var y = event.pageY - theCanvas.offsetTop;
    var i = theMap.getLocForXY(x, y, theCanvas.width, theCanvas.height);
    if (i >= 0) {
        onHighlight(i);
    }
}

function float2int(value) {
    return value | 0;
}

function onMouseMove(event) {
    var w = theMap.maxX - theMap.minX;
    var h = theMap.maxY - theMap.minY;
    var x = float2int(theMap.minX + (event.pageX - theCanvas.offsetLeft) * w / (theCanvas.width));
    var y = float2int(theMap.minY + (event.pageY - theCanvas.offsetTop) * h / (theCanvas.height));
    let coords = <HTMLElement>document.getElementById('mousexy');

    coords.textContent = "(" + x + ", " + y + ")";
}

function onMouseWheel(event) {
    if (   (event.pageX > theCanvas.offsetLeft)
        && (event.pageX < (theCanvas.width + theCanvas.offsetLeft))
        && (event.pageY > theCanvas.offsetTop)
        && (event.pageY < (theCanvas.height + theCanvas.offsetTop))
    ) {
        if (event.wheelDelta > 0) {
            zoomIn();
            event.preventDefault();
        } else {
            zoomOut();
            event.preventDefault();
        }
    }
}

function deleteMap() {
    var okToDelete = confirm("Are you sure you want to delete " + theMap.name + "?");
    if (okToDelete) {
        let hashName = theMap.shortName();
        localStorage.removeItem(hashName);
        updateMapCombo(false);
        loadMap(false);
    }
}

function exportMap() {
    var blob = new Blob([JSON.stringify(theMap)], { type: "text/plain;charset=utf-8" });
    saveAs(blob, theMap.shortName() + ".json");
}

window.onload = () => {
    theCanvas = <HTMLCanvasElement>document.getElementById('mapCanvas');

    updateMapCombo(false);
    loadMap(false);

    theCanvas.addEventListener('click', onClick, false);
    theCanvas.addEventListener('mousemove', onMouseMove, false);
    theCanvas.addEventListener('wheel', onMouseWheel, false);

    var fileInput = <HTMLInputElement>document.getElementById('fileInput');
    fileInput.addEventListener('change', function (e) {
        let file = fileInput.files[0];
        let textType = /text.*/;
        var reader = new FileReader();
        reader.onload = function (e) {
            let map = JSON.parse(reader.result);
            importMap(map);
        }
        reader.readAsText(file);
        fileInput.value = null;
    });

    renderMap();
};