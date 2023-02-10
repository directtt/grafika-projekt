#version 430 core

float AMBIENT = 0.03;
float PI = 3.14;

uniform sampler2D depthMap;
uniform sampler2D sleighDepthMap;

uniform vec3 cameraPos;

uniform vec3 sunDir;
uniform vec3 sunColor;

uniform vec3 lightPos[8];
uniform vec3 lightColor[8];

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;
uniform vec3 spotlightPhi;

uniform float alpha;
uniform float metallic;
uniform float roughness;

uniform float exposition;

in vec3 vecNormal;
in vec3 worldPos;

in vec4 sunSpacePos;
in vec4 sleighLightSpacePos;

out vec4 outColor;


in vec3 viewDirTS;
in vec3 lightDirTS[8];
in vec3 spotlightDirTS;
in vec3 sunDirTS;

in vec3 test;
in vec2 texCoord;

uniform sampler2D colorTexture;
vec3 color;


float DistributionGGX(vec3 normal, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 normal, vec3 V, vec3 lightDir, float roughness){
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

vec3 PBRLight(vec3 lightDir, vec3 radiance, vec3 normal, vec3 V){
	float diffuse=max(0,dot(normal,lightDir));

	//vec3 V = normalize(cameraPos-worldPos);
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, color, metallic);

    vec3 H = normalize(V + lightDir);    
        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, V, lightDir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);   
	return (kD * color / PI + specular) * radiance * NdotL;
}

float generateShadow(vec3 normal, vec3 lightDir) {
	vec4 sunSpacePosNormalized =  (0.5 * sunSpacePos / (sunSpacePos.w)) + 0.5;

    float closestDepth = texture2D(depthMap, sunSpacePosNormalized.xy).r;

    float bias = max(0.01*(1.0-dot(normal,lightDir)),0.001);

    if (closestDepth + bias > sunSpacePosNormalized.z)
    {
        return 1.0;
    }

    return 0.0;
}

float generateShadowSleigh(vec3 normal, vec3 spotLightDir) {
    vec4 sleighLightSpacePosNormalized =  (0.5 * sleighLightSpacePos / (sleighLightSpacePos.w)) + 0.5;

    float closestDepth = texture2D(sleighDepthMap, sleighLightSpacePosNormalized.xy).r;

    float bias = max(0.02*(1.0-dot(normal,spotLightDir)),0.002);

    if (closestDepth + bias > sleighLightSpacePosNormalized.z)
    {
        return 1.0;
    }

    return 0.0;
}

void main()
{
    // based on https://youtu.be/95WAAYsOifQ
    vec3 totalIllumination = vec3(0.0);

    for (int i=0; i<8; i++) {
        vec4 textureColor = texture2D(colorTexture, texCoord);
        color = textureColor.xyz;

        vec3 normal = normalize(vecNormal);

        vec3 viewDir = normalize(cameraPos-worldPos);

	    vec3 lightDir = normalize(lightPos[i]-worldPos);


	    vec3 ambient = AMBIENT*color;
	    vec3 attenuatedlightColor = lightColor[i]/pow(length(lightPos[i]-worldPos),2);
	    vec3 ilumination;
	    ilumination = ambient+PBRLight(lightDir,attenuatedlightColor,normal,viewDir);
	
	    //flashlight
	    vec3 spotlightDir= normalize(spotlightPos-worldPos);
	

        float angle_atenuation = clamp((dot(-normalize(spotlightPos-worldPos),spotlightConeDir)-0.5)*3,0,1);
	    attenuatedlightColor = angle_atenuation*spotlightColor/pow(length(spotlightPos-worldPos),2);
	    ilumination=ilumination+PBRLight(spotlightDir,attenuatedlightColor*generateShadowSleigh(normal,spotlightDir),normal,viewDir);

	    //sun
	    ilumination=ilumination+PBRLight(sunDir,sunColor*generateShadow(normal, lightDir),normal,viewDir);
        totalIllumination = totalIllumination + ilumination;
    }
   
	outColor = vec4(vec3(1.0) - exp(-totalIllumination*exposition), alpha);
}