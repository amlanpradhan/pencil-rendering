varying vec2 myTexCoord;
uniform sampler2D myTexture;

varying vec3 fragmentNormal;

void main()
{
	const vec3 ambientColor = vec4(0.1, 0.1, 0.1);
    const vec3 diffuseColor = vec4(0.9, 0, 0);
    vec3 normal = normalize(fragmentNormal);
	//gl_FragColor = texture2D(myTexture, myTexCoord);
    vec4 sample = vec4(1.0, 1.0, 1.0, 1.0);
	gl_FragColor = vec4(clamp(sample.rgb * (diffuseColor + ambientColor), 0.0, 1.0), sample.a);
}