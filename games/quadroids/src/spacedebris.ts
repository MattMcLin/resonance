// quadroids

///<reference path="key.ts"/>
///<reference path="objects.ts"/>
///<reference path="shapes.ts"/>

enum GameState { Title, Playing, GameOver };
var gameState: GameState = GameState.Title;

var MAXX: number;
var MAXY: number;

var ticks = 0;
var level = 1;
var score = 0;

var ship: Ship[] = [];
var numShips = 1;

var rocks: Rock[] = [];

var _canvas;
var _context;
var _backgroundImage;
var _soundRockExplode;
var _soundShipExplode;
var _soundZap;

function clearScreen(context: CanvasRenderingContext2D) {
    context.beginPath();
    context.rect(0, 0, _canvas.width, _canvas.height);
    /*
    if (_backgroundImage) {
        var pattern = context.createPattern(_backgroundImage, 'repeat');
        context.fillStyle = pattern;
    } else {
        context.fillStyle = '#000000';
    }
    */
    context.fillStyle = '#000000';
    context.fill();
    context.lineWidth = 3;
    context.strokeStyle = '#bbbbbb';
    context.stroke();

    context.globalAlpha = 0.3;
    if (_backgroundImage) {
        context.drawImage(_backgroundImage, 0, 0);
    }
    context.globalAlpha = 1.0;
}

function drawTitleScreen(context: CanvasRenderingContext2D) {
    context.strokeStyle = '#00ff00';
    context.lineWidth = 1;
    context.lineCap = 'round';
    drawPolyText(context, "QUADROIDS", MAXX / 2, MAXY / 3, 5.0);
    context.fillStyle = '#00cc33';
    context.textAlign = 'center';
    context.font = 'italic 20pt Calibri';
    context.fillText('Can you survive?', MAXX / 2, MAXY / 2);
    //drawPolyText(context, "ABCDEFGHIJKLM", MAXX/2, MAXY/2, 3.0);
    //drawPolyText(context, "NOPQRSTUVWXYZ", MAXX/2, 2*MAXY/3, 3.0);
}

function drawGameOverScreen(context: CanvasRenderingContext2D) {
    context.strokeStyle = '#00ff00';
    context.lineWidth = 1;
    context.lineCap = 'round';
    drawPolyText(context, "GAME OVER", MAXX / 2, MAXY / 3, 5.0);
}

function updateScreen(context: CanvasRenderingContext2D) {
    for (var i = 0; i < numShips; i++) {
        if (ship[i].alive || (ship[i].dying > 0)) {
            ship[i].draw(context);
        }
    }
    for (var i = 0; i < rocks.length; i++) {
        if (rocks[i].alive || (rocks[i].dying > 0)) {
            rocks[i].draw(context);
        }
    }
}

function makeAsteroids() {
    rocks = [];
    var numRocks = level + 1;
    for (var i = 0; i < numRocks; i++) {
        rocks[i] = new Rock(MAXX, MAXY, 50);
        
        // make sure the rock isn't on top of the ship
        if ((rocks[i].pos.x > (ship[0].pos.x - 10)) && (rocks[i].pos.x < (ship[0].pos.x + 10)))
            rocks[i].pos.x += 60;
        if ((rocks[i].pos.y > (ship[0].pos.y - 10)) && (rocks[i].pos.y < (ship[0].pos.y + 10)))
            rocks[i].pos.y += 60;
        if (rocks[i].acc.x == 0.0) rocks[i].acc.x = 0.4;
        if (rocks[i].acc.y == 0.0) rocks[i].acc.y = 0.4;
    }
}

function moveRocks() {
    for (var i = 0; i < rocks.length; i++) {
        if (rocks[i].alive || (rocks[i].dying > 0)) {
            rocks[i].update();
        }
    }
}

function breakupRock(rock: Rock, acc) {
    _soundRockExplode.currentTime = 0;
    _soundRockExplode.play();
    rock.crush();
    if (rock.size < 3) {
        // spawn two smaller rocks
        for (var i = 0; i < 2; i++) {
            var rockindex = rocks.length;
            rocks[rockindex] = new Rock(MAXX, MAXY, 50);
            rocks[rockindex].size = rock.size + 1;
            rocks[rockindex].acc.x = rock.acc.x + acc.x * 0.1 + Math.random() * 2 - 1;
            rocks[rockindex].acc.y = rock.acc.y + acc.y * 0.1 + Math.random() * 2 - 1;
            rocks[rockindex].pos.x = rock.pos.x + Math.random() * 20 - 10;
            rocks[rockindex].pos.y = rock.pos.y + Math.random() * 20 - 10;
        }
    }
}

function checkCollision(ship: Ship) {
    var hitShip = 0;
    var radius = 0;

    // check for collisions with each asteroid,
    // assuming they are spheres
    var numRocks = rocks.length;    // only iterate over existing rocks, not any new ones that may result of a crash
    for (var i = 0; i < numRocks; i++) {
        if (rocks[i].alive) {
            switch (rocks[i].size) {
                case 1: radius = 900; break;
                case 2: radius = 400; break;
                case 3: radius = 100; break;
            }

            var dx = ship.pos.x - rocks[i].pos.x;
            var dy = ship.pos.y - rocks[i].pos.y;
            //note: we skip the square root since radius is squared above
            var dist = dx * dx + dy * dy;

            if (dist < (radius + ship.radius)) {
                // collision
                breakupRock(rocks[i], ship.acc);
                hitShip = 1;
            }
            
            // now check for collisions with bullets from this ship
            for (var b = 0; b < ship.bullets.length; b++) {
                if (ship.bullets[b].alive) {
                    dx = ship.bullets[b].pos.x - rocks[i].pos.x;
                    dy = ship.bullets[b].pos.y - rocks[i].pos.y;
                    dist = dx * dx + dy * dy;
                    if (dist < radius) {
                        breakupRock(rocks[i], ship.bullets[b].acc);
                        ship.bullets[b].alive = false;
                        score += 50 * rocks[i].size * numShipsAlive();
                    }
                }
            }
        }
    }

    return hitShip;
}

function numShipsAlive() {
    var numAlive = 0;
    for (var i = 0; i < numShips; i++) {
        if (ship[i].alive) {
            numAlive++;
        }
    }
    return numAlive;
}

function updateGame() {
    for (var i = 0; i < numShips; i++) {
        if (ship[i].alive) {
            if (Key.isDown(Key.LEFT)) {
                ship[i].turnLeft();
            }

            if (Key.isDown(Key.RIGHT)) {
                ship[i].turnRight();
            }

            if (Key.isDown(Key.SPACE)) {
                _soundZap.currentTime = 0;
                _soundZap.play();
                ship[i].shoot();
            }

            if (Key.isDown(Key.UP)) {
                ship[i].accelerate();
            }

            if (Key.isDown(Key.DOWN)) {
                ship[i].slowdown();
                ship[i].activateShield();
            }
        }
    }

    for (var i = 0; i < numShips; i++) {
        if (ship[i].alive || (ship[i].dying > 0)) {
            ship[i].update();
        }
    }

    moveRocks();

    if (gameState == GameState.Playing) {
        var won_level = true;
        for (var i = 0; i < rocks.length; i++) {
            if (rocks[i].alive) {
                won_level = false;
            }
        }
        if (won_level) {
            if (level < 20) {
                level++;
                newLevel();
            }
        }

        for (var i = 0; i < numShips; i++) {
            if (ship[i].alive && checkCollision(ship[i])) {
                _soundShipExplode.currentTime = 0;
                _soundShipExplode.play();
                ship[i].crash();
            }
        }

        if (numShipsAlive() == 0) {
            gameState = GameState.GameOver;
            ticks = 0;
        }
    }
}

function loop() {
    clearScreen(_context);
    document.getElementById("score").innerHTML = "Score: " + score;
    document.getElementById("level").innerHTML = "Level: " + level;
    if (ship.length > 0) {
        document.getElementById("shields").innerHTML = "Shields: " + ship[0].shields;
    }

    switch (gameState) {
        case GameState.Title:
            drawTitleScreen(_context);
            if (ticks > 100) {
                gameState = GameState.Playing;
                newLevel();
            }
            break;
        case GameState.Playing:
            updateGame();
            updateScreen(_context);
            break;
        case GameState.GameOver:
            updateGame();
            updateScreen(_context);
            drawGameOverScreen(_context);
            if (ticks > 180) {
                gameState = GameState.Title;
                ticks = 0;
            }
            break;
    }

    ticks++;
}

function newLevel() {
    score = 0;
    numShips = level;
    if (numShips > 4)
        numShips = 4;
    delete ship[0];
    delete ship[1];
    delete ship[2];
    delete ship[3];
    ship[0] = new Ship(MAXX, MAXY, MAXX / 4, MAXY / 4); ship[0].theta = 2 * Math.PI / 3;
    ship[1] = new Ship(MAXX, MAXY, 3 * MAXX / 4, MAXY / 4); ship[1].theta = -2 * Math.PI / 3;
    ship[2] = new Ship(MAXX, MAXY, MAXX / 4, 3 * MAXY / 4); ship[2].theta = Math.PI / 3;
    ship[3] = new Ship(MAXX, MAXY, 3 * MAXX / 4, 3 * MAXY / 4); ship[3].theta = -Math.PI / 3;

    makeAsteroids();
}

function init() {
    _canvas = <HTMLCanvasElement>document.getElementById('canvas');
    _context = _canvas.getContext('2d');
    MAXX = _canvas.width;
    MAXY = _canvas.height;

    _soundRockExplode = document.getElementById('rockExplode');
    _soundShipExplode = document.getElementById('shipExplode');
    _soundZap = document.getElementById('zap');

    _backgroundImage = new Image();
    _backgroundImage.src = 'resources/stars.jpg';

    window.addEventListener('keyup', function (event) { Key.onKeyup(event); }, false);
    window.addEventListener('keydown', function (event) { Key.onKeydown(event); }, false);

    setInterval(loop, 1000 / 60);
}