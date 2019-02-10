#version 420

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUv;

out vec4 finalColor;

uniform sampler2D depthTex;
uniform sampler2D randTex;
uniform mat4 proj;

mat3 toCamSpaceMat()
{
	vec3 randDir = normalize(texture(randTex, fragUv).xyz);
	vec3 n = normalize(fragNormal);
	vec3 biTang = cross(n, randDir);
	if(length(biTang) < 0.00001)
		biTang = vec3(0.0, 0.0, 1.0);
	biTang = normalize(biTang);
	vec3 tang = normalize(cross(biTang, n));
	return mat3(tang, biTang, n);
}

const float kernelSize = 64;
uniform vec3 sampleKernel[64];
const float radius = 0.55;

float aoPass(mat3 toCamSpace)
{
	float occlusionSum = 0.0;
	for(int i = 0; i < kernelSize; i++)
	{
		// Sample a point from current poistion adound a hemisphere.
		vec3 samplePos = fragPos + radius * (toCamSpace * sampleKernel[i]);

		// Project the sampled point onto screen.
		vec4 p = proj * vec4(samplePos, 1.0);
		p *= 1.0 / p.w;

		// Convert position from [-1, 1] to [0, 1].
		p.xyz = p.xyz * 0.5 + 0.5;

		float surfaceZ = texture(depthTex, p.xy).r;
		float dz = surfaceZ - fragPos.z;

		if(dz >= 0.0)
			occlusionSum += 1.0;
	}
	int index = int(63.0*(fragPos.x+1.0)*0.5 + (63.0*(fragPos.y+1.0)*0.5)*64.0);
	vec3 samplePos2 = fragPos + radius * (toCamSpace * sampleKernel[index]);
	finalColor = vec4(sampleKernel[index], 1.0);
	return 1.0 - occlusionSum / kernelSize;
}


void main()
{
    mat3 toCamSpace = toCamSpaceMat();
	float aoFactor = aoPass(toCamSpace);
	//finalColor = vec4(aoFactor, aoFactor, aoFactor, 1.0);
}