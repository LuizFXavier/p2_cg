#include "RayCaster.h"
#include "math/Vector3.h"

Raycaster::Raycaster(int width, int height) : imageBuffer(width, height) {

    image = std::make_unique<cg::GLImage>(width, height);

}

cg::Ray3f Raycaster::generateRay(const cg::Camera& camera, int x, int y) {
    
    float fov = camera.viewAngle(); 
    float aspect = camera.aspectRatio();
    float d = 1.0f;

    float halfHeight = d * tan(cg::math::toRadians(fov) * 0.5f);
    float halfWidth = halfHeight * aspect;

    float cx = (2.0f * x / image->width() - 1.0f) * halfWidth;
    float cy = (1.0f - 2.0f * y / image->height()) * halfHeight;

    cg::vec3f direction_cam = cg::vec3f{cx, cy, -d}.normalize();

    const auto& camToWorld = camera.cameraToWorldMatrix();
    cg::vec3f directionWorld = camToWorld.transformVector(direction_cam);

    return cg::Ray{ camera.position(), directionWorld };

}

cg::Color Raycaster::shade(Scene& scene, const cg::Ray3f& ray, const Intersection& hit) {

    const auto& material = hit.actor->material;

    cg::Color finalColor = material.ambient * scene.ambientLight;

    auto V = -ray.direction;

    for (const auto& light : scene.lights) {

        auto L = light.position() - hit.point;
        auto lightDistance = L.length();

        L.normalize();

        cg::Ray shadowRay{ hit.point + L * 0.001f, L };

        Intersection shadowHit = scene.intersect(shadowRay);

        if (shadowHit && shadowHit.distance < lightDistance)
            continue;

        float NdotL = std::max(hit.normal.dot(L), 0.0f);

        finalColor += material.diffuse * light.color * NdotL;

        if (NdotL > 0) {

            auto R = 2.0f * hit.normal.dot(L) * hit.normal - L;
            float RdotV = std::max(R.dot(V), 0.0f);
            float specular = pow(RdotV, material.shine);

            finalColor += material.specular * light.color * specular;

        }

    }

    return finalColor;

}

void Raycaster::render(Scene& scene) {

    int w = imageBuffer.width();
    int h = imageBuffer.height();

    scene.camera.setAspectRatio((float)w / h);
    scene.camera.update();

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {

            cg::Ray ray = generateRay(scene.camera, x, y);
            Intersection hit = scene.intersect(ray);
            
            cg::Color finalColor;

            if (hit)
                finalColor = shade(scene, ray, hit);
            
            else
                finalColor = backgroundColor;
            
            imageBuffer(x, y) = finalColor;

        }
    
    image->setData(imageBuffer);
    
}