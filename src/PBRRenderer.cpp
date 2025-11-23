#include "PBRRenderer.h"
#include "Actor.h"
#include "graphics/Camera.h"
#include "graphics/GLRenderWindow3.h"
#include "FileReader.h"
#include "graphics/Light.h"
#include "vector"

PBRRenderer::PBRRenderer() {

    fragmentShader = FileReader::read("../assets/shaders/pbr/pbr.frag");

    vertexShader = FileReader::read("../assets/shaders/pbr/pbr.vert");

}

void PBRRenderer::initialize() {

    program = new cg::GLSL::Program("PBR shader");

    program->setShaders(vertexShader, fragmentShader);    

}

void PBRRenderer::setImageSize(int w, int h) {
    glViewport(0, 0, w, h);
}

void PBRRenderer::render(const Scene& scene) {

    if (!program) 
        return;

    program->use();
    
    cg::mat4f viewMatrix;
    scene.camera.cameraToWorldMatrix().inverse(viewMatrix);

    cg::mat4f projectionMatrix = scene.camera.projectionMatrix();
    
    cg::mat4f vpMatrix = projectionMatrix * viewMatrix;

    setLightUniforms(scene.lights, viewMatrix);

    for (auto& actor : scene.getActors()) {
        
        if (!actor || !actor->mesh) 
            continue;

        cg::mat4f modelMatrix = actor->getTransform();

        cg::mat4f mvMatrix = viewMatrix * modelMatrix;  
        cg::mat4f mvpMatrix = vpMatrix * modelMatrix;   
        
        cg::mat3f nMatrix;
        cg::mat4f invMV;

        if (mvMatrix.inverse(invMV)) 
            nMatrix = cg::mat3f(invMV).transposed();
        

        program->setUniformMat4("mvMatrix", mvMatrix);
        program->setUniformMat4("mvpMatrix", mvpMatrix);
        program->setUniformMat3("nMatrix", nMatrix);

        setMaterialUniforms(actor->material);

        actor->mesh->bind();

        glDrawElements(
            GL_TRIANGLES, 
            actor->mesh->vertexCount(), 
            GL_UNSIGNED_INT, 
            0
        );

    }

}

void PBRRenderer::setLightUniforms(const std::vector<cg::Light>& lights, const cg::mat4f& viewMatrix) {

    int count = 0;

    for (auto& light : lights) {
        
        cg::vec4f posWorld = {
            light.position().x, 
            light.position().y, 
            light.position().z, 
            1.0f
        };

        cg::vec4f posView = viewMatrix * posWorld;

        std::string base = "uLights[" + std::to_string(count) + "]";

        cg::vec4f lightColor ={
            light.color.r, 
            light.color.g, 
            light.color.b, 
            1.0f
        };


        program->setUniformVec4(
            (base + ".position").c_str(), 
            posView
        );

        program->setUniformVec4(
            (base + ".color").c_str(), 
            lightColor
        );
       
        count++;

    }

}

// TODO: quando tiver GUI mudar o uMaterial.m 
void PBRRenderer::setMaterialUniforms(const cg::Material& mat) {

    program->setUniformVec3("uMaterial.Od", mat.diffuse);
    program->setUniformVec3("uMaterial.Os", mat.specular);
    program->setUniform("uMaterial.r", mat.shine); 
    program->setUniform("uMaterial.m", 0.0f);     
  
}