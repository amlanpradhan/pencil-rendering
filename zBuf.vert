varying vec3 normal, lightDirection;
varying vec4 position;
varying vec4 diffuse, ambient, globalAmbient;
attribute vec3 curvDir;

void main()
{
	normal = normalize(gl_NormalMatrix*gl_Normal);

	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	lightDirection = normalize(vec3(gl_LightSource[0].position));

	gl_Position = ftransform();
	position = gl_Position();
}