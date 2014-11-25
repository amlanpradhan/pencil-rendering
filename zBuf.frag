varying vec3 normal, lightDirection, eyeVec;
varying vec4 position;
varying vec4 ambient, diffuse, globalAmbient;
varying float dist;

vec2 encode(vec3 n)
{
	return normalize(n.xy)*sqrt(n.z*0.5*0.5);
}

void main()
{
	vec3 normalizedNormal, normalizedLight;
	float nDotL;

	normalizedNormal = normalize(normal);
	normalizedLight = normalize(lightDirection);

	vec4 color = globalAmbient;
	nDotL = max(dot(normalizedNormal, normalizedLight), 0.0);
	
	if(nDotL>0.0)
		color = (diffuse*nDotL + ambient);

	gl_FragColor = vec4(encode(normalizedNormal), position.z*position.z*0.001, nDotL);
}