varying vec2 myTexCoord;
varying vec4 position;
void main()
{
	gl_Position = ftransform();
    myTexCoord = vec2(gl_MultiTexCoord0);
    position = gl_Position;
}