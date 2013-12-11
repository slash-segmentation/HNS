#ifndef _n3_cv_shape_h_
#define _n3_cv_shape_h_

#include "util/image_util.h"

namespace n3 {

  template <typename TImage> void 
    downSample (Points3& pout, Points3 const& pin, float sampleFactorX, 
		float sampleFactorY, float sampleFactorZ, 
		typename TImage::Pointer canvas)
    {
      int w = getw<TImage>(canvas), h = geth<TImage>(canvas), 
	d = getd<TImage>(canvas);
      float dx[6] = {sampleFactorX, 0, -sampleFactorX, 0, 0, 0};
      float dy[6] = {0, sampleFactorY, 0, -sampleFactorY, 0, 0};
      float dz[6] = {0, 0, 0, 0, sampleFactorZ, -sampleFactorZ};
      canvas->FillBuffer(0);
      setvs<TImage>(canvas, pin, 1);
      std::queue<Point3> openSet;
      Point3 p = pin.front();
      openSet.push(p);
      while (openSet.size() > 0) {
	p = openSet.front();
	openSet.pop();
	pout.push_back(p);
	for (int i = 0; i < 6; ++i) {
	  float x = p.x + dx[i], y = p.y + dy[i], z = p.z + dz[i];
	  if (x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d && 
	      getv<TImage>(canvas, x, y, z) == 1) {
	    openSet.push(Point3(x, y, z));
	    setv<TImage>(canvas, x, y, z, 2);
	  }
	}
      }
    }

};

#endif
