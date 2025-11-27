#pragma once

#include "graphics/GLRenderer.h"
#include "graphics/GLProgram.h"
#include "Scene.h"
#include "PBRMaterial.h"

class PBRRenderer {

public:
    
    PBRRenderer();
    ~PBRRenderer() = default;

    void initialize();

    void render(const Scene& scene);

    void setImageSize(int w, int h);

private:
    
    cg::GLSL::Program* program = nullptr;

    const char* vertexShader;

    const char* fragmentShader;

    void setLightUniforms(const std::vector<LightPBR>& lights, const cg::mat4f& viewMatrix);
    void setMaterialUniforms(const PBRMaterial& mat);

};