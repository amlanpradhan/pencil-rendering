varying vec4 position;
attribute vec3 curvatureDir;
varying vec3 curvature;

void main()
{
	gl_Position = ftransform();
	position = gl_Position;
}