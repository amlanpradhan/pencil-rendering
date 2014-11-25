uniform sampler2D textureSrc;
uniform sampler2D curvatureTexture;
uniform sampler3D pencilTexture;

vec2 rotateDir(vec2 orgnVec, vec2 dir)
{
	return vec2(((orgnVec.x*dir.x)-(orgnVec.y*dir.y)),
				((orgnVec.x*dir.y)+(orgnVec.y*dir.x)));
}