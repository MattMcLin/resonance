// objects

///<reference path="shapes.ts"/>

class Vector
{
    constructor(public x : number, 
                public y : number) {
    }
    
    static times(k : number, v : Vector) { return new Vector(k * v.x, k * v.y); }
    static plus(v1 : Vector, v2 : Vector) { return new Vector(v1.x + v2.x, v1.y + v2.y); }
    
    rotate(theta: number) {
        var newx = (-this.y)*Math.sin(theta) +
                (this.x)*Math.cos(theta);
        var newy = (this.x)*Math.sin(theta) -
                (-this.y)*Math.cos(theta);
        this.x = newx;
        this.y = newy;
    }
};


class Rock {
    pos : Vector = new Vector(0,0);
    acc : Vector = new Vector(0,0);
    rot : number = 0;
    spin : number = 0;
    
    alive : boolean = true;
    dying = 0;  // if > 0, indicates opacity, which fades out to nothing
    size : number = 1;    // 1 = big, 2 = medium, 3 = small
    
    shape : Vector[] = [];
    
    constructor(public MAXX : number, public MAXY : number, public BORDER : number) {
        this.pos.x = Math.random() * this.MAXX;
        this.pos.y = Math.random() * this.MAXY;
        this.acc.x = Math.random() * 6 - 3;
        this.acc.y = Math.random() * 6 - 3;
        this.spin = Math.random() * 0.2 - 0.1;
        
        this.shape = [];
        var i = 0;
        for (var rad = 0; rad <= (2*Math.PI+.01); rad += (Math.PI/8)) {
            var r = 4 + Math.random() * 2;  // hope for average radius of 5
            this.shape[i] = new Vector(r,0);
            this.shape[i].rotate(rad);
            i++;
        }
        // make last point equal to first point to make closed loop
        this.shape[this.shape.length-1] = this.shape[0];
    }
    
    update() {
        this.pos.x += this.acc.x;
        this.pos.y += this.acc.y;
        this.rot += this.spin;
        
        if (this.pos.x > MAXX) this.pos.x = -this.BORDER;
        if (this.pos.y > MAXY) this.pos.y = -this.BORDER;
        if (this.pos.x < (-this.BORDER)) this.pos.x = this.MAXX;
        if (this.pos.y < (-this.BORDER)) this.pos.y = this.MAXY;
        
        if (this.dying > 0)
            this.dying = this.dying - 0.1;
    }
    
    draw(context : CanvasRenderingContext2D) {
        var scale = 1;
        if (this.dying > 0)
        {
            context.globalAlpha = this.dying;
            scale += (1.0 - this.dying);
        }
        context.strokeStyle = '#cccccc';
        context.lineWidth = 0.5;
        drawVectorPath(context, this.shape, this.pos.x, this.pos.y, (8 - this.size*2), this.rot);
        context.globalAlpha = 1.0;
    }
    
    crush() {
        this.alive = false;
        this.dying = 1.0;
    }
};

class Bullet {
    pos : Vector = new Vector(0,0);
    acc : Vector = new Vector(0,0);
    alive : boolean = true;
    lifetime = 60;
};

class Ship {
    pos : Vector = new Vector(0,0);
    acc : Vector = new Vector(0,0);
    radius = 6; // size used for collision detection
    theta = 0;
    shootdelay = 0;
    alive : boolean = true;
    bullets : Bullet[] = [];
    dying = 0;  // if > 0, indicates opacity, which fades out to nothing
    shields = 3;
    shieldtimeleft = 0;
    
    constructor(MAXX : number, MAXY : number, newx : number, newy : number) {        
        this.pos.x = newx;
        this.pos.y = newy;
    }
    
    update() {
        this.shootdelay--;
        if (this.shieldtimeleft > 0)
        {
            this.shieldtimeleft--;
        }
        this.acc.x *= 0.999;
        this.acc.y *= 0.999;
        this.pos.x += this.acc.x;
        this.pos.y += this.acc.y;
        if (this.pos.x > MAXX) this.pos.x = 0;
        if (this.pos.x < 0) this.pos.x = MAXX;
        if (this.pos.y > MAXY) this.pos.y = 0;
        if (this.pos.y < 0) this.pos.y = MAXY;
        if (this.dying > 0)
            this.dying = this.dying - 0.1;
        
        // update the bullets as well
        for (var i=0; i<this.bullets.length; i++) {
            if (this.bullets[i].alive) {
                this.bullets[i].lifetime--;
                this.bullets[i].pos.x += this.bullets[i].acc.x;
                this.bullets[i].pos.y += this.bullets[i].acc.y;
                if ((this.bullets[i].pos.x > MAXX)
                    || (this.bullets[i].pos.x < 0) 
                    || (this.bullets[i].pos.y > MAXY)
                    || (this.bullets[i].pos.y < 0)
                    || (this.bullets[i].lifetime <= 0)) 
                { 
                    this.bullets[i].alive = false; 
                    // TODO: if bullet goes off screen, subtract point for bad accuracy
                }
            }
        }
    }
    
    draw(context : CanvasRenderingContext2D) {
        context.strokeStyle = '#ffff00';
        if (this.dying > 0)
        {
            context.globalAlpha = this.dying;
            context.strokeStyle = '#ff2000';
        }
        
        context.lineWidth = 2;
        drawPoly(context, P_Ship, this.pos.x, this.pos.y, 1.0, this.theta);
        
        context.strokeStyle = '#7f7f7f';
        context.lineWidth = 1;
        for (var i=0; i<this.bullets.length; i++) {
            if (this.bullets[i].alive) {
                context.globalAlpha = (this.bullets[i].lifetime < 60) ? (this.bullets[i].lifetime/60) : 1.0;
                drawPoly(context, P_Bullet, this.bullets[i].pos.x, this.bullets[i].pos.y, 1.0, 0.0);
            }
        }
        context.globalAlpha = 1.0;
        
        if (this.shieldtimeleft > 0)
        {
            context.beginPath();
            // TODO: draw circle filled with light blue color & alpha-channel gradient
            var shieldradius = 18;
            var gradient = context.createRadialGradient(this.pos.x, this.pos.y, this.radius, this.pos.x, this.pos.y, this.radius + shieldradius);
            gradient.addColorStop(0, 'rgba(0,40,255,0.4)');
            gradient.addColorStop(1, 'rgba(0,40,255,0)');
            context.fillStyle = gradient;
            context.arc(this.pos.x, this.pos.y, this.radius + shieldradius, 0, 2*Math.PI, false);
            context.fill();
            //context.arc(this.pos.x, this.pos.y, this.radius + 3, 0, 2*Math.PI);
            //context.stroke();
        }
    }
    
    crash() {
        if (this.shieldtimeleft <= 0) {
            this.dying = 1.0;
            this.alive = false;
        }
    }
    
    turnLeft() {
        this.theta -= 0.1;
    }
    
    turnRight() {
        this.theta += 0.1;
    }
    
    accelerate() {
        var accVec = new Vector(0, -1);
        accVec.rotate(this.theta);
        this.acc.x += accVec.x*0.15;
        this.acc.y += accVec.y*0.15;
        if (this.acc.x > 5.0) this.acc.x = 5.0;
        if (this.acc.x < -5.0) this.acc.x = -5.0;
        if (this.acc.y > 5.0) this.acc.y = 5.0;
        if (this.acc.y < -5.0) this.acc.y = -5.0;
    }
    
    slowdown() {
        this.acc.x *= 0.95;
        this.acc.y *= 0.95;
    }
    
    shoot() {
        if ((this.shootdelay > 1) || (!this.alive)) {
            return;
        }
        
        // find an unused bullet in the list, or add a new one
        var i;
        for (i=0; i<this.bullets.length; i++) {
            if (!this.bullets[i].alive) {
                break;
            }
        }
        this.bullets[i] = new Bullet;
        var bulletVec = new Vector(0, -1);
        bulletVec.rotate(this.theta);
        this.bullets[i].acc = Vector.plus(this.acc, Vector.times(10, bulletVec));
        this.bullets[i].pos = Vector.plus(this.pos, Vector.times(10, bulletVec));
        this.shootdelay = 15;
    }
    
    activateShield() {
        if ((this.shieldtimeleft <= 0) && (this.shields > 0)) {
            this.shieldtimeleft = 180;
            this.shields--;
        }
    }
};
