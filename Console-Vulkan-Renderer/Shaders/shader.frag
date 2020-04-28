#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vPosition;

layout(binding = 1) uniform sampler2D texSampler;

vec4 lightPos = vec4(0.0, 1.0, 0.0, 1.0);
vec4 lightCol = vec4(1.0, 1.0, 1.0, 1.0);

vec4 phongCalc()
{
	vec4 P = vec4(vPosition, 1.);
	vec3 N = vNormal;
	vec3 L = lightPos.xyz;
	vec3 V = -P.xyz;

	N = normalize(N);
	L = normalize(L);
	V = normalize(V);

	vec3 R = reflect(-L, N);

	vec3 diffuse = max(dot(N,L), 0.) * lightCol.xyz;
	vec3 specular = pow(max(dot(R,V), 0.), 16) * diffuse;

	vec4 ambient = vec4(.01, .01, .01, .01);

	vec4 total = ambient + vec4((diffuse + specular), diffuse);

	return total;
	//return vec4(diffuse, 1.);
	//return vec4(vNormal, 1.);
	//return vec4(specular, 1.f);
}


void main()
{
	//outColor = texture(texSampler, fragTexCoord);
	//outColor = vec4(fragTexCoord, 0.0, 1.0);
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
	//outColor = vec4(vNormal, 1.0);
	outColor = phongCalc() * texture(texSampler, fragTexCoord);
}