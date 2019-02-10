#version 420

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUv;

layout(std140) uniform Material
{
    vec3 ka;
    vec3 ks;
} material;

out vec4 finalColor;

uniform sampler2D tex;

const vec3 lightPos = vec3(4.0, 5.0, 5.0);

void main()
{
    vec3 lightDir = normalize(fragPos - lightPos); //normalize(vec3(0.5, -1.0, -2.0));
    vec3 texColor = texture2D(tex, fragUv).rgb;
    
    float diffuse = max(dot(fragNormal, -lightDir), 0.0);

    finalColor.rgb += material.ka + material.ks*0.01;

    finalColor = max(vec4(texColor, 1.0), 0);
}