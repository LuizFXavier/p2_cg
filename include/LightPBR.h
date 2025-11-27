#pragma once

#include "graphics/Light.h"
#include <algorithm> 

class LightPBR : public cg::Light {

    public:

        cg::Color finalColor; 

        float intensity;  

        LightPBR() : cg::Light() {

            finalColor = cg::Color::white;

            intensity = 1.0f;

            updateColor();

        }

        void setBaseColor(const cg::Color& color) {

            this->color = color;

            updateColor();

        }

        void setIntensity(float i) {

            intensity = std::max(0.0f, i); 

            updateColor();

        }

        void updateColor() {
            finalColor = color * intensity;
        }

};