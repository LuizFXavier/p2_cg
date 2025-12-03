//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: RayTracer.h
// ========
// Class definition for simple ray tracer.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#ifndef __RayTracer_h
#define __RayTracer_h

#include <iostream>

#include "geometry/Intersection.h"
#include "graphics/Image.h"
#include "graphics/PrimitiveBVH.h"
#include "graphics/Renderer.h"

namespace cg
{ // begin namespace cg

struct PixelBuffer{
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;
  std::uint8_t state{};
};

/////////////////////////////////////////////////////////////////////
//
// RayTracer: simple ray tracer class
// =========
class RayTracer: public Renderer
{
public:
  static constexpr auto minMinWeight = float(0.001);
  static constexpr auto maxMaxRecursionLevel = uint32_t(20);
  static constexpr auto maxSubdivision = 4;
  static constexpr auto maxSteps = 1 << maxSubdivision;
  static constexpr auto maxAdaptativeDistance = 1.f;

  RayTracer(SceneBase&, Camera&);

  auto minWeight() const
  {
    return _minWeight;
  }

  void setMinWeight(float w)
  {
    _minWeight = math::max(w, minMinWeight);
  }

  auto maxRecursionLevel() const
  {
    return _maxRecursionLevel;
  }

  void setMaxRecursionLevel(uint32_t rl)
  {
    _maxRecursionLevel = math::min(rl, maxMaxRecursionLevel);
  }

  void update() override;
  void render() override;
  virtual void renderImage(Image&);

private:
  Reference<PrimitiveBVH> _bvh;
  struct VRC
  {
    vec3f u;
    vec3f v;
    vec3f n;

  } _vrc;
  float _minWeight;
  uint32_t _maxRecursionLevel;
  uint64_t _numberOfRays;
  uint64_t _numberOfHits;
  Ray3f _pixelRay;
  float _Vh;
  float _Vw;
  float _Ih;
  float _Iw;

  PixelBuffer* lineBuffer = nullptr;
  PixelBuffer gridBuffer[(maxSteps + 1)][(maxSteps + 1)];

  float _adaptativeDistance;
  int _subDivisionLevel;
  int vezesQueSabo = 0;
  int consultas = 0;
  int macaco = 0;

  void scan(Image& image);
  void setPixelRay(float x, float y);
  Color shoot(float x, float y);
  bool intersect(const Ray3f&, Intersection&);
  Color trace(const Ray3f& ray, uint32_t level, float weight, float ior = 1.f);
  Color shade(const Ray3f&, Intersection&, uint32_t, float, float ior = 1.f);
  bool shadow(const Ray3f&);
  Color background() const;

  void adaptativeScan(Image& image);
  Color adaptativeColor(int i, int j, float x, float y, int step, int level);
  void firstSlideGridBuffer(int begin);
  void slideGridBuffer(int begin);
  void clearLastGridColumn();
  void clearFirstGridColumn();
  void copyBottomGridLine(int begin);
  void printGrid(int begin);

  vec3f imageToWindow(float x, float y) const
  {
    return _Vw * (x * _Iw - 0.5f) * _vrc.u + _Vh * (y * _Ih - 0.5f) * _vrc.v;
  }

}; // RayTracer

inline void
RayTracer::firstSlideGridBuffer(int begin){
  
  for(int i = 0; i <= maxSteps; ++i){
    // Cópia da última coluna na primeira
    gridBuffer[i][0] = gridBuffer[i][maxSteps];
  }

  // Reset do restante do grid
  for(int i = 0; i <= maxSteps; ++i)
    for(int j = 1; j <= maxSteps; ++j)
      gridBuffer[i][j].state = 0;
    
}

inline void
RayTracer::slideGridBuffer(int begin){
  
  for(int j = 0; j <= maxSteps; ++j){
    // Cópia da última coluna para a primeira
    gridBuffer[j][0] = gridBuffer[j][maxSteps];

    // Cópia do conteúdo do lineBuffer para a primeira linha
    gridBuffer[0][j] = lineBuffer[begin + j];
  }

  // Reset do restante do grid
  for(int i = 1; i <= maxSteps; ++i)
    for(int j = 1; j <= maxSteps; ++j)
      gridBuffer[i][j].state = 0;
    
}

inline void
RayTracer::clearLastGridColumn(){
  
  for(int i = 0; i <= maxSteps; ++i){
    
    gridBuffer[i][maxSteps].state = 0;
  }
    
}
inline void
RayTracer::clearFirstGridColumn(){
  
  for(int i = 0; i <= maxSteps; ++i){
    
    gridBuffer[i][0].state = 0;
  }
    
}

inline void
RayTracer::copyBottomGridLine(int begin){
  
  for(int j = 0; j <= maxSteps; ++j){
    
    lineBuffer[begin + j] = gridBuffer[maxSteps][j];
  }
    
}

inline void
RayTracer::printGrid(int begin){
  
  for(int i = 0; i <= maxSteps; ++i){
    for(int j = 0; j <= maxSteps; ++j){
      std::cout << (int)gridBuffer[i][j].state << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
  for(int i = 0; i <= maxSteps; ++i)
    std::cout << (int)lineBuffer[begin + i].state << " ";
  
  std::cout << "\n";
}


} // end namespace cg

#endif // __RayTracer_h
