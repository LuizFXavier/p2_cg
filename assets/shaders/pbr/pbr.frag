#version 330 core

#define PI 3.1415926535897932384626433832795

in vec3 vPos;
in vec3 vNormal;
out vec4 fragmentColor;

struct Light { vec4 position; vec4 color; };
struct Material { vec3 Od; vec3 Os; float r; float m; };

#define NL 3
uniform Light uLights[NL];
uniform Material uMaterial;

// refletância de fresnel com aproximacão de schlick
// F(Ll, Hl) = Os + (1 - Os) * (1 - (Ll . Hl))^5    
vec3 fresnelSchlick(vec3 Os, vec3 L, vec3 H) {

    float LdotH = clamp(dot(L, H), 0.0, 1.0); // clamp foi necessario pra não ficar aterfato na tela

    return Os + (vec3(1.0, 1.0, 1.0) - Os) * pow(1.0 - LdotH, 5.0);
}

// termo G1 da função de G
// G1(*) = N . * / (N . *)(1 - k) + k
float G1(vec3 N, vec3 X, float k) {

    float NdotX = max(dot(N, X), 0.0);

    return NdotX / (NdotX * (1.0 - k) + k);

}

// função do termo G na equacão 2
// G(Ll, V) = G1(Ll)G1(V)
float G(vec3 N, vec3 L, vec3 V, float r) {

    // k = (r + 1)^2 / 8 constante rugosidade para luz pontual direta
    float k = ((r + 1.0) * (r + 1.0)) / 8.0;

    return G1(N, L, k) * G1(N, V, k);

}

// distribuicão normal de microgeometria 
// D(Hl) = a²/ π[(N · Hl)²(a² - 1) + 1]2
float distNormMicrogeometry(vec3 N, vec3 H, float r) {

    float a = r * r; 

    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0);

    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);

    return a2 / (PI * denom * denom);

}

void main() {

    vec3 N = normalize(vNormal);
    vec3 V = normalize(-vPos); 
    vec3 Lo = vec3(0.0, 0.0, 0.0);

    vec3 Od = uMaterial.Od;
    vec3 Os = uMaterial.Os;
    float r = uMaterial.r;
    float m = uMaterial.m;

    // se for metal m = 1, não há reflexão difusa então Od = 0
    Od = mix(Od, vec3(0.0, 0.0, 0.0), m);

    // parcela difusa lambertiana
    vec3 fd = Od / PI;

    // somatória da equação 1
    for(int i = 0; i < NL; ++i) {

        // cálculo do vetor de luz Ll e distância dl
        vec3 L_dir = uLights[i].position.xyz - vPos;
        float dist = length(L_dir);
        vec3 L = normalize(L_dir);
        
        // intensidade da Luz Ll na equação 1 com decaimento
        // Ll = Cl / dl^fl assumindo fl = 2
        float attenuation = 1.0 / (dist * dist);
        vec3 Li = uLights[i].color.rgb * attenuation;

        float NdotL = max(dot(N, L), 0.0);

        float NdotV = max(dot(N, V), 0.0);

        if (NdotL > 0.0) {

            // vetor halfway Hl
            // Hl = (Ll + V) / ||Ll + V||
            vec3 H = normalize(L + V);

            // calculo dos termos da BRDF especular (F, G, D)
            vec3  F = fresnelSchlick(Os, L, H);
            float _G = G(N, L, V, r);
            float D = distNormMicrogeometry(N, H, r);

            // Parcela da BRDF Especular
            // fs = (F * G * D) / (4 * (N.L) * (N.V))
            float denom = (4.0 * NdotL * NdotV); 
            vec3 fs = (F * _G * D) / max(denom, 0.001);

            // conservação de energia kD = 1 - kS 
            vec3 kS = F;
            vec3 kD = vec3(1.0, 1.0, 1.0) - kS;
            kD *= (1.0 - m); 

            // f(Ll, V) = fd + fs
            vec3 f = (kD * fd) + fs;

            // π * somatorio(Ll * f(Ll, V) * (-N * Ll))
            Lo += (f * Li * NdotL) * PI; 

        }

    }

    // definindo a cor final
    fragmentColor = vec4(Lo, 1.0);

}