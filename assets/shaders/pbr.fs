#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;

struct Light {
    int type;           // 0 = directional, 1 = point, 2 = spot
    vec3 position;      // Used for point lights
    vec3 direction;     // Used for directional lights
    vec3 color;
    float intensity;
    float cutoff;
    float outerCutoff;
};

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emissive;
    float transparency;
    
    // Texture flags
    bool hasAlbedoTexture;
    bool hasMetallicTexture;
    bool hasRoughnessTexture;
    bool hasNormalTexture;
    bool hasLegacySpecular;
};

uniform Material material;
uniform Light lights[4];
uniform int numLights;
uniform vec3 viewPos;

// Texture samplers
uniform sampler2D texture_albedo1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

const float PI = 3.14159265359;

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Material sampling functions
vec3 sampleAlbedo()
{
    if (material.hasAlbedoTexture) {
        vec3 texColor = texture(texture_albedo1, TexCoords).rgb;
        texColor = max(texColor, vec3(0.1));
        return pow(texColor, vec3(2.2)) * material.albedo;
    }
    return material.albedo;
}

float sampleMetallic()
{
    if (material.hasMetallicTexture) {
        return texture(texture_metallic1, TexCoords).b * material.metallic;
    }
    return material.metallic;
}

float sampleRoughness()
{
    if (material.hasRoughnessTexture) {
        return texture(texture_roughness1, TexCoords).g * material.roughness;
    } else if (material.hasLegacySpecular) {
        vec3 specular = texture(texture_specular1, TexCoords).rgb;
        float specularIntensity = dot(specular, vec3(0.299, 0.587, 0.114));
        return mix(0.2, 0.9, 1.0 - specularIntensity);
    }
    return material.roughness;
}

vec3 sampleNormal()
{
    if (material.hasNormalTexture) {
        vec3 normal = texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0;
        
        vec3 N = normalize(Normal);
        vec3 T = normalize(Tangent);
        vec3 B = normalize(Bitangent);
        mat3 TBN = mat3(T, B, N);
        
        return normalize(TBN * normal);
    }
    return normalize(Normal);
}

void main()
{
    vec3 albedo = sampleAlbedo();
    float metallic = sampleMetallic();
    float roughness = sampleRoughness();
    vec3 N = sampleNormal();
    
    vec3 V = normalize(viewPos - FragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    
    // Calculate lighting contribution from each light
    for(int i = 0; i < numLights && i < 4; ++i) {
        vec3 L;
        vec3 radiance;
        
        if (lights[i].type == 0) { // Directional light
            L = normalize(-lights[i].direction);
            radiance = lights[i].color * lights[i].intensity;
        } else if (lights[i].type == 1) { // Point light
            L = normalize(lights[i].position - FragPos);
            float distance = length(lights[i].position - FragPos);
            float attenuation = 1.0 / (distance * distance);
            radiance = lights[i].color * lights[i].intensity * attenuation;
        } else if (lights[i].type == 2) { // Spot light
            vec3 lightDir = normalize(lights[i].position - FragPos);
            float distance = length(lights[i].position - FragPos);
            float attenuation = 1.0 / (distance * distance);

            float theta = dot(lightDir, normalize(-lights[i].direction));
            float epsilon = lights[i].cutoff - lights[i].outerCutoff;
            float intensity = clamp((theta - lights[i].outerCutoff) / epsilon, 0.0, 1.0);

            L = lightDir;
            radiance = lights[i].color * lights[i].intensity * attenuation * intensity;
        }
                
        vec3 H = normalize(V + L);
        
        // PBR calculations
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.1) * albedo * material.ao;
    vec3 color = ambient + Lo + material.emissive;

    // HDR tonemapping and gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, material.transparency);
}