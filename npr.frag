uniform sampler2D srcTexture:
uniform sampler2D curvTexture;
uniform sampler3D pencilTexture;
uniform vec2 offset[9];

vec3 decode(vec2 n)
{
	vec3 normal;
	normal.z = (dot(n,n) * 2.0) - 1.0;
	normal.xy = normalize(n) * sqrt(1 - (normal.z * normal.z));
}

vec2 rotateDir(vec2 orgnVec, vec2 dir)
{
	return vec2(((orgnVec.x*dir.x)-(orgnVec.y*dir.y)),
				((orgnVec.x*dir.y)+(orgnVec.y*dir.x)));
}

vec3 reflectedDir(vec3 origDir, vec3 normal)
{
	return (origDir - (dot(origDir ,normal)*normal));
}

void main()
{
	float sample[9];
	vec4 curvature = texture2D(curvTexture, gl_TexCoord[0].st);
	
	vec3 zDir = vec3(0.0, 0.0, 1.0);
	vec3 xDir = vec3(1.0, 0.0, 0.0);

	vec3 curvDir2D = decode(curvature.xy);
	vec3 curvDir = reflectedRay(curvDir2D , zDir);
	float theta = acos(dot(curvDir, xDir) / length(curvDir));

	//curvDir2D = decode(curvature.zw);
	//curvDir = reflectedRay(curvDir2D , zDir);
	//float theta2 = acos(dot(curvDir, xDir) / length(curvDir));

	vec4 srcTexNormal = texture2D(srcTexture, gl_TexCoord[0].st);
	vec3 normal = normalize(decode(stcTexNormal.rg));

	for(int i=0; i<9; i++)
	{
		vec4 temp1 = texture2D(srcTexture, gl_TexCoord[0].st+offset[i]);
		vec3 n = normalize(decode(temp1.rg));
		sample[i] = temp1.b;
		sum += dot(normal, n);
	}

	sum /= 9;

	float horizontalEdge = (sample[2] + (2.0*sample[5]) + sample[8]) - (sample[0] + (2.0*sample[3]) + sample[6]);

	float verticalEdge = (sample[0] + (2.0*sample[1]) + sample[2]) - (sample[6] + (2.0*sample[7]) + sample[8]);

	vec4 zBuf = texture2D(srcTexture, gl_TexCoord[0].st);
	vec4 tempTexObj = texture2D(srcTexture, gl_TexCoord[0].st);
	vec3 tempNormal = normalize(decode(tempTexObj.rg));

	float slice = zBuf.a;
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

	//vec3 tempRot = rotateDirections(vec2(cos(theta), sin(theta)), gl_TexCoord[0].st);
	//color+= texture3D(pencilTexture, vec3(5 * tempRot), gl_TexCoord[0].st);

	//TODO
	if(zBuf.b < 1.0)
	{
		if(sum < 0.4)
			gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		else
			gl_FragColor = color;
	}
	else
		gl_FragColor = vec4(1.0 - sqrt((horizontalEdge*horizontalEdge) + (verticalEdge*verticalEdge)));
	//gl_FragColor = color;
}