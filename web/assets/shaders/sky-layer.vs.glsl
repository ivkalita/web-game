precision highp float;

attribute vec2 in_Position;

varying vec2 position;

uniform mat4 viewProjection;

void main() {
	position = in_Position;
	gl_Position = viewProjection * vec4(in_Position, 0, 1);
}