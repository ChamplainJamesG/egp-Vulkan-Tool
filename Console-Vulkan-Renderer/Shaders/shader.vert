#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec3 vPosition;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;

	vec4 clipSpace = ubo.view * ubo.model * vec4(inPosition, 1.0);

	vec3 normalizedNormal = mat3(ubo.view) * mat3(ubo.model) * inNormal;
	normalizedNormal /= length(normalizedNormal);
	vNormal = normalizedNormal;
	vPosition = vec3(clipSpace.x, clipSpace.y, clipSpace.z);
}