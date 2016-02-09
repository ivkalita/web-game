precision highp float;

varying vec2 position;

uniform float borderSize;
uniform float offset;
uniform vec2 mapSize;
uniform vec2 textureSize;
uniform sampler2D texture;

void main() {
	vec2 uv = fract((position + vec2(offset, 0)) / textureSize);
	vec4 color = texture2D(texture, uv);
	
	color.rgb *= 1.0 - length(min(position / borderSize, vec2(0.0, 0.0)));
	color.rgb *= 1.0 - length(min((mapSize - position) / borderSize, 0.0));
	
	gl_FragColor = color;
}