#version 410 core

#define MAX_LIGHTS 15

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 FragPosLight;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_LIGHTS];
uniform int numPointLights;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform samplerCube depthCubemap;
uniform float far_plane;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    if (!gl_FrontFacing)
    {
        norm = -norm;
    }

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for (unsigned int i = 0; i < numPointLights; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, texture(material.diffuse, TexCoords).a);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal); 

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = diff != 0 ? pow(max(dot(normal, halfwayDir), 0.0), material.shininess) : 0;

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    float shadow = 0.0;
	vec3 lightCoords = FragPosLight.xyz / FragPosLight.w;
	if(lightCoords.z <= 1.0)
	{
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		float bias = max(0.025 * (1.0 - dot(normal, lightDir)), 0.0005);
		int sampleRadius = 5;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}

    return (ambient + (diffuse + specular) * (1.0 - shadow));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = diff != 0 ? pow(max(dot(normal, halfwayDir), 0.0), material.shininess) : 0;
    
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec *  texture(material.specular, TexCoords).rgb;

    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.05;
    float samples = 4.0;
    float offset = 0.1;
    
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(depthCubemap, fragToLight + vec3(x, y, z)).r;
                closestDepth *= far_plane;
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);

    return (ambient + (diffuse + specular) * (1.0 - shadow)) * attenuation;
}