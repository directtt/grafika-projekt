#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;

uniform mat4 LightVP;
uniform mat4 sleighLightVP;

out vec3 vecNormal;
out vec3 worldPos;
out vec4 sunSpacePos;
out vec4 sleighLightSpacePos;

uniform vec3 lightPos[8];
uniform vec3 spotlightPos;
uniform vec3 cameraPos;
uniform vec3 sunDir;

out vec3 viewDirTS;
out vec3 lightDirTS[8];
out vec3 spotlightDirTS;
out vec3 sunDirTS;

void main()
{
	worldPos = (modelMatrix* vec4(vertexPosition,1)).xyz;
	vecNormal = (modelMatrix* vec4(vertexNormal,0)).xyz;
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	
	vec3 w_tangent = normalize(mat3(modelMatrix)*vertexTangent);
	vec3 w_bitangent = normalize(mat3(modelMatrix)*vertexBitangent);
	mat3 TBN = transpose(mat3(w_tangent, w_bitangent, vecNormal));
	
	vec3 V = normalize(cameraPos-worldPos);
	viewDirTS = TBN*V;

	for (int i = 0; i < 8; i++) {
		vec3 L = normalize(lightPos[i]-worldPos);
		lightDirTS[i] = TBN*L;
	}
	
	vec3 SL = normalize(spotlightPos-worldPos);
	spotlightDirTS = TBN*SL;
	sunDirTS = TBN*sunDir;

	sunSpacePos=LightVP*modelMatrix*vec4(vertexPosition,1);
	sleighLightSpacePos=sleighLightVP*modelMatrix*vec4(vertexPosition,1);
}
