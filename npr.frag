uniform sampler2D texSrc;
uniform sampler2D texCur;
uniform sampler3D texPencil;
uniform sampler2D tempTex;

vec3 decode(vec2 n)
{
	vec3 normal;
	normal.z = dot(n, n) * 2.0 - 1.0;
	normal.xy = normalize(n) * sqrt(1.0 - normal.z * normal.z);
	return normal;
}

vec2 rotateDirections(vec2 dir, vec2 uv)					
{
	return vec2(dir.x * uv.x - dir.y * uv.y, dir.x * uv.y + dir.y * uv.x);
}

void main()
{
	
	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 srcColor = texture2D(texSrc, gl_TexCoord[0].st);

	vec4 curvature = texture2D(texCur, gl_TexCoord[0].st);
	vec3 curv = decode(curvature.xy);
	vec3 zDir = vec3(0.0,0.0,1.0);
	vec3 strokeDir = vec3(1.0,0.0,0.0);

	vec3 direction = curv - dot(curv, zDir)*zDir;
	float rotAngle = acos(dot(direction, strokeDir)/length(direction));

	

	//color += texture2D(tempTex, 5.0 * rotateDirections(vec2(cos(rotAngle), sin(rotAngle)), gl_TexCoord[0].st));
	color += texture2D(tempTex, 5.0 * rotateDirections(vec2(cos(0.2), sin(0.2)), gl_TexCoord[0].st));
	color += texture2D(tempTex, 5.0 * rotateDirections(vec2(cos(-2.6), sin(-2.6)), gl_TexCoord[0].st));
	color *= 0.5;
	color *= color;	
	if(srcColor.r == 1.0)
		gl_FragColor = color;
	
}