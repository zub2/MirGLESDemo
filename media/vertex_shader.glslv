#version 100
attribute vec3 vPosition;
attribute vec3 vColor;
uniform mat4 MVPMatrix;
varying vec4 fragColor;

void main()
{
    gl_Position = MVPMatrix * vec4(vPosition, 1);
	fragColor = vec4(vColor, 1);
}
