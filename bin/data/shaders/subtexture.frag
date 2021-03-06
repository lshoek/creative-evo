#version 450
#define PI 3.14159265359

uniform sampler2D tex;
uniform vec4 patchRotationMatrix;
uniform vec2 patchLocation;
uniform float patchSize;

in vec2 texcoord_varying;
out vec4 fragColor;
 
mat2 rotate2d(float _angle)
{
	return mat2(
		cos(_angle),-sin(_angle),
		sin(_angle),cos(_angle)
	);
}

void main()
{
	vec2 st = texcoord_varying;

	mat2 rot = mat2(
		patchRotationMatrix[0], 
		patchRotationMatrix[1],
		patchRotationMatrix[2],
		patchRotationMatrix[3]
	);

	//vec2 patch_st = patchLocation + ((st * patchSize) * rot) - patchSize*0.5;
	
	// center the patch and scale
	vec2 st_scaled = st;

	st_scaled -= vec2(0.5); 
	st_scaled *= rot;
	st_scaled *= patchSize;
	st_scaled += patchLocation;

	vec2 st_patch = st_scaled;
	//patch_st += patchLocation * patchSize;

	fragColor = texture(tex, st_patch);
}
