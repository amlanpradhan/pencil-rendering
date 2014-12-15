uniform sampler2D fbo_tex;
uniform sampler2D pencil_tex;

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
	vec4 objColor = texture2D(fbo_tex, gl_TexCoord[0].st);
	vec4 texColor = texture2D(pencil_tex, gl_TexCoord[0].st);
	float colVal = objColor.r;
	if(objColor.b < 1.0)
		gl_FragColor = texColor;
	else
		gl_FragColor = objColor;
	//gl_FragColor = texture2D(fbo_tex, gl_TexCoord[0].st);
}