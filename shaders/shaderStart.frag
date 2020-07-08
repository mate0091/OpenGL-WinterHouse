//pass-through fragment shader for OpenGL lighting
#version 410 core

in vec3 vNorm;
in vec2 texCoord;
uniform sampler2D diffuseTexture;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 viewDir;
uniform vec3 viewPos;

in vec3 fragPos;

vec4 color;

out vec4 fColor;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform int pointLightOn;
uniform int dirLightOn;
uniform int fogOn;
uniform int wireFrameOn;

float ambientStrength = 0.1f;
float specularStrength = 0.1f;
float shininess = 128.0f;

vec4 fogColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

struct PointLight
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;

	vec3 color;
};

PointLight p1 = PointLight(vec3(-7.52, 3.8, 2.16), 1.0, 0.35, 0.44, vec3(1, 0.8, 0.3));
PointLight p2 = PointLight(vec3(6.59, 3.8, 3.04), 1.0, 0.35, 0.44, vec3(1, 0.6, 0.3));

vec4 pointLight(PointLight p)
{
	vec3 lightDirN = p.position - fragPos;
	lightDirN = normalize(lightDirN);
	float diff = max(dot(vNorm, lightDirN), 0.0);

	float dist = length(p.position - fragPos);
	float att = 1.0 / (p.constant + p.linear * dist + p.quadratic * (dist * dist));

	vec3 baseColor = texture(diffuseTexture, texCoord).rgb;
	vec3 ambient = p.color * baseColor;
	vec3 diffuse = p.color * diff * baseColor;
	ambient *= att;
	diffuse *= att;

	return (vec4(ambient + diffuse, 1.0f));
}

vec4 phongDirectional()
{
	vec3 ambient = ambientStrength * lightColor;
	vec3 baseColor = texture(diffuseTexture, texCoord).rgb;

	vec3 lightDirN = normalize(-lightDir);
	vec3 normalEye = normalize(normalMatrix * vNorm);

	vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	vec3 viewDirN = normalize(viewDir - fragPos.xyz);
	vec3 halfVec = normalize(lightDirN + viewDirN);

	float specCoeff = pow(max(dot(normalEye, halfVec), 0.0f), shininess);
	vec3 specular = specularStrength * specCoeff * lightColor;

	color = vec4(min((ambient + diffuse) * baseColor + specular, 1.0f), 1.0f);
	//compute final vertex color
	return color;
}

float fog()
{
	float fogDensity = 0.05f;
	float fragmentDistance = length(fragPos - viewPos);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
	//fColor = phongDirectional();// + pointLight(p1) + pointLight(p2);
	//fColor = pointLight(p1);

	//fColor = pointLight(p2) * pointLight2On;

	vec4 tempColor = (dirLightOn * phongDirectional()) + ((pointLight(p1) + pointLight(p2)) * pointLightOn);

	if(fogOn == 1)
	{
		float fogFac = fog();
		tempColor = fogColor * (1 - fogFac) + tempColor * fogFac;
	}

	if(wireFrameOn == 0) fColor = tempColor;
	else
	{
		fColor = vec4(0, 0, 0, 1);
	}
}
