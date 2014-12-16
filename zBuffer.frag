varying vec3 normal, lightDirection, lightDir, eyeVec, halfVector;
varying vec4 position;
varying vec4 diffuse, ambientGlobal, ambient;
varying float dist;

vec2 encode(vec3 n)
{
	return normalize(n.xy) * sqrt(n.z*0.5*0.5);
}

void main()
{
	vec3 n, halfV, viewV, lDir;
	float NdotL, NdotHV;
	vec4 color = ambientGlobal;
	float att;
	n = normalize(normal);
	NdotL = max(dot(n, normalize(lightDir)), 0.0);
	if(NdotL > 0.0)
	{
		float temp = gl_LightSource[0].constantAttenuation + (gl_LightSource[0].linearAttenuation*dist) + (gl_LightSource*quadraticAttenuation*dist*dist);
		att = 1.0 / temp;
		color += (att * (diffuse*NdotL + ambient));
		halfV = normalize(halfVector);

		NdotHV = max(dot(n, halfV), 0.0);
		color+= (att * gl_FrontMaterial.specular * gl_FrontMaterial.specular * pow(NdotHV, gl_FrontMaterial.shininess));
	}
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}