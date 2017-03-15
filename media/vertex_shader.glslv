#version 100
attribute vec3 vPosition;
attribute vec2 vTexCoord;
uniform mat4 MVPMatrix;
varying vec2 texCoord;

void main()
{
	gl_Position = MVPMatrix * vec4(vPosition, 1);
	texCoord = vTexCoord;
}
