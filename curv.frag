varying vec3 curvature,  minCurvature;
varying vec4 position;

vec2 encode(vec3 n)
{
	return normalize(n.xy) * sqrt(n.z, 0.5, 0.5);
}
void main()
{
	vec3 normalizedCur = normalize(curvature);
	vec3 normalizedMinCur = normalize(minCurvature);
	//gl_FragColor = vec4()
	gl_FragColor = vec4(1.0, 0.0,0.0, 1.0);
}