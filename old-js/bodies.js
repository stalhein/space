import {mat4, vec3} from "https://cdn.jsdelivr.net/npm/gl-matrix@3.4.3/esm/index.js";
import { Shader } from "./shader.js";
import { Constants } from "./constants.js";

export class Bodies {
    constructor(gl) {
        this.gl = gl;

        this.vbo = null;
        this.vao = null;
        this.ebo = null;

        this.elementsCount = 0;

        this.nextBodyID = 0;
        this.data = new Map();
        this.physics = new Map();
        this.lighting = new Map();
    }

    load() {
        // Graphics
        const gl = this.gl;

        const RESOLUTION = 30;
        const vertices = [];
        const elements = [];

        for (let lat = 0; lat <= RESOLUTION; ++lat) {
            const alpha = lat * Math.PI / RESOLUTION;
            const sinAlpha = Math.sin(alpha);
            const cosAlpha = Math.cos(alpha);
            for (let lon = 0; lon <= RESOLUTION; ++lon) {
                const beta = lon * 2 * Math.PI / RESOLUTION;
                const sinBeta = Math.sin(beta);
                const cosBeta = Math.cos(beta);

                const x = sinAlpha * cosBeta;
                const y = cosAlpha;
                const z = sinAlpha * sinBeta;

                vertices.push(x, y, z);
            }
        }


        for (let lat = 0; lat < RESOLUTION; ++lat) {
            for (let lon = 0; lon < RESOLUTION; ++lon) {
                const one = (lat * (RESOLUTION + 1)) + lon;
                const two = one + RESOLUTION + 1;

                elements.push(one);
                elements.push(two);
                elements.push(one+1);

                elements.push(two);
                elements.push(two+1);
                elements.push(one+1);
            }
        }

        this.elementsCount = elements.length;
        
        this.vao = gl.createVertexArray();
        this.vbo = gl.createBuffer();
        this.ebo = gl.createBuffer();

        gl.bindVertexArray(this.vao);
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vbo);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.ebo);

        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(elements), gl.STATIC_DRAW);

        gl.enableVertexAttribArray(0);
        gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 0, 0);

        gl.bindVertexArray(null);

        // Sun
        this.addBody(0, 0, 0, 0, 0, 0, 696340000 * 2, 1.989e30, 1.0, 0.8, 0.2, 1.0);
        // Earth
        this.addBody(149.6e9, 0, 0, 0, 0, 29780, 6371000 * 2, 5.972e24, 0.2, 0.5, 0.9, 0.0);
        // Moon
        this.addBody(149.6e9 + 384400000, 0, 0, 0, 0, 29780 + 1022, 1737000*2, 7.35e22, 0.7, 0.7, 0.7, 0.1);
    }

    addBody(x, y, z, vx, vy, vz, size, mass, r, g, b, emmitance) {
        this.data.set(this.nextBodyID, {
            position: vec3.fromValues(x, y, z),
            velocity: vec3.fromValues(vx, vy, vz),
            size: size,
            mass: mass,
            colour: vec3.fromValues(r, g, b),
            emmitance: emmitance});
        this.nextBodyID++;
    }

    getAccelerations() {
        const G = 6.67430e-11;
        const bodies = Array.from(this.data.values());
        const accelerations = bodies.map(() => vec3.create());
        for (let i = 0; i < bodies.length; ++i) {
            for (let j = 0; j < bodies.length; ++j) {
                if (i == j) continue;

                const a = bodies[i];
                const b = bodies[j];

                const difference = vec3.create();
                vec3.sub(difference, b.position, a.position);

                const distanceSquared = Math.max(vec3.squaredLength(difference), 1e6);
                const distance = Math.sqrt(distanceSquared);

                const magnitude = (G * b.mass) / distanceSquared;

                vec3.scale(difference, difference, 1/distance);
                vec3.scale(difference, difference, magnitude);

                vec3.add(accelerations[i], accelerations[i], difference);
            }
        }
        return accelerations;
    }

    update(dt) {
        const bodies = Array.from(this.data.values());

        const a1 = this.getAccelerations();

        for (let i = 0; i < bodies.length; ++i) {
            const body = bodies[i];
            
            const temp = vec3.create();
            vec3.scaleAndAdd(body.position, body.position, body.velocity, dt);

            vec3.scale(temp, a1[i], 0.5 * dt * dt);
            vec3.add(body.position, body.position, temp);
        }

        const a2 = this.getAccelerations();

        for (let i = 0; i < bodies.length; ++i) {
            const body = bodies[i];
            
            const ac = vec3.create();
            vec3.add(ac, a1[i], a2[i]);
            vec3.scale(ac, ac, 0.5 * dt);

            vec3.add(body.velocity, body.velocity, ac);
        }
    }

    render(shader) {
        const gl = this.gl;

        shader.use();
        gl.bindVertexArray(this.vao);

        let model = mat4.create();
        const bodies = Array.from(this.data.values());
        for (let i = 0; i < bodies.length; ++i) {
            const body = bodies[i];
            mat4.identity(model);
            const renderPos = vec3.create();
            vec3.scale(renderPos, body.position, Constants.RENDER_CONSTANT);
            mat4.translate(model, model, renderPos);
            const s = body.size * Constants.RENDER_CONSTANT;
            mat4.scale(model, model, vec3.fromValues(s, s, s));

            shader.setMat4("uModel", model);

            shader.setVec3("uColour", body.colour);

            shader.setFloat("uEmmiter", body.emmitance);

            gl.drawElements(gl.TRIANGLES, this.elementsCount, gl.UNSIGNED_INT, 0);

            if (body.emmitance == 0.0) console.log(body.position[0]);
        }

        gl.bindVertexArray(null);
    }
}