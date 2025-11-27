#pragma once

#include "geometry/Ray.h"

cg::Color
fresnelSchlick(cg::Color Os, cg::vec3f L, cg::vec3f H) {

    float LdotH = std::clamp(L.dot(H), 0.f, 1.f); // clamp foi necessário pra não ficar artefato na tela

    return Os + (cg::Color(1.f, 1.f, 1.f) - Os) * pow(1.0 - LdotH, 5.0);
}

float
G1(cg::vec3f& N, cg::vec3f& X, float k) {

    float NdotX = cg::math::max(N.dot(X), 0.f);

    return NdotX / (NdotX * (1.0 - k) + k);

}

float
G(cg::vec3f& N, cg::vec3f& L, cg::vec3f& V, float r) {

    // k = (r + 1)^2 / 8 constante rugosidade para luz pontual direta
    float k = ((r + 1.f) * (r + 1.f)) / 8.f;

    return G1(N, L, k) * G1(N, V, k);

}

float
distNormMicrogeometry(cg::vec3f& N, cg::vec3f& H, float r) {

    float a = r * r; 

    float a2 = a * a;

    float NdotH = cg::math::max(H.dot(N), 0.f);

    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);

    return a2 / (cg::math::pi<float> * denom * denom);

}