varying vec4 position;
attribute vec3 maxCurvDir, minCurvDir;
varying vec3 maxCurv, minCurv;

void main()
{
	maxCurv = normalize(vec3(gl_ModelViewMatrix * vec4(maxCurvDir, 0.0)));
	minCurv = normalize(vec3(gl_ModelViewMatrix * vec4(minCurvDir, 0.0)));
	gl_Position = ftransform();
	position = gl_Position;
}