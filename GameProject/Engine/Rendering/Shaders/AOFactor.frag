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
	vec4 p2 = proj * vec4(fragPos, 1.0);
	p2 *= 1.0 / p2.w;
	p2.xyz = p2.xyz * 0.5 + 0.5;
	vec2 screenPos = p2.xy;

	vec3 randDir = normalize(texture(randTex, screenPos).xyz);
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
	vec4 p2 = proj * vec4(fragPos, 1.0);
	p2 *= 1.0 / p2.w;
	p2.xyz = p2.xyz * 0.5 + 0.5;
	vec2 screenPos = p2.xy;

	mat4 projInv = inverse(proj);

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
		vec4 p3 = vec4(screenPos.x, screenPos.y, surfaceZ, 1.0);
		float dz = (projInv*p3).z - fragPos.z;

		if(dz <= 0.0)
			occlusionSum += 1.0;
	}
	float d = texture(depthTex, screenPos.xy).r;
	float viewZ = proj[3][2] / (2.0 *d - 1.0 - proj[2][2]);
	vec3 viewPos = vec3(viewZ);

	/*
	float d = texture(depthTex, screenPos.xy).r;
	vec2 pndc = 2.0*screenPos - 1.0;
	vec3 pray = (projInv * vec4(pndc, 1.0, 1.0)).xyz;
	vec3 viewPos = d*100.0*pray;
	*/

	/*
	vec4 p4 = vec4(2.0*screenPos.x-1.0, 2.0*screenPos.y-1.0, surfaceZ2, 1.0);
	p4.w = proj[3][2] / (p4.z - (proj[2][2] / proj[2][3]));
	p4.xyz = p4.xyz * p4.w;
	vec4 pz = (projInv*p4);
	vec3 viewPos = pz.xyz;*/

	finalColor = vec4(viewPos, 1.0);

	//int index = int(floor(63.0*screenPos.x) + floor((63.0*screenPos.y)*64.0));
	//vec3 samplePos2 = fragPos + radius * (toCamSpace * sampleKernel[index]);
	//finalColor = vec4((sampleKernel[index]+1.0)*0.5, 1.0);
	return 1.0 - occlusionSum / kernelSize;
}


void main()
{
    mat3 toCamSpace = toCamSpaceMat();
	float aoFactor = aoPass(toCamSpace);
	//finalColor = vec4(aoFactor, aoFactor, aoFactor, 1.0);
}