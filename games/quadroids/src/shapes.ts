// shapes

var letters =
[
    [[-7,7],[2,-5],[4,7],[3,1],[0,1]], // A
    [[-7,7],[-3,-5],[2,-5],[2,1],[-3,1],[4,1],[4,5],[2,7],[-7,7]],    // B
    [[4,7],[-7,7],[-3,-5],[4,-5]],  // C
    [[1,7],[-7,7],[-3,-5],[3,-5],[3,5],[1,7]],  // D
    [[4,7],[-7,7],[-5,1],[1,1],[-5,1],[-3,-5],[4,-5]],  // E
    [[-7,7],[-5,1],[1,1],[-5,1],[-3,-5],[4,-5]],  // F
    [[2,-5],[-3,-5],[-7,7],[1,7],[3,5],[4,1],[-1,1]],    // G
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // H
    [[-4,7],[0,-5]],    // I
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // J
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // K
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // L
    [[-7,7],[-3,-5],[0,0],[4,-5],[4,7]],    // M
    [[-7,7],[-3,-5],[1,7],[4,-5]],    // N
    [[-7,7],[-4,-2],[-2,-5],[4,-5],[1,5],[-1,7],[-7,7]],    // O
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // P
    [[-7,7],[-4,-2],[-2,-5],[4,-5],[1,5],[0,6],[-1,5],[1,7],[0,6],[-1,7],[-7,7]],    // Q
    [[-7,7],[-3,-5],[4,-5],[3,1],[-5,1],[0,1],[3,7]],    // R
    [[4,-4],[3,-5],[-1,-5],[-2,-4],[-3.5,0],[-3,1],[1,1],[2,2],[0,7],[-5,7],[-6,6]],    // S
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // T
    [[-4,-5],[-7,7],[1,7],[4,-5]],    // U
    [[-7,-5],[-4,7],[3,-5]],    // V
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // W
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // X
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // Y
    [[2,2],[-2,-2],[0,0],[2,-2],[-2,2]],    // Z
];

var P_Ship = [[-10,10], [0,-10], [10,10], [0,7], [-10,10]];
var P_Bullet = [[-2,-2],[-2,3],[3,3],[3,-2],[-2,-2]];

function drawPoly(context : CanvasRenderingContext2D,
                  path : any,   // array of 2D points
                  x : number,
                  y : number,
                  scale : number,
                  rot : number)
{
    context.save();
    context.beginPath();
    context.translate(x, y);
    context.scale(scale, scale);
    context.rotate(rot);
    for (var i=0; i<path.length; i++)
    {
        if (i == 0) {
            context.moveTo(path[i][0], path[i][1]);
        } else {
            context.lineTo(path[i][0], path[i][1]);
        }
    }
    context.stroke();
    context.restore();
}

function drawVectorPath(context : CanvasRenderingContext2D,
                  path : Vector[],   // array of 2D points
                  x : number,
                  y : number,
                  scale : number,
                  rot : number)
{
    context.save();
    context.beginPath();
    context.translate(x, y);
    context.scale(scale, scale);
    context.rotate(rot);
    for (var i=0; i<path.length; i++)
    {
        if (i == 0) {
            context.moveTo(path[i].x, path[i].y);
        } else {
            context.lineTo(path[i].x, path[i].y);
        }
    }
    context.stroke();
    context.restore();
}

function drawLetter(context : CanvasRenderingContext2D,
                  letter : number,  // index into letter array
                  x : number,
                  y : number,
                  scale : number)
{
    if (letter >= letters.length)
    {
        console.log("error: don't know how to draw that letter.");
        return;
    }

    drawPoly(context, letters[letter], x, y, scale, 0);
}

function drawPolyText(context : CanvasRenderingContext2D,
                      text : string,
                      x : number,
                      y : number,
                      scale : number)
{
    // compute offset to center the text
    var width = (text.length - 0.5) * 13 * scale;
    var xoff = -(width / 2);

    for (var i=0; i<text.length; i++) {
        if (text[i] != " ") {
            var index: number = text.charCodeAt(i) - 65;
            drawLetter(context, index, x + xoff, y, scale);
        }
        x += 13 * scale;
    }
}
