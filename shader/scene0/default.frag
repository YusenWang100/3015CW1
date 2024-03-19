#version 410

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragColor2;

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
}
vs_out;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;

uniform UBOCamera
{
    mat4 V;
    mat4 P;
    vec3 viewPosition;
    float fogFactor;
    vec3 fogColor;
};

uniform UBOLight {
    // point light
    vec3 lightPosition;
    vec3 lightColor;
    // moving Light
    vec3 movingLightPosition;
    vec3 movingLightColor;
    // directional light
    vec3 directionalLightColor;

    // ambient light
    vec3 ambientColor;

    //spot light
    vec3 spotLightPosition0;
    float spotLightUmbra;     // rad
    vec3 spotLightDirection0;
    float spotLightPenumbra;  // rad
    vec3 spotLightColor;
    vec3 spotLightPosition1;
    vec3 spotLightDirection1;
};
uniform UBOMaterial
{
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 ambient;
    vec3 emissionColor;
    float shininess;
    int enableSpecular;
    int onlyEmission;
    int enableToonShading;
    int useNormalMap;
};

// void main()
// {
//     vec3 col = texture(diffTex, vs_out.texcoord).rgb;
//     outColor = vec4(col, 1);
// }
vec3 computeSpecular(vec3 lightDirection, vec3 viewDirection, vec3 normal) {
    vec3 halfVector = normalize(lightDirection + viewDirection);

    float specular = 0.0;

    float specularIntensity = 1.0;

    // if (useSpecularMap) {
    //     specularIntensity = texture(secondTexture, texcoord).r * 2.0;
    // }

    specular = pow(max(0.0, dot(halfVector, normal)), shininess) * specularIntensity;

    return vec3(specular);
}

vec3 computeDiffuse(vec3 albedoColor, vec3 normal, vec3 lightDirection) {
    float diffuse = max(0., dot(normal, lightDirection));

    return vec3(diffuse * albedoColor);
}
float computeSpotLightFalloff(vec3 lightDirection, vec3 spotDirection, float penumbra, float umbra) {
    float cosTheta = dot(-lightDirection, spotDirection);
    float cosThetaUmbra = cos(umbra);
    float cosThetaPenumbra = cos(penumbra);
    return clamp((cosTheta - cosThetaUmbra) / (cosThetaPenumbra - cosThetaUmbra), 0, 1);
}

vec3 toonShading(vec3 lightDirection, vec3 normal) {
    float intensity;
    vec3 color;
    intensity = max(0.0, dot(lightDirection, normal));

    if (intensity > 0.7)
        color = vec3(1.0, 0.5, 0.5);
    else if (intensity > 0.5)
        color = vec3(0.6, 0.3, 0.3);
    else if (intensity > 0.25)
        color = vec3(0.4, 0.2, 0.2);
    else
        color = vec3(0.2, 0.1, 0.1);
    return color;
}

void main() {
    vec4 albedoColor = texture(albedoMap, vs_out.texcoord);
    if(albedoColor.a<0.5)discard;

    mat3 TBN = mat3(vs_out.tangent, vs_out.bitangent, vs_out.normal);

    vec3 lightDirection0 = normalize(lightPosition - vs_out.position);
    vec3 lightDirection1 = normalize(movingLightPosition - vs_out.position);
    vec3 lightDirection2 = normalize(vec3(-1.0, -1.0, -1.0));
    vec3 lightDirection3 = normalize(spotLightPosition0 - vs_out.position);
    vec3 lightDirection4 = normalize(spotLightPosition1 - vs_out.position);

    float spotLightFalloff0 =
        computeSpotLightFalloff(lightDirection3, spotLightDirection0, spotLightPenumbra, spotLightUmbra);
    float spotLightFalloff1 =
        computeSpotLightFalloff(lightDirection4, spotLightDirection1, spotLightPenumbra, spotLightUmbra);

    vec3 normal = normalize(vs_out.normal);
    if (bool(useNormalMap)) normal = normalize(TBN * (texture(normalMap, vs_out.texcoord).rgb * 2 - 1));

    vec3 diffuse0 = computeDiffuse(albedoColor.rgb, normal, lightDirection0) * lightColor;
    vec3 diffuse1 = computeDiffuse(albedoColor.rgb, normal, lightDirection1) * movingLightColor;
    vec3 diffuse2 = computeDiffuse(albedoColor.rgb, normal, -lightDirection2) * directionalLightColor;
    vec3 diffuse3 = computeDiffuse(albedoColor.rgb, normal, lightDirection3) * spotLightColor * spotLightFalloff0;
    vec3 diffuse4 = computeDiffuse(albedoColor.rgb, normal, lightDirection4) * spotLightColor * spotLightFalloff1;

    vec3 finalColor = diffuse0 * 0.25 + diffuse1 * 0.25 + diffuse2 * 0.5 + diffuse3 + diffuse4;

    if (bool(enableSpecular)) {
        vec3 viewDirection = normalize(viewPosition - vs_out.position);

        vec3 specular0 = computeSpecular(lightDirection0, viewDirection, normal);
        vec3 specular1 = computeSpecular(lightDirection1, viewDirection, normal);
        vec3 specular2 = computeSpecular(-lightDirection2, viewDirection, normal);
        vec3 specular3 = computeSpecular(lightDirection3, viewDirection, normal) * spotLightFalloff0 * spotLightColor;
        vec3 specular4 = computeSpecular(lightDirection4, viewDirection, normal) * spotLightFalloff1 * spotLightColor;

        finalColor += specular0 * 0.25 + specular1 * 0.25 + specular2 * 0.5 + specular3 + specular4;
    }

    fragColor = vec4(finalColor + ambientColor * ambient, albedoColor.a);

    if (bool(onlyEmission)) {
        fragColor = vec4(emissionColor, 1.0);
    }

    if (bool(enableToonShading)) {
        fragColor = vec4(finalColor * toonShading(lightDirection2, normal), 1.0);
    }

    float a = length(viewPosition - vs_out.position) * fogFactor;
    float t = smoothstep(0, 1, exp(-a * a));
    fragColor.rgb = mix(fogColor, fragColor.rgb, t);
    fragColor.a = albedoColor.a;

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1)
        fragColor2 = vec4(fragColor.rgb, 1);
    else
        fragColor2 = vec4(0);
}
