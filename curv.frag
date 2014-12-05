varying vec3 maxCurvDir,  minCurvDir;
varying vec4 position;

vec2 encode(vec3 n)
{
	return normalize(n.xy) * sqrt(n.z, 0.5, 0.5);
}

void main()
{
	vec3 normMaxCurvDir = normalize(maxCurvDir);
	vec3 normMinCurvDir = normalize(minCurvDir);
	gl_FragColor = vec4(encode(normMaxCurvDir), encode(normMinCurvDir));
}