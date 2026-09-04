#version 330 core

struct Material {
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D emissionMap;

	float shininess;
};

struct DirectionalLight {
	vec4 lightVector ;

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

struct SpotLight {
	vec3 position; 
	vec3 direction; 
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float cutoff;
}; 

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;
uniform vec3 tint;

uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform vec3 cameraPos;

vec3 customRelfect(vec3 nIncidentDir, vec3 nNormal){
	vec3 projLightOnNormal = dot(nIncidentDir, nNormal) * nNormal;
	vec3 reflectProj = 2 * projLightOnNormal; // if this doesn't make sense think about what is happening geometrically
	return nIncidentDir - reflectProj;
}

//Blin Phong lighting calculates specular light better than Phong becuase it doesnt remove specular contributions at extreme angles
vec3 calculateBlinPhong(vec3 lightDir){
	// ambient
	vec3 finalAmbientColor = vec3(texture(material.diffuseMap, TexCoords)) * directionalLight.ambient;

	// diffuse
	vec3 nNormal = normalize(worldNormal);
	vec3 finalDiffuseColor = vec3(texture(material.diffuseMap, TexCoords)) * directionalLight.diffuse * max(dot(nNormal, lightDir), 0.0);

	// specular
	vec3 nViewDir = normalize(cameraPos - worldPosition);
	vec3 nHalfwayDir = normalize(lightDir + nViewDir);
	float specular = pow(max(dot(nNormal, nHalfwayDir), 0.0), material.shininess);
	vec3 finalSpecularColor = specular * vec3(texture(material.specularMap, TexCoords)) * directionalLight.specular;

	//emission
	vec3 colorEmissionMap = texture(material.emissionMap, TexCoords).rgb;
	vec3 grayscaleEmissionMap = vec3(dot(colorEmissionMap, vec3(0.299, 0.587, 0.114)));
	vec3 emission = grayscaleEmissionMap * tint;

	// sum up result
	vec3 result = finalAmbientColor + finalDiffuseColor + finalSpecularColor;
	return result;
}

vec3 calculateDirectionalLight(DirectionalLight directionalLight){
	//calculate light dir based on if given light pos or dir
	vec3 lightDir;
	if (directionalLight.lightVector.w == 0.0){
		lightDir = normalize(-directionalLight.lightVector.xyz);
	}
	else{
		lightDir = normalize(directionalLight.lightVector.xyz - worldPosition);
	}

	return calculateBlinPhong(lightDir);
}

vec3 calculatePointLight(PointLight pointLight){
	float lightDistance = length(pointLight.position - worldPosition);
	vec3 lightDir = normalize(pointLight.position - worldPosition);

	// attenuation so that light gets dimmer the further away an object is to a light source
	float attenuation = 1.0 / (pointLight.constant + (pointLight.linear * lightDistance) + (pointLight.quadratic * lightDistance * lightDistance));

	return calculateBlinPhong(lightDir) * attenuation;
}


vec3 calculateSpotLight(SpotLight spotLight){
	vec3 fragDirection = normalize(worldPosition - spotLight.position);
	float angle = dot(normalize(spotLight.direction), fragDirection);
	float normalDotLight = dot(worldNormal, -fragDirection);
	
	vec3 result;
	if(normalDotLight >= 0.0 && angle > spotLight.cutoff){
		result = vec3(texture(material.diffuseMap, TexCoords));
	} 
	else {
		result = spotLight.ambient;
	}

	return result;
}



void main(){
	
	//vec3 result = calculateDirectionalLight(directionalLight);
	//vec3 result = calculatePointLight(pointLight);
	vec3 result = calculateSpotLight(spotLight);

	FragColor = vec4 (result, 1.0);
}