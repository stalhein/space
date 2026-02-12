import {mat4, vec3} from "https://cdn.jsdelivr.net/npm/gl-matrix@3.4.3/esm/index.js";
import {Camera} from "./camera.js";
import {Bodies} from "./bodies.js";
import {Constants} from "./constants.js";
import {Shader} from "./shader.js";

const canvas = document.querySelector("canvas");
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

const gl = canvas.getContext("webgl2");
if (!gl) {
    console.warn("WebGL2 not supported.");
}

const shader = new Shader(gl, "shaders/vertex.glsl", "shaders/fragment.glsl");
const camera = new Camera();
const bodies = new Bodies(gl);

let lastTime = 0;

async function init() {
    await shader.load();
    await bodies.load();

    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.enable(gl.DEPTH_TEST);
    //gl.enable(gl.CULL_FACE);
    //gl.cullFace(gl.FRONT);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    lastTime = performance.now();

    requestAnimationFrame(render);
}

function render(time) {
    const dt = (time - lastTime);
    lastTime = time;

    camera.update(dt);

    bodies.update(dt * Constants.TIME_SCALE);

    gl.clearColor(0.5, 0.65, 0.8, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    const projection = mat4.create();
    mat4.perspective(
        projection,
        Math.PI / 3,
        canvas.width / canvas.height,
        0.1,
        50000.0
    );

    shader.use();
    shader.setMat4("uView", camera.getViewMatrix());
    shader.setMat4("uProjection", projection);

    bodies.render(shader);

    requestAnimationFrame(render);
}

window.addEventListener("resize", () => {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
});

await init();

document.addEventListener("click", () => {
    document.body.requestPointerLock();
});