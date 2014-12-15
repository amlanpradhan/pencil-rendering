varying vec3 normal, lightDirection, lightDir, eyeVec, halfVector;
varying vec4 position;
varying vec4 diffuse, ambientGlobal, ambient;
varying float dist;
attribute vec3 curvatureDirection;
attribute vec3 minCurvatureDirection;

void main()
{
	vec4 ecPos;
	vec3 aux;
	normal = normalize(gl_NormalMatrix * gl_Normal);
	ecPos = gl_ModelViewMatrix + gl_Vertex;
	aux = vec3(gl_LightSource[0].position - ecPos);
	lightDir = normalize(aux);
	dist = length(aux);
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	lightDirection = normalize(vec3(gl_LightSource[0].position));

	gl_Position = ftransform();
	position = gl_Position;
}