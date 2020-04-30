#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINTS_LIGHTS 1
uniform PointLight pointLights[NR_POINTS_LIGHTS];

struct Spotlight {
	vec3 position;
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	//Attenuation Elements
	float constant;
	float linear;
	float quadratic;
	
	//Cutoffs
	float cutOff;
	float outerCutOff;
};
uniform Spotlight spotLight;

uniform Material material;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

//Prototypes
vec3 CalcDirLight(DirLight light, vec3 Normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 Normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(Spotlight light, vec3 Normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	//Directional Lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	//Point Lighting
	for(int i = 0; i < NR_POINTS_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	
	//Spotlight
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
		
	//Output
	FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 Normal, vec3 viewDir)
{
	//Light Direction
	vec3 lightDir = normalize(-light.direction);
	
	//Diffuse
	float diff = max(dot(Normal, lightDir), 0.0);
	
	//Specular
	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	
	//Output
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));  
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 Normal, vec3 fragPos, vec3 viewDir)
{
	//Light Direction
    vec3 lightDir = normalize(light.position - fragPos);
    
	//Diffuse
    float diff = max(dot(Normal, lightDir), 0.0);
	
    //Specular
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    //Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	
    //Output
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

vec3 CalcSpotLight(Spotlight light, vec3 Normal, vec3 fragPos, vec3 viewDir)
{
	//Light Direction
    vec3 lightDir = normalize(light.position - fragPos);
	
	//Diffuse
    float diff = max(dot(Normal, lightDir), 0.0);
	
	//Specular
	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	
	vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	
	//Spotlight
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon =(light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;
	
	//Attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	//Output
	return (ambient + diffuse + specular);
}