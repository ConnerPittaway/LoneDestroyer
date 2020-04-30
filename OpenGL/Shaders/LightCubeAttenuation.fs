#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	//Attenuation Elements
	float constant;
	float linear;
	float quadratic;
};
uniform Light light;
uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
	//Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir),0.0);
	
	//Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	
	//Texture Calc
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * (spec * material.specular);  
	
	//Attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	//Output
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}