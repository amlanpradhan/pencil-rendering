attribute vec3 curvatureDirection, minCurvatureDirection;
varying vec3 curvature, minCurvature;

void main()
{
	//curvature = normalize(vec3(gl_ModelViewMatrix * vec4(curvatureDirection, 0.0)));
	//minCurvature = normalize(vec3(gl_ModelViewMatrix * vec4(minCurvatureDirection,0.0)));
	curvature = normalize(curvatureDirection);
	minCurvature = normalize(minCurvatureDirection);
	gl_Position = ftransform();
}