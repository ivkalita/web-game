precision highp float;

attribute vec2 in_Position;

varying vec2 uv;

uniform float radius;
uniform vec2 offset;
uniform mat4 viewProjection;

void main() {
	uv = in_Position;
	gl_Position = viewProjection * vec4((in_Position - 0.5) * radius * 2.0 + offset, 0, 1);
}