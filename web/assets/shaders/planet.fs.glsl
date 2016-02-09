precision highp float;

varying vec2 uv;

uniform vec4 color;
uniform sampler2D texture;

void main() {
	gl_FragColor = texture2D(texture, uv) * color;
}