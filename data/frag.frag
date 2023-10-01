#version 140

uniform mediump float time;
uniform sampler2D tex;
in highp vec2 vTextureCoord;
in mediump vec4 vColour;
out vec4 fragColor;

void main() {
    vec2 ripple = vec2(sin(time * 0.1 + vTextureCoord.y) * 0.3, sin(time * 0.03 + vTextureCoord.x) * 0.5);
    fragColor = vec4(vTextureCoord.x, vTextureCoord.y, 0.0, 1.0);//texture(tex, vTextureCoord + ripple) * vColour;
}